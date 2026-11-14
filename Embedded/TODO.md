# Embedded C++ Topic Roadmap

[← Embedded](README.md) · [← Handbook](../README.md)

逐步擴充 Embedded Systems 的寫作清單，以受限資源、可預期的執行時間與硬體互動為主線。以下全部為**待新增專題**；既有相關筆記列在 [前置基礎](README.md#先讀既有基礎)，不代表 Embedded 實作已完成。

## 使用方式

- **P0**：建議先補的基礎；**P1**：接續的工程實務；**P2**：依平台與應用選修。
- 每項列出一個可交付的範例、分析或量測，避免只堆名詞。
- 完成文章、加入索引並記錄驗證方式後，才勾選該項；目前不預設開發板與 RTOS。
- 實作時標示 MCU／CPU、工具鏈、最佳化選項與硬體限制；host-side 測試、模擬與板上驗證分開記錄。

## 第一階段：Bare-metal 與硬體基礎

- [ ] **P0 · Cross-compilation 與 firmware build**：整理 compiler、assembler、linker、ELF／BIN／HEX 的流程，產出可重現的最小建置指令。
- [ ] **P0 · Startup、reset 與 linker script**：畫出 vector table、`.data` 複製、`.bss` 清零、C++ initialization 到 `main` 的流程，對照 map file。
- [ ] **P0 · Memory map 與資源預算**：盤點 Flash、RAM、stack、heap、靜態資料與對齊，產出一份映像大小與 RAM 配置表。
- [ ] **P0 · MMIO、register access 與 `volatile`**：比較 volatile、atomic 與 memory barrier 的角色；以讀寫副作用、read-modify-write 與 write-one-to-clear 為案例。
- [ ] **P0 · Interrupt 與 exception**：以 Cortex-M 為範例整理 NVIC、優先權、巢狀中斷與 ISR 限制，量測 interrupt latency。
- [ ] **P0 · ISR 到主迴圈的資料交接**：比較 critical section、atomic flag 與 SPSC queue；列明共享狀態與可重入假設。
- [ ] **P0 · GPIO、timer 與時間基準**：實作 debounce 與週期任務，比較 polling／interrupt，處理 tick counter wraparound。
- [ ] **P1 · UART、SPI 與 I2C**：各選一個周邊，描述 framing、timeout、錯誤回復與 blocking／interrupt／DMA 取捨。
- [ ] **P1 · DMA 與 buffer ownership**：畫出 CPU／DMA 的 buffer 生命週期，區分傳輸完成與資料可重用的時機。
- [ ] **P1 · Cache coherency、alignment 與 MPU**：在支援的平台驗證 DMA cache maintenance、記憶體區域屬性與對齊限制。
- [ ] **P1 · ADC／DAC 與取樣時序**：量測 timer-triggered sampling，分析取樣率、轉換時間與資料搬移方式。

## 第二階段：Embedded C++ 與資源限制

- [ ] **P0 · Freestanding、ABI 與 C interoperability**：釐清 runtime／標準庫可用範圍、`extern "C"` 與 ISR／C callback 的銜接。
- [ ] **P0 · RAII 與硬體資源生命週期**：以 GPIO、critical section 或 DMA channel 示範 ownership、初始化與釋放限制。
- [ ] **P0 · 靜態配置、memory pool 與 arena**：比較固定容量結構、初始化期配置與執行期配置，列出失敗處理及記憶體上限。
- [ ] **P0 · STL 與動態成本盤點**：分析 vector、string、function、shared_ptr 的配置與時間成本，區分初始化路徑與即時路徑。
- [ ] **P0 · Atomics 與目標平台保證**：檢查型別的 lock-free 支援、對齊、memory order 與產生的指令；分別分析 task／ISR 使用情境。
- [ ] **P1 · Exceptions、RTTI 與 runtime policy**：比較工具鏈選項、錯誤傳遞方式、映像大小與延遲；記錄採用條件。
- [ ] **P1 · `constexpr`、templates 與 zero-cost abstraction**：比較 compile-time register configuration 與 runtime abstraction 的組語、大小與可讀性。
- [ ] **P1 · Object layout、padding 與 serialization**：示範 struct padding、endianness、unaligned access 與 wire format，避免直接依賴物件記憶體布局。
- [ ] **P1 · Undefined behavior 與整數邊界**：以 shift、溢位、signedness、aliasing 與物件生命週期建立小型案例。
- [ ] **P1 · 狀態機與非阻塞 driver**：將 blocking driver 改寫為有 timeout、取消與錯誤狀態的 state machine。

## 第三階段：RTOS 與即時性

- [ ] **P0 · Tasks、scheduler 與 context switch**：以一個 RTOS 整理 task state、優先權、preemption 與 time slicing，觀察執行軌跡。
- [ ] **P0 · Deadline、WCET、jitter 與 response time**：區分實測最大時間與 WCET 保證，建立週期任務的時間預算。
- [ ] **P0 · Mutex、semaphore、queue 與 notification**：用同一個事件交接案例比較語義、ownership、buffering 與 API 限制。
- [ ] **P0 · Priority inversion 與 inheritance**：建立三任務示範，觀察阻塞與 priority inheritance 的效果和限制。
- [ ] **P0 · ISR-safe API 與 deferred work**：以 FreeRTOS 為例整理 FromISR API、可呼叫 API 的中斷優先權條件與 task 喚醒流程。
- [ ] **P0 · Critical section 與 interrupt masking**：量測最長遮蔽時間，說明保護範圍、巢狀行為與延遲代價。
- [ ] **P1 · Periodic scheduling 與 timer**：比較相對 delay、絕對週期喚醒與 software timer，記錄 drift 與 callback 執行上下文。
- [ ] **P1 · Stack sizing 與 overflow**：量測 task stack high-water mark，涵蓋最深呼叫鏈與中斷堆疊需求。
- [ ] **P1 · Backpressure 與 overload policy**：為 queue full、資料過期與 deadline miss 定義 drop、coalesce、retry 或 fail-safe 策略。
- [ ] **P1 · FreeRTOS 最小專案**：完成 task＋queue／notification＋ISR＋靜態配置的可重現案例。
- [ ] **P2 · Zephyr 專案結構**：整理 west、Kconfig、devicetree 與 driver model，完成一個板級周邊案例。
- [ ] **P2 · Multicore 與共享資源**：分析 SMP／AMP、core affinity、cache line sharing 與跨核心通訊。

## 第四階段：除錯與驗證

- [ ] **P0 · SWD／JTAG、GDB 與 fault diagnosis**：建立 breakpoint、watchpoint 與 Cortex-M HardFault 排查流程，解讀 fault registers 與 exception stack frame。
- [ ] **P0 · 時間量測與 trace**：比較 GPIO＋logic analyzer、cycle counter 與 RTOS trace，記錄量測本身的成本。
- [ ] **P0 · Host-side unit testing**：將 state machine、parser 或 DSP 邏輯與硬體分離，使用 fake HAL 測試正常與失敗路徑。
- [ ] **P1 · 低干擾 logging**：實作固定容量事件紀錄與延後輸出，處理 buffer 滿載、timestamp 與丟失計數。
- [ ] **P1 · Watchdog 與 fault recovery**：定義各任務健康條件、reset cause 與復原流程，驗證卡死情境。
- [ ] **P1 · Hardware-in-the-loop 與 fault injection**：測試周邊 timeout、DMA error、資料損壞、拔除裝置與負載突增。
- [ ] **P1 · 靜態分析與 CI**：建立 warnings、host tests、cross-build 與映像大小追蹤；區分 host sanitizer 與目標端覆蓋範圍。

## 第五階段：Embedded Audio

- [ ] **P0 · I2S／SAI／TDM 與 codec bring-up**：整理 BCLK、LRCLK、MCLK、slot／sample format 與 codec 控制介面，驗證時鐘與實際資料。
- [ ] **P0 · DMA ping-pong audio pipeline**：串起 half／full-transfer callback、buffer ownership 與處理截止時間。
- [ ] **P0 · Audio block size 與 latency budget**：由實際取樣率與 block size 推導處理週期，分別記錄可計算與實測的延遲。
- [ ] **P0 · Underrun、overrun 與復原**：注入處理超時與 producer／consumer 速率差，觀察計數、輸出與恢復策略。
- [ ] **P1 · Fixed-point、floating-point 與 FPU**：比較 Q format、飽和／溢位、數值誤差與執行成本。
- [ ] **P1 · DSP kernels 與 CMSIS-DSP**：以 FIR、biquad 或 FFT 建立 scalar／最佳化版本的正確性與週期比較。
- [ ] **P1 · Control path 與 audio path 分離**：處理參數更新、平滑、跨執行緒交接與緩衝策略，連結既有 Multi-Buffering 分析。
- [ ] **P1 · Clock drift 與跨時鐘域緩衝**：模擬輸入輸出時鐘差，分析長時間 buffer 水位與 sample-rate conversion 需求。
- [ ] **P2 · USB Audio 與外部串流**：整理端點、同步模式、feedback 與資料格式，以可重現平台案例驗證。

## 第六階段：平台與產品化

- [ ] **P1 · Bootloader 與 firmware update**：畫出 image layout、啟動判定、更新中斷電與 rollback 流程。
- [ ] **P1 · Flash persistence**：處理 erase／program granularity、磨耗、資料版本與斷電一致性。
- [ ] **P1 · Low-power design**：比較 sleep state、喚醒來源與 clock gating，量測功耗及 wake-up latency。
- [ ] **P1 · Secure boot 與更新驗證**：整理簽章驗證、信任根、金鑰管理與 anti-rollback 的責任邊界。
- [ ] **P2 · Embedded Linux bring-up**：以具體平台整理 boot chain、cross-toolchain、device tree 與 Buildroot／Yocto 的角色。
- [ ] **P2 · Linux real-time execution**：以目標 kernel 與設定分析 PREEMPT_RT、scheduling policy、memory locking 與 page fault 對延遲的影響。
- [ ] **P2 · Linux audio integration**：整理 ALSA PCM、period／buffer、xrun 與 userspace audio thread 的量測案例。
- [ ] **P2 · CAN／CAN FD 與通訊可靠性**：依應用整理 arbitration、bus load、錯誤處理與 timeout policy。

## 建議先完成的五篇

1. **Memory map、startup 與 linker script**：建立 firmware 執行全貌。
2. **MMIO、volatile、atomic 與 barrier**：釐清硬體存取與共享狀態的邊界。
3. **Interrupt、critical section 與 deferred work**：建立硬體事件到應用邏輯的路徑。
4. **RTOS scheduling、priority inversion 與時間預算**：連結並行與即時性。
5. **I2S＋DMA ping-pong audio pipeline**：把既有音訊知識落到 Embedded 實作。

## 每篇完成條件

- 說清楚問題、執行上下文、平台假設與資源限制。
- 提供一個最小範例、狀態圖、時序圖或可重現實驗。
- 列出主要取捨與至少一個失敗情境。
- 記錄實際驗證方式、結果與尚未驗證的部分。
- 補上官方參考資料，並加入 Embedded 索引後勾選對應 TODO。

## 官方參考入口

以下是規劃與後續查證的入口，各項實作仍應以目標晶片手冊及所用版本為準。

- [Arm CMSIS-Core](https://arm-software.github.io/CMSIS_6/latest/Core/index.html)：Cortex-M 核心介面、interrupt、cache 與 MPU。
- [FreeRTOS Task Scheduling](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/04-Task-scheduling)：排程模型。
- [FreeRTOS Mutexes](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/04-Mutexes)：互斥與 priority inheritance 的限制。
- [Zephyr Devicetree](https://docs.zephyrproject.org/latest/build/dts/index.html)：硬體描述與設定。
- [Zephyr Debugging](https://docs.zephyrproject.org/latest/develop/debug/index.html)：除錯工具與工作流程。
- [ST AN4839: Level 1 Cache on STM32F7/H7](https://www.st.com/resource/en/application_note/an4839-level-1-cache-on-stm32f7-series-and-stm32h7-series-stmicroelectronics.pdf)：特定 Cortex-M7 平台的 cache 與 DMA 案例。
