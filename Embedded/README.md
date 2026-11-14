# Embedded Systems

[← Handbook](../README.md) · **[Embedded 主題 TODO](TODO.md)**

這是 Real-Time C++ Handbook 的擴充方向。目前以主題規劃為主，尚未建立 Embedded 專題文章；既有 C++ 與音訊文件可作為前置基礎。

## 規劃路線

| 階段 | 要回答的問題 | 狀態 |
|---|---|---|
| Bare-metal 與硬體基礎 | 程式如何啟動、存取硬體與處理中斷？ | 待補 |
| Embedded C++ 與資源限制 | 如何管理生命週期、配置成本與編譯選項？ | 待補 |
| RTOS 與即時性 | 如何安排任務、同步與分析 deadline？ | 待補 |
| 除錯與驗證 | 如何觀察故障、量測時間並驗證行為？ | 待補 |
| Embedded Audio | 如何連接 I2S、DMA、DSP 與音訊 deadline？ | 待補 |
| 平台與產品化 | 如何處理更新、低功耗與 Embedded Linux？ | 待補 |

詳細題目、建議順序與每題練習成果見 [TODO](TODO.md)。

## 先讀既有基礎

- [Placement new 與記憶體生命週期](../CppDesignConcepts/Memory/placement_new_and_memory_handling.md)
- [Alignment 與 false sharing](../Modern%20C%2B%2B/C%2B%2B11_alignas.md)
- [Atomic operations](../Modern%20C%2B%2B/C%2B%2B11_atomic.md)
- [即時執行限制](../Audio%20Programming/04_%E7%A6%81%E5%BF%8C%E6%B8%85%E5%96%AE_%E5%8D%B3%E6%99%82%E5%AE%89%E5%85%A8C%2B%2B%E7%9A%84%E5%AF%A6%E8%B8%90%E6%8C%87%E5%8D%97.md)
- [Multi-Buffering 與資料交換](../Audio%20Programming/Multi-Buffering/README.md)
- [Embedded 參考書索引](../Reading/README.md)

## 補文方式

先從一個平台與一個具體問題開始，例如 Cortex-M 的中斷到任務資料交接。每篇交代平台／工具鏈、限制、設計取捨、最小範例與驗證結果；平台特定結論應明確標示。

TODO 的核取方塊表示「文章完成並已加入索引」，不是「已讀過這個主題」。不必為尚未開始的題目建立空白文件。
