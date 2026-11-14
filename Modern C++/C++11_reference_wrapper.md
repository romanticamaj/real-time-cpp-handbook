# C++11 std::ref 與 std::reference_wrapper

## 核心概念

`std::ref(x)` 回傳 `std::reference_wrapper<T>`，其中 `T` 是 `x` 的型別。它是**可以複製、能轉換成 `T&` 的物件**，定義於 `<functional>`。

一般參考可以直接寫成 `T&`；當 API 需要以值的形式保存引數，或容器需要存放物件時，可以用 wrapper 保存參考語意。

```cpp
#include <functional>

void demo() {
    int x = 10;
    int& alias = x;          // 語言本身的參考
    auto r = std::ref(alias); // reference_wrapper<int> 物件
    auto copy = r;           // 複製 wrapper，仍參考同一個 x
    copy.get() = 20;         // x 變成 20
}
```

## 它怎麼提供 reference？

核心機制可以用「保存位址，再解參考」理解。以下為教學用簡化版本，省略標準版本的可呼叫物件支援等細節：

```cpp
#include <memory>

template<class T>
class RefWrapper {
    T* ptr;

public:
    explicit RefWrapper(T& obj) : ptr(std::addressof(obj)) {}
    RefWrapper(T&&) = delete;  // 拒絕直接包裝暫時物件

    T& get() const { return *ptr; }
    operator T&() const { return *ptr; }
};

template<class T>
RefWrapper<T> ref(T& obj) {
    return RefWrapper<T>(obj);
}
```

- `std::addressof(obj)` 取得真正位址，即使類別重載了 `operator&` 也能正確運作。
- `get()` 回傳 `T&`，讓呼叫者存取原物件。
- `operator T&()` 是轉型運算子，讓 wrapper 在需要 `T&` 時可以隱式轉換。
- 複製 wrapper 只會複製參考關係，不會複製原物件；標準不要求特定的內部儲存布局。

```cpp
#include <functional>

void increment(int& n) { ++n; }
void increment_copy(int n) { ++n; }

void demo() {
    int x = 10;
    auto r = std::ref(x);
    increment(r);       // 隱式轉換成 int&，x 變成 11
    increment_copy(r);  // 轉換後仍須複製成值參數，x 維持 11
}
```

**是否修改原物件，仍取決於接收端的參數型別。** 此外，模板型別推導不會先套用這個轉換：`f(T)` 接收 `r` 時，`T` 會推導成 wrapper 型別。

## 為什麼 std::thread 需要 std::ref？

`std::thread` 預設將引數以值的形式保存；直接傳入 `x` 不會保存 `int&`。若工作函式要求 `int&`，直接傳 `x` 會無法編譯。

```cpp
#include <cassert>
#include <functional>
#include <thread>

void increment(int& n) { ++n; }

int main() {
    int x = 10;
    // std::thread t(increment, x);  // 無法編譯
    std::thread t(increment, std::ref(x));
    t.join();
    assert(x == 11);
}
```

這裡的過程是：

1. `std::ref(x)` 建立參考 `x` 的 wrapper。
2. thread 保存 wrapper 的副本，副本仍參考 `x`。
3. 呼叫 `increment(int&)` 時，wrapper 隱式轉換成 `int&`。

`join()` 讓主執行緒在工作完成後才讀取結果，且此例中 `x` 一直存活到工作完成。`std::ref` 本身不提供同步機制。

## std::cref 與使用限制

| 寫法 | 結果型別 | 取得的參考 |
|------|----------|------------|
| `std::ref(x)`，`x` 為 `T` | `std::reference_wrapper<T>` | `T&` |
| `std::ref(x)`，`x` 為 `const T` | `std::reference_wrapper<const T>` | `const T&` |
| `std::cref(x)`，`x` 為 `T` | `std::reference_wrapper<const T>` | `const T&` |

- **生命週期**：wrapper 不擁有物件，也不延長其生命週期；原物件銷毀後就會懸空。
- **暫時物件**：`std::ref(42)` 與 `std::cref(42)` 都無法編譯，但這不代表 wrapper 能防止所有懸空情況。
- **容器**：可以使用 `std::vector<std::reference_wrapper<T>>` 保存對既有物件的參考；`std::vector<T&>` 則不合法。
- **重新指定**：對 wrapper 做 `a = b` 會改變 `a` 參考的對象；一般 `T&` 的賦值會修改原物件，不能重新綁定。

## 相關文件

- [C++11 多執行緒支援](C++11_threading.md)：共享 counter、queue 與非同步呼叫的使用情境。
