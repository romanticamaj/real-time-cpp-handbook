# C++11 Mutex Ownership：誰取得鎖，誰負責解鎖

[← Modern C++](README.md) · [Threading 基礎](C%2B%2B11_threading.md)

**成功取得 `std::mutex` 的 thread 才能對它呼叫 `unlock()`。** Mutex 的 ownership 屬於執行緒；把它當成讓兩個 threads 互相解鎖的交棒訊號，會違反使用契約。

## 1. 原文在哪裡說 Undefined Behavior？

需要對照兩條標準規則，不能只引用 unlock 的前提就當成原文直接寫了 UB：

| 原文 | 意義 |
|:---|:---|
| [Mutex unlock 前提](https://eel.is/c++draft/thread.mutex.requirements.mutex#general-21)：**“The calling thread owns the mutex.”** | 呼叫 unlock 的 thread 必須是目前 owner |
| [標準庫 Preconditions 總則](https://eel.is/c++draft/structure.specifications#3.4)：**“violation of any preconditions results in undefined behavior.”** | 違反前提條件即為 undefined behavior |

因此：**thread B 對 thread A 持有的 mutex 呼叫 unlock → 違反前提 → undefined behavior。** 同一個 thread 對自己未持有的 mutex 解鎖，也違反這項前提。

Undefined behavior 表示 C++ 不保證程式的行為；不保證報錯、丟例外或立刻當掉。某個環境碰巧正常輸出，仍無法證明實作符合契約。以上連結為持續更新的標準工作草案。

## 2. FooBar 網路解答的陷阱：用 mutex 模擬交棒

討論起點是一份網路上的 FooBar 解答；它使用兩把 mutex，流程可摘要為：

```cpp
// 建構子所在 thread：m2.lock();
// foo thread：m1.lock(); printFoo(); m2.unlock();
// bar thread：m2.lock(); printBar(); m1.unlock();
```

| 位置 | Ownership 問題 |
|:---|:---|
| foo 呼叫 `m2.unlock()` | m2 原先由建構子所在 thread 取得，foo 無權代它解鎖 |
| bar 呼叫 `m1.unlock()` | m1 由 foo thread 取得，bar 無權代它解鎖 |

此例假設建構、foo 與 bar 按一般題目測試架構由不同 threads 執行。**這是 correctness 問題，不能用評測時間判斷是否值得採用。** 本文沒有執行這段含 UB 的程式，也沒有量測它的效能。

## 3. C++14：由同一個 thread 完成 lock／unlock

```cpp
#include <cassert>
#include <mutex>
#include <thread>

int main() {
    std::mutex m;
    int count = 0;
    auto increment = [&] {
        std::lock_guard<std::mutex> guard(m); // 由執行此 lambda 的 thread 取得 m
        ++count;
    }; // 同一個 thread 離開 scope，guard 解構並解鎖 m
    std::thread a(increment), b(increment);
    a.join(); b.join();
    assert(count == 2);
}
```

需要交替輸出或等待資料時，使用 **mutex＋共享 predicate＋condition_variable**：wait 在 consumer thread 內釋放 mutex，返回前由該 thread 重新取得它；producer 則自行取得、更新狀態、再釋放同一把 mutex。[wait 的鎖定契約](https://eel.is/c++draft/thread.condition.condvar)

`unique_lock` 可以 move，但 move 的是管理物件的責任，並不改變底層 mutex 的持鎖 thread；不能藉此讓另一個 thread 解鎖。

## 4. Semaphore 為什麼可以跨 thread 交棒？

| 工具 | 管理的資訊 | 跨 thread 交棒 |
|:---|:---|:---|
| `std::mutex`（C++11） | 哪個 thread 擁有互斥鎖 | 取得與解鎖必須由 owner 完成 |
| `std::condition_variable`（C++11） | 等待與通知；條件另存在共享變數 | 可以跨 thread notify，wait 的 mutex 仍遵守 ownership |
| `std::binary_semaphore`（C++20） | 可取得的 permit | 可由一個 thread acquire，另一個 thread release |

Semaphore 沒有 mutex 的 thread ownership 限制；release 增加 permit，acquire 成功時消耗 permit，兩個操作不必由同一個 thread 執行。下面示範一次 producer→consumer 交接：

```cpp
#include <cassert>
#include <semaphore>
#include <thread>

int main() {
    std::binary_semaphore ready(0); // 初始無 permit
    int value = 0;
    std::thread producer([&] {
        value = 42;
        ready.release(); // 發布資料並提供 permit
    });
    ready.acquire(); // 等到取得 permit，再讀取 producer 發布的資料
    assert(value == 42);
    producer.join();
}
```

此例依賴 semaphore release／成功取得間的同步關係；若有多個 threads 共同修改 queue，仍須另外設計互斥與資料結構 invariant，不能只靠 permit 計數保護所有操作。

## 使用重點與參考

- **C++14 練習：** 優先使用 mutex＋condition_variable；`std::binary_semaphore` 需 C++20。
- **生命週期：** mutex 仍被任何 thread 持有時銷毀，或 thread 結束時仍持有 mutex，也屬 UB。[Mutex 生命週期契約](https://eel.is/c++draft/thread.mutex.class#5)
- **即時性：** mutex 與 semaphore 都可能阻塞，不因符合 ownership 契約就適合放進 real-time audio callback。
- [Semaphore 契約](https://eel.is/c++draft/thread.sema.cnt)：permit 計數、release 與 acquire 的同步語義。
- [Semaphore synopsis](https://eel.is/c++draft/thread.sema): `binary_semaphore` 是 C++20 標準庫的二元 semaphore 型別。
