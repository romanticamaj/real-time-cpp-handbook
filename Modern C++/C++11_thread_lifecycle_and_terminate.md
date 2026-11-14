# C++11 Thread 生命週期與 std::terminate：從契約追到實作

[← Modern C++](README.md) · [Threading 基礎](C%2B%2B11_threading.md)

**核心規則：`std::thread` 物件解構時若仍 `joinable()`，就呼叫 `std::terminate()`；預設會終止整個程式。** worker 的工作完成，與 thread 物件已完成管理收尾，是兩件事。

## 1. 先分清楚兩個生命週期

| 狀態 | `joinable()` | thread 物件此時解構 |
|:---|:---:|:---|
| worker 還在執行，未 join／detach | true | 呼叫 terminate |
| worker 已完成，未 join／detach | true | 一樣呼叫 terminate |
| 成功 join／detach 後 | false | 不因這條規則終止程式 |
| 預設建構、尚未管理執行緒 | false | 不因這條規則終止程式 |

```cpp
// worker.join()：等待工作完成，再解除 thread 物件與執行緒的關聯。
// worker.detach()：解除關聯，不等待工作完成；資料生命週期仍由程式設計者負責。
// 沒有 join 不會立即停止 worker；觸發點是物件「解構時仍 joinable」。
```

[標準草案：thread 解構契約](https://eel.is/c++draft/thread.thread.destr) 明訂此規則，並指出：自動 detach 可能引入正確性問題，自動 join 可能造成難察覺的效能問題，尤其在例外路徑。因此不能假設解構子會自動替你選擇其中一種。

## 2. libc++：解構子檢查內部 handle

LLVM libc++ 的原始碼：

```cpp
thread::~thread() {
  if (!__libcpp_thread_isnull(&__t_))
    terminate();
}
```

- [解構子：thread.cpp](https://github.com/llvm/llvm-project/blob/852cf83ea07d7ecdad841d82d3a51beca306cf93/libcxx/src/thread.cpp#L38-L41)：`__t_` 仍非空，就呼叫 terminate。
- [joinable：thread.h](https://github.com/llvm/llvm-project/blob/852cf83ea07d7ecdad841d82d3a51beca306cf93/libcxx/include/__thread/thread.h#L259)：也使用 `!__libcpp_thread_isnull(&__t_)`，與解構子的條件相同。
- [join／detach：thread.cpp](https://github.com/llvm/llvm-project/blob/852cf83ea07d7ecdad841d82d3a51beca306cf93/libcxx/src/thread.cpp#L43-L65)：底層操作成功後，才將 `__t_` 設為 `_LIBCPP_NULL_THREAD`。

**這裡檢查的是物件是否仍管理執行緒，而非向 OS 查詢工作是否已完成。** 單純讓 worker 函式返回，不會執行呼叫端的 join／detach 收尾。

## 3. libstdc++：直接呼叫 joinable

GCC libstdc++ 使用同一個規則：

```cpp
~thread()
{
  if (joinable())
    std::__terminate();
}
```

[解構子與 joinable：std_thread.h](https://github.com/gcc-mirror/gcc/blob/173072492587491dea9e2b0a9d02da9e4dff2c65/libstdc++-v3/include/bits/std_thread.h#L181-L208)；其 `joinable()` 以保存的 `_M_id` 是否等於預設 `id()` 判斷。

這是 **C++ 標準庫層**的管理規則；Linux／macOS 的 OS thread API 位於更下層。本文對照 LLVM 與 GCC upstream 實作，不代表本機或某一版 macOS 發行的 binary 就與這個 revision 完全相同。

## 4. terminate 如何一路走到整個程式終止？

`std::terminate()` 宣告於 `<exception>`，屬於 C++ 的 abnormal termination 機制；「異常終止」不表示一定發生過 `throw`。

```text
std::thread 解構時仍 joinable
    → std::terminate()
    → terminate handler
    → 預設 handler 的診斷／終止處理
    → abort()
    → 整個 process 終止，主執行緒與所有 workers 都結束
```

沿 LLVM libc++abi 追蹤：

| 位置 | 要看的重點 |
|:---|:---|
| [cxa_handlers.cpp](https://github.com/llvm/llvm-project/blob/852cf83ea07d7ecdad841d82d3a51beca306cf93/libcxxabi/src/cxa_handlers.cpp#L51-L91) | `terminate()` 選擇 handler，再由 `__terminate()` 呼叫；handler 返回或丟例外也會走向終止 |
| [cxa_default_handlers.cpp](https://github.com/llvm/llvm-project/blob/852cf83ea07d7ecdad841d82d3a51beca306cf93/libcxxabi/src/cxa_default_handlers.cpp#L27-L96) | 預設可經過輸出診斷的 handler；silent 配置則直接使用 `std::abort` |
| [abort_message.cpp](https://github.com/llvm/llvm-project/blob/852cf83ea07d7ecdad841d82d3a51beca306cf93/libcxxabi/src/abort_message.cpp#L66) | 診斷路徑最後呼叫 `abort()` |

正常從 `main` 返回會做正常的結束處理；預設 `abort()` 不會照正常 scope 離開流程解構物件，不能把它當成清理機制。自訂 `std::set_terminate()` handler 也必須終止程式，不能返回後繼續執行。[標準草案：terminate／handler 契約](https://eel.is/c++draft/exception.terminate)

## 5. 最小實驗：少一行 join，main 就無法繼續

以下是獨立程式；**未加回 join 時會刻意終止整個測試 process**。

```cpp
#include <iostream>
#include <thread>

int main() {
    std::cerr << "before scope\n";
    {
        std::thread worker([] {});
        // worker.join(); // 加回這行，比較兩個版本
    }                    // 仍 joinable：呼叫 terminate，預設 abort
    std::cerr << "after scope\n";
}
```

```sh
clang++ -std=c++11 -pthread demo.cpp -o demo
./demo
```

| 版本 | 本次 macOS／Apple Clang＋libc++ 實驗 |
|:---|:---|
| 不呼叫 join | 印出 before scope，然後以 SIGABRT 終止；沒有 after scope |
| 加回 join | 印出 before scope 與 after scope，正常結束，exit status 0 |

訊息與 signal 是本次環境的觀察；跨平台應依賴的是「解構時 joinable 就呼叫 terminate」的標準契約。這個範例沒有假定 worker 在解構前一定已完成，也不需要這個前提。

## 6. C++ 常見陷阱與處理重點

- **例外／early return 跳過 join：** C++11／14 可用 RAII owner 安排各條路徑的收尾；worker 所需資料必須活到 join 結束。
- **持鎖 join：** 若 worker 完成前也需要這把鎖，會互等；先離開 critical section，再 join。
- **detach 後 reference 失效：** `[&]`、`std::ref` 與捕捉的 `this` 都不會自動延長目標生命週期；參考 [reference_wrapper](C%2B%2B11_reference_wrapper.md)。
- **誤把 join 當互斥：** join 同步完成與呼叫端，不會補救 workers 工作期間已發生的 data race。
- **C++20 的選項：** `std::jthread` 解構時若 joinable，會 request_stop 再 join；停止是合作式的，不保證 worker 立即結束。

`join` 可能阻塞；在即時音訊系統中，應將這類生命週期管理安排在非即時控制路徑。

## 原始碼版本

查閱日期：2026-11-15。上方原始碼連結皆固定到 commit，避免開發分支更新造成行號漂移：

- LLVM libc++／libc++abi：`852cf83ea07d7ecdad841d82d3a51beca306cf93`。
- GCC libstdc++：`173072492587491dea9e2b0a9d02da9e4dff2c65`。
