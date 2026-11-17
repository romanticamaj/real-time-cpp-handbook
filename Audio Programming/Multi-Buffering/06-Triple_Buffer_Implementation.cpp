#include <iostream>
#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>

// SPSC 最新快照交換，可略過中間版本，不是音訊 sample FIFO。
// 原子操作是否 lock-free 取決於平台；不宣稱硬即時或 wait-free 保證。
template<typename T>
class TripleBuffer {
private:
    static constexpr size_t kCacheLineSize = 64;
    static constexpr std::uint32_t kDirty = 4;
    static constexpr std::uint32_t kIndexMask = 3;

    struct alignas(kCacheLineSize) AlignedBuffer {
        T data{};
    };

public:
    TripleBuffer() = default;

    // 僅生產者呼叫；引用只可使用至下一次 Commit。
    [[nodiscard]] T& Write() noexcept {
        return buffers_[back_buffer_].data;
    }

    // 生產者：提交資料。
    void Commit() noexcept {
        // release 發布寫入；acquire 接收消費者歸還的 buffer，
        // 確保其先前的讀取 happens-before 生產者重用該 buffer。
        back_buffer_ = middle_.exchange(back_buffer_ | kDirty,
                                        std::memory_order_acq_rel) & kIndexMask;
    }

    // 僅消費者呼叫；引用只可使用至下一次 Read。
    [[nodiscard]] const T& Read() noexcept {
        // 此 load 只判斷 dirty；取得 payload 的 acquire 在 exchange。
        if (middle_.load(std::memory_order_relaxed) & kDirty) {
            // 只有消費者清除 dirty；期間生產者只能發布另一個 dirty 狀態。
            front_buffer_ = middle_.exchange(front_buffer_,
                                             std::memory_order_acq_rel) & kIndexMask;
        }
        return buffers_[front_buffer_].data;
    }

    // 初始化／非即時路徑查詢；true 也不代表有執行時間上界。
    bool IsLockFree() const noexcept { return middle_.is_lock_free(); }

private:
    std::array<AlignedBuffer, 3> buffers_;
    // 初始 front=0、back=1、middle=2；索引與 dirty 必須一起交換。
    std::atomic<std::uint32_t> middle_{2};

    alignas(kCacheLineSize) std::uint32_t front_buffer_{0};
    alignas(kCacheLineSize) std::uint32_t back_buffer_{1};
};

// --- 使用範例 ---

struct AudioFrame {
    std::array<float, 512> samples;
    std::uint64_t frame_id = 0;
};

int main() {
    TripleBuffer<AudioFrame> buffer;
    // 這是一般執行緒上的 ownership 示範；cout、sleep、join 不屬於 callback。
    std::cout << "middle atomic lock-free: " << buffer.IsLockFree() << '\n';
    std::atomic<bool> stop_flag{false};

    // 生產者執行緒（模擬音訊處理）
    std::thread producer([&]() {
        for (std::uint64_t id = 1;
             id < std::numeric_limits<std::uint64_t>::max() &&
             !stop_flag.load(std::memory_order_relaxed); ++id) {
            AudioFrame& frame = buffer.Write();

            frame.frame_id = id;
            for (size_t i = 0; i < frame.samples.size(); ++i) {
                frame.samples[i] = static_cast<float>(id % 100) * 0.01f;
            }

            buffer.Commit();
            // Commit 後不再使用 frame 引用。

            std::this_thread::sleep_for(std::chrono::microseconds(10600)); // 模擬 512 frame @ 48kHz
        }
    });

    // 消費者執行緒（模擬播放）
    std::thread consumer([&]() {
        std::uint64_t last_frame_id = 0;
    
        while (!stop_flag.load(std::memory_order_relaxed)) {
            const AudioFrame& frame = buffer.Read();

            if (frame.frame_id > last_frame_id) {
                last_frame_id = frame.frame_id;
                // 將執行期間的輸出集中在消費端，避免交錯 logging。
                std::cout << "消費者：正在播放資料幀 " << frame.frame_id << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    stop_flag.store(true, std::memory_order_relaxed);

    producer.join();
    consumer.join();

    return 0;
}
