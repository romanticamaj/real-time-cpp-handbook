# Modern C++

[← Handbook](../README.md)

依問題選主題，或依 C++ 標準版本查閱。這裡是即時系統的語言與標準庫基礎；各特性的使用限制仍需依目標平台與執行環境判斷。

## 資源管理與生命週期

- [Move semantics 與右值引用](C%2B%2B11_move_semantics.md)
- [Smart pointers：unique / shared / weak ownership](C%2B%2B11_smart_pointers.md)
- [Initializer list 與初始化](C%2B%2B11_initializer_list.md)

## 型別推導與語法

- [auto](C%2B%2B11_auto.md)
- [decltype](C%2B%2B11_decltype.md)
- [nullptr](C%2B%2B11_nullptr.md)
- [enum class](C%2B%2B11_enum_class.md)
- [Range-based for](C%2B%2B11_range_based_for.md)
- [User-defined literals](C%2B%2B11_user_defined_literals.md)

## Callable 與泛型

- [Lambda](C%2B%2B11_lambda.md)
- [std::function](C%2B%2B11_function.md)
- [std::bind](C%2B%2B11_bind.md)
- [Variadic templates](C%2B%2B11_variadic_templates.md)
- [Type traits](C%2B%2B11_type_traits.md)
- [constexpr](C%2B%2B11_constexpr.md)
- [std::reference_wrapper](C%2B%2B11_reference_wrapper.md)

## 並行與效能

- [Threading](C%2B%2B11_threading.md)
- [std::lock：重試、退讓與 deadlock avoidance](C%2B%2B11_std_lock_deadlock_avoidance.md)
- [Thread 生命週期與 terminate：標準契約、原始碼與實驗](C%2B%2B11_thread_lifecycle_and_terminate.md)
- [Atomic operations 與 memory ordering](C%2B%2B11_atomic.md)
- [alignas 與 false sharing](C%2B%2B11_alignas.md)
- [std::chrono](C%2B%2B11_chrono.md)

## 標準庫工具

- [std::tuple](C%2B%2B11_tuple.md)
- [Unordered containers](C%2B%2B11_unordered_containers.md)
- [Random number facilities](C%2B%2B11_random.md)
- [std::regex](C%2B%2B11_regex.md)

## 依版本閱讀

C++11 的個別特性依上方主題分類；C++14–23 可從下列版本文件開始。

- [C++14：核心特性](C%2B%2B14_core_features.md)
- [C++17：核心特性](C%2B%2B17_core_features.md)
- [C++17：標準庫](C%2B%2B17_standard_library.md)
- [C++20：核心特性](C%2B%2B20_core_features.md)
- [C++20：標準庫](C%2B%2B20_standard_library.md)
- [C++23：特性總覽](C%2B%2B23_features.md)

## 跨版本總覽

- [Modern C++ 核心特性：C++11 到 C++23](%E7%8F%BE%E4%BB%A3%20C%2B%2B%20%E6%A0%B8%E5%BF%83%E7%89%B9%E6%80%A7%E6%B7%B1%E5%BA%A6%E8%A7%A3%E6%9E%90%EF%BC%9A%E5%BE%9E%20C%2B%2B11%20%E5%88%B0%20C%2B%2B23.md)
