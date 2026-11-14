# Real-Time Audio Programming

[← Handbook](../README.md)

以延遲與執行時間限制為起點，逐步連接 callback、跨執行緒資料交換與系統最佳化。

## 總覽

- [C++ 低延遲音訊處理的架構與技術](00-C%2B%2B%20%E4%BD%8E%E5%BB%B6%E9%81%B2%E9%9F%B3%E8%A8%8A%E6%9C%80%E4%BD%B3%E5%8C%96-Overview.md)

## 即時音訊基礎

- [定義延遲：從 ADC 到 DAC 的訊號路徑](01_%E5%AE%9A%E7%BE%A9%E5%BB%B6%E9%81%B2_%E5%BE%9EADC%E5%88%B0DAC%E7%9A%84%E8%A8%8A%E8%99%9F%E8%B7%AF%E5%BE%91.md)
- [即時音訊回呼：系統的心跳](02_%E5%8D%B3%E6%99%82%E9%9F%B3%E8%A8%8A%E5%9B%9E%E5%91%BC_%E7%B3%BB%E7%B5%B1%E7%9A%84%E5%BF%83%E8%B7%B3.md)
- [即時程式設計的最高原則：「時間不等人」](03_%E5%8D%B3%E6%99%82%E7%A8%8B%E5%BC%8F%E8%A8%AD%E8%A8%88%E7%9A%84%E6%9C%80%E9%AB%98%E5%8E%9F%E5%89%87_%E6%99%82%E9%96%93%E4%B8%8D%E7%AD%89%E4%BA%BA.md)
- [「禁忌清單」：即時安全 C++ 的實踐指南](04_%E7%A6%81%E5%BF%8C%E6%B8%85%E5%96%AE_%E5%8D%B3%E6%99%82%E5%AE%89%E5%85%A8C%2B%2B%E7%9A%84%E5%AF%A6%E8%B8%90%E6%8C%87%E5%8D%97.md)
- [基本權衡：延遲 vs. 穩定性](05_%E5%9F%BA%E6%9C%AC%E6%AC%8A%E8%A1%A1_%E5%BB%B6%E9%81%B2vs%E7%A9%A9%E5%AE%9A%E6%80%A7.md)

## 並行與資料交換

- [環形緩衝區](07_%E7%92%B0%E5%BD%A2%E7%B7%A9%E8%A1%9D%E5%8D%80.md)
- [生產者-消費者架構模式](08_%E7%94%9F%E7%94%A2%E8%80%85%E6%B6%88%E8%B2%BB%E8%80%85%E6%9E%B6%E6%A7%8B%E6%A8%A1%E5%BC%8F.md)
- [無鎖程式設計](09_%E7%84%A1%E9%8E%96%E7%A8%8B%E5%BC%8F%E8%A8%AD%E8%A8%88.md)
- [Multi-Buffering：Atomic building blocks → 同步策略 → 雙／三緩衝](Multi-Buffering/README.md)

## 系統與架構

- [作業系統干預](10_%E4%BD%9C%E6%A5%AD%E7%B3%BB%E7%B5%B1%E5%B9%B2%E9%A0%90.md)
- [使用 SIMD 實現指令級平行](11_%E4%BD%BF%E7%94%A8SIMD%E5%AF%A6%E7%8F%BE%E6%8C%87%E4%BB%A4%E7%B4%9A%E5%B9%B3%E8%A1%8C.md)
- [複雜處理的架構模式](12_%E8%A4%87%E9%9B%9C%E8%99%95%E7%90%86%E7%9A%84%E6%9E%B6%E6%A7%8B%E6%A8%A1%E5%BC%8F.md)

## 延伸閱讀

- [延伸閱讀與推薦資源](13_%E5%BB%B6%E4%BC%B8%E9%96%B1%E8%AE%80%E8%88%87%E6%8E%A8%E8%96%A6%E8%B3%87%E6%BA%90.md)
- [ADC：音訊系統設計演講整理](ADC/README.md)

## 建議順序

基礎 01–05 → Ring buffer / Producer-consumer 07–08 → Atomics 基礎與 Lock-free 09 → Multi-Buffering → 系統與架構 10–12。

現有系列編號從 05 跳至 07，此索引保留原始編號；目前沒有 06 文件。

範例程式與教學中的設計取捨請搭配閱讀；各種同步策略對 callback 的適用性不同。硬體端的 I2S、DMA 與 RTOS 音訊管線列在 [Embedded TODO](../Embedded/TODO.md)。
