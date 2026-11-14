# C++11 std::lock：用重試與退讓避免 Deadlock

[← Modern C++](README.md) · [Threading 基礎](C%2B%2B11_threading.md)

**核心策略：先 lock 第一把，其餘用 try_lock；失敗就釋放已取得的鎖，再調整順序重試。** 持有部分鎖時，不阻塞等待剩餘的鎖，避免取得過程形成循環等待。

## 1. std::lock 保證什麼？

`std::lock(a, b, ...)` 成功返回時，所有傳入的 Lockable 都已鎖定；標準允許使用不同的 `lock`／`try_lock`／`unlock` 序列，但取得過程不得造成 deadlock。若 lock／try_lock 丟例外，必須釋放這次已取得的鎖。[標準草案](https://eel.is/c++draft/thread.lock.algorithm)

```text
自己逐把阻塞取得：T1 持有 a 等 b；T2 持有 b 等 a → deadlock
重試與退讓：     T1 持有 a，但 try_lock(b) 失敗 → 放掉 a，再嘗試
```

前提：傳入不同且有效的 mutex，呼叫端尚未持有它們；本文示範非遞迴 mutex。這個保證不涵蓋外部鎖、join 或其他等待關係造成的循環。

## 2. libc++：雙鎖版本的兩個方向

[libc++ 原始碼：mutex](https://github.com/llvm/llvm-project/blob/852cf83ea07d7ecdad841d82d3a51beca306cf93/libcxx/include/mutex#L357-L378) 的雙鎖版本交替執行：

```text
lock(a) → try_lock(b) → 成功：保留兩把，返回
                     → 失敗：unlock(a)，yield
lock(b) → try_lock(a) → 成功：保留兩把，返回
                     → 失敗：unlock(b)，yield，再重試
```

實作以 `unique_lock` 管理第一把鎖：try_lock 失敗時離開 scope，由解構子自動 unlock；成功時呼叫 release，避免返回前的解構子把第一把鎖放掉。

## 3. 可執行的簡化版：把 unlock 明寫出來

下例依相同策略重新整理，僅處理**兩個相同型別的 Lockable**；用交換指標取代 libc++ 的兩段對稱程式。這是教學版本，實務優先使用 `std::lock`。

```cpp
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

template<class Mutex>
void lockTwo(Mutex& a, Mutex& b) {
    Mutex* first = std::addressof(a);
    Mutex* second = std::addressof(b);
    for (;;) {
        std::unique_lock<Mutex> owner(*first); // lock：還沒持有任何一把時，可以阻塞等待第一把
        if (second->try_lock()) {             // try_lock：已持有第一把，第二把只嘗試，不阻塞等待
            owner.release();                  // 成功：保留鎖定狀態，把後續解鎖責任交給呼叫端
            return;
        }
        owner.unlock();                       // 失敗：先放掉第一把，讓其他 threads 有機會前進
        std::swap(first, second);             // 下一輪先 lock 剛才取得失敗的那把
        std::this_thread::yield();            // 給排程器安排其他工作之機會；不保證誰一定會執行
    }
}
```

**關鍵 invariant：進入阻塞 lock 時，這次操作沒有持有其他鎖；持有部分鎖時只做 try_lock。** 第二把取得失敗就撤回第一把，打破「各自拿一把、阻塞等另一把」的循環。

若第二把的 try_lock 丟例外，owner 仍會在 stack unwinding 時解鎖第一把；成功路徑則保留兩把，交給呼叫端管理。

## 4. release 與 unlock：這裡為什麼需要 release？

| API | mutex 狀態 | unique_lock 狀態 |
|:---|:---|:---|
| `unlock()` | 解鎖 | 保留 mutex 關聯，owns_lock 為 false，可重新 lock |
| `release()` | 不改變鎖定狀態 | 解除關聯，owns_lock 為 false，回傳原 mutex 指標 |

一般 producer–consumer、LeetCode 同步題，優先熟悉 RAII、wait 與 unlock；release 通常用於 ownership 交接等較底層的設計。**直接 release 後若沒安排解鎖者，mutex 會繼續保持鎖定。**

```cpp
// 以下接在 lockTwo 定義後，放在函式內使用。
std::mutex a, b;
lockTwo(a, b); // 成功後兩把都保持鎖定；函式內部的 owner 已不再負責解鎖
std::lock_guard<std::mutex> first(a, std::adopt_lock);  // 接管 a 的解鎖責任，不再次 lock
std::lock_guard<std::mutex> second(b, std::adopt_lock); // 接管 b；離開 scope 時兩把各自 unlock
```

## 5. libstdc++：多把鎖的共同思路

[GCC libstdc++ 原始碼：__lock_impl／lock](https://github.com/gcc-mirror/gcc/blob/173072492587491dea9e2b0a9d02da9e4dff2c65/libstdc++-v3/include/std/mutex#L635-L719) 會先阻塞取得一把，再嘗試其他鎖；失敗時釋放已取得的鎖，將失敗位置調整成下一輪先取得的鎖。

在本文固定版本中，C++11／14 使用遞迴旋轉參數的路徑；C++17 起，同型別 Lockable 另有陣列與迴圈路徑。這是實作選擇，不能當成標準要求所有平台採同一套演算法。

## 6. 不等於公平、無 starvation 或固定延遲

- **Deadlock：** 互相等待而無法前進；這裡避免的是取得這組鎖時形成循環。
- **Livelock：** threads 持續撤回與重試，卻暫時沒有工作完成；換順序與 yield 可改善競爭，不能據此推論完全不會發生。
- **Starvation：** 某個 thread 長期競爭不到鎖；標準不保證每個呼叫者公平輪到或固定時間內完成。
- **即時系統：** 阻塞、重試與 yield 都沒有固定完成時間，不能因為 deadlock-free 就當成 audio callback 的即時安全操作。

Hinnant 在 [Dining Philosophers Rebooted](https://howardhinnant.github.io/dining_philosophers.html)（2014）比較 Ordered、Persistent、Smart、Smart & Polite；libc++ 上述策略對應 Smart & Polite。文章的測試結果屬於其硬體與工作負載，不能視為所有平台的效能保證。

## 驗證與原始碼版本

查閱日期：2026-11-15；原始碼連結固定到 commit：

- LLVM：`852cf83ea07d7ecdad841d82d3a51beca306cf93`。
- GCC：`173072492587491dea9e2b0a9d02da9e4dff2c65`。

以 C++11 驗證教學版：記錄 Lockable 呼叫順序，確認 try_lock 失敗後先 unlock 再換順序；注入 try_lock 例外，確認第一把已釋放；兩個 threads 以相反參數順序操作同一對 mutex，20000 次受保護更新的結果一致。測試是實作檢查，deadlock 避免理由仍以上述 invariant 為準。
