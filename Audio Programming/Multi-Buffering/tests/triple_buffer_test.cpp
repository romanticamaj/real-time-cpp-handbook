// clang++ -std=c++17 -O2 -pthread triple_buffer_test.cpp -o /tmp/triple_buffer_test
#define main triple_buffer_demo_main
#include "../06-Triple_Buffer_Implementation.cpp"
#undef main

#include <cassert>

struct Snapshot {
    unsigned sequence = 0;
    std::array<unsigned, 64> values{};
};

int main() {
    TripleBuffer<Snapshot> buffer;
    assert(buffer.Read().sequence == 0);
    for (unsigned id = 1; id <= 10; ++id) {
        buffer.Write().sequence = id;
        buffer.Commit();
    }
    assert(buffer.Read().sequence == 10); // 中間版本可被略過。
    assert(buffer.Read().sequence == 10); // 無更新時保留 front。

    // 消費者持有的引用，在下次 Read 前不得被生產者重用。
    const Snapshot& held = buffer.Read();
    std::thread writer([&] {
        for (unsigned id = 11; id <= 10000; ++id) {
            buffer.Write().sequence = id;
            buffer.Commit();
        }
    });
    for (unsigned i = 0; i < 10000; ++i) assert(held.sequence == 10);
    writer.join();
    assert(buffer.Read().sequence == 10000);

    TripleBuffer<Snapshot> concurrent;
    constexpr unsigned iterations = 200000;
    std::atomic<bool> done{false};
    std::thread producer([&] {
        for (unsigned id = 1; id <= iterations; ++id) {
            auto& frame = concurrent.Write();
            frame.sequence = id;
            frame.values.fill(id);
            concurrent.Commit();
            if (id % 31 == 0) std::this_thread::yield();
        }
        done.store(true, std::memory_order_release);
    });
    unsigned last = 0;
    do {
        const auto& frame = concurrent.Read();
        const auto sequence = frame.sequence;
        assert(sequence >= last);
        for (auto value : frame.values) assert(value == sequence);
        last = sequence;
    } while (!done.load(std::memory_order_acquire));
    producer.join();
    assert(concurrent.Read().sequence == iterations);
    std::cout << "PASS: initial, latest, retained snapshot, concurrent consistency\n";
}
