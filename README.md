# Real-Time C++ Handbook

**Modern C++ foundations for low-latency audio and real-time systems.**

以即時音訊系統為主軸，整理 Modern C++、STL API 設計、記憶體管理與並行程式設計的繁體中文工程手冊，並逐步擴充 Embedded Systems。

適合想查 API、理解設計取捨，或把 C++ 基礎串接到即時系統的開發者。內容以技術筆記、速查與示範程式為主。

**[STL 速查](CppDesignConcepts/Algorithm/CheatSheet/README.md) · [C++ 設計](CppDesignConcepts/README.md) · [Modern C++](Modern%20C%2B%2B/README.md) · [即時音訊](Audio%20Programming/README.md) · [Embedded 待辦](Embedded/TODO.md)**

## 依問題找文件

| 我想要…… | 從這裡開始 |
|---|---|
| 查排序、搜尋、Heap、數值演算法 | [STL Algorithm Cheatsheets](CppDesignConcepts/Algorithm/CheatSheet/README.md) |
| 查 `string`、`set`、`bitset` 的 API 與設計 | [容器與字串](CppDesignConcepts/Container/README.md) |
| 分清楚 `resize`、`assign`、`fill` | [容器填充操作比較](CppDesignConcepts/Container/container_fill_operations_comparison.md) |
| 理解 `set::lower_bound` 與通用版本的差異 | [Container Member vs. Algorithm](CppDesignConcepts/Algorithm/Searching/OrderedSet_Binary_Search.md) |
| 查 iterator 類別、失效規則與 output iterator | [Iterator 設計](CppDesignConcepts/Iterator/README.md) |
| 釐清 comparator 與 `priority_queue` 比較語義 | [Comparator 設計心法](CppDesignConcepts/Function/comparator_design_principles.md) |
| 補強 move semantics、資源管理、atomics | [Modern C++ 主題索引](Modern%20C%2B%2B/README.md) |
| 釐清 thread 為什麼會觸發 `std::terminate`、如何管理 join／detach | [Thread 生命週期研究](Modern%20C%2B%2B/C%2B%2B11_thread_lifecycle_and_terminate.md) |
| 理解 audio callback 與即時執行限制 | [即時音訊閱讀路線](Audio%20Programming/README.md) |
| 比較 double / triple buffering 與資料交換方式 | [Multi-Buffering 系列](Audio%20Programming/Multi-Buffering/README.md) |
| 開始補 Embedded、RTOS、硬體介面 | [Embedded Roadmap](Embedded/TODO.md) |

## 主題地圖

| 主題 | 內容 | 形式 |
|---|---|---|
| [Modern C++](Modern%20C%2B%2B/README.md) | C++11–23、生命週期、Callable、型別與並行基礎 | 主題分析、版本總覽 |
| [C++ Design Concepts](CppDesignConcepts/README.md) | STL API、容器、演算法、Iterator、記憶體與函式設計 | 速查、比較、深入分析 |
| [Real-Time Audio](Audio%20Programming/README.md) | 延遲、callback、ring buffer、lock-free、SIMD、buffering | 系列教學、設計案例、示範程式 |
| [Embedded Systems](Embedded/README.md) | Bare-metal、RTOS、DMA、除錯與 Embedded Audio | **規劃中：主題待辦** |
| [Software Testing](Software%20Testing/README.md) | 軟體測試課程綱要 | 延伸學習 |
| [Reading](Reading/README.md) | Embedded 與測試參考書 | PDF 資料索引 |

## 建議閱讀路線

### 理解 C++ 的設計

[資源管理與生命週期](Modern%20C%2B%2B/README.md#資源管理與生命週期) → [容器操作與 API](CppDesignConcepts/Container/README.md) → [Iterator](CppDesignConcepts/Iterator/README.md) → [演算法與比較器](CppDesignConcepts/Algorithm/README.md)。

### 走向即時音訊

[C++ Atomics](Modern%20C%2B%2B/C%2B%2B11_atomic.md) → [Audio Callback](Audio%20Programming/02_%E5%8D%B3%E6%99%82%E9%9F%B3%E8%A8%8A%E5%9B%9E%E5%91%BC_%E7%B3%BB%E7%B5%B1%E7%9A%84%E5%BF%83%E8%B7%B3.md) → [即時安全限制](Audio%20Programming/04_%E7%A6%81%E5%BF%8C%E6%B8%85%E5%96%AE_%E5%8D%B3%E6%99%82%E5%AE%89%E5%85%A8C%2B%2B%E7%9A%84%E5%AF%A6%E8%B8%90%E6%8C%87%E5%8D%97.md) → [Multi-Buffering](Audio%20Programming/Multi-Buffering/README.md) → [音訊系統最佳化](Audio%20Programming/README.md#系統與架構)。

### 擴充 Embedded

[既有 C++ 基礎](Embedded/README.md#先讀既有基礎) → [Bare-metal 與硬體介面](Embedded/TODO.md#第一階段bare-metal-與硬體基礎) → [RTOS 與即時性](Embedded/TODO.md#第三階段rtos-與即時性) → [Embedded Audio](Embedded/TODO.md#第五階段embedded-audio)。此路線的 Embedded 專題目前為待補內容。

## 精選設計分析

- [Iterator 設計分析](CppDesignConcepts/Iterator/iterator_design_analysis.md)：類別、容器支援與失效規則。
- [Container 填充操作比較](CppDesignConcepts/Container/container_fill_operations_comparison.md)：從需求選擇建構、指派或填值。
- [Comparator 設計心法](CppDesignConcepts/Function/comparator_design_principles.md)：排序、多欄位比較與容器語義。
- [即時安全的同步策略](Audio%20Programming/Multi-Buffering/03-RealTime_Thread_Safety.md)：從資料交換需求理解同步取捨。

## 工具與使用方式

- 直接從上方入口閱讀；長文可使用 GitHub 的文件大綱跳轉章節。
- [C++ STL Analyzer](tools/cpp-std-analyzer/README.md)：整理 API 設計分析的 Prompt、模板與 Roo Code Mode。
- 範例與分析以各文件描述為準；本 repo 未提供涵蓋所有範例的統一建置與測試流程。
- [Embedded TODO](Embedded/TODO.md) 使用 checkbox 追蹤新增專題；已完成的文章會加入各區索引。
