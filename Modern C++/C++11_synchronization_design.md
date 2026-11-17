# 同步設計心法：從單執行緒正確性到縮小同步範圍

[← Modern C++](README.md)

先確定操作如何正確改變狀態，再處理多執行緒存取、條件等待與同步成本。本文以 C++11／14 的 mutex＋condition variable 為主，適用於允許阻塞的一般工作執行緒。

**四階段是從練習經驗與教材原則歸納的設計流程，不是某本書原封不動提出的固定方法。** 不必每題都實作四版；每階段確認正確性後，再按需求往下走。

## 1. 先有單執行緒正確性：Sequential correctness

**先不考慮 thread safety，釐清共享狀態、合法操作與 invariant。**

- 狀態代表什麼？例如 queue 的 `head`、`tail`、`count`，或本組已輸出的 H／O 數量。
- 操作成立的前提是什麼？例如非空才能 dequeue，H 額度未滿才能輸出 H。
- 一次操作結束後，哪些關係必須維持？例如 `0 <= count <= capacity`。

```cpp
// H2O：每組最多 2H＋1O；三次輸出完成才重設，不能讓下一組混入
// Queue：先確認非空，再取出並更新 head／count，視為一次完整操作
```

這一階段驗證的是單次操作的邏輯，不要求協調程式能在單一 thread 下跑完整個情境；等待條件可能需要其他 thread 才能成立。

## 2. 再擴充到多執行緒：Coarse-grained locking

**先用簡單、完整的鎖，保護可能被其他 thread 看見的狀態轉換。**

```cpp
std::lock_guard<std::mutex> lock(m); // 同一把 mutex 保護相關共享狀態的讀與寫
// 持鎖完成：檢查條件 → 使用／預留資源 → 更新狀態
```

鎖保護的是整個邏輯操作，而不只是個別變數或單行指令。更新中間可以暫時打破 invariant，但不能讓其他受同一 mutex 協調的存取看見這個中間狀態。

**把每個函式各自上鎖，不代表組合起來就安全。** `empty()`、`top()`、`pop()` 若分別加鎖，其他 thread 仍能在呼叫之間插入；應重新設計成完整的取出操作。

H2O 的實際陷阱：檢查 `Hydrogen < 2` 後先解鎖，之後才輸出並 `++Hydrogen`，其他 H threads 就可能全部通過檢查；未受鎖保護的計數讀寫還會形成 data race。應把檢查、輸出、計數與重設視為同一段受保護的操作。

## 3. 再處理「不能繼續」：Condition-based waiting

**不能繼續時放鎖等待；狀態改變後，通知等待者重新檢查。**

```cpp
std::unique_lock<std::mutex> lock(m); // wait 需要能解鎖／重新上鎖的 unique_lock
cv.wait(lock, [&] { return ready; }); // false 才放鎖等待；返回時持鎖且 predicate 為 true
```

設計時問三件事：

1. 哪個 predicate 表示現在可以繼續？由哪把 mutex 保護它？
2. 哪些狀態轉換可能讓 predicate 成立？例如入列、釋放叉子、本組輸出完成。
3. 應通知哪些等待者？共用 CV 的等待者若條件不同，`notify_one()` 不保證挑到條件成立的人。

```cpp
ready = true;    // 在保護 ready 的 mutex 下修改狀態；狀態記住條件是否成立
cv.notify_all(); // 讓目前等待者醒來競爭 mutex、重新檢查；不是直接分配資源
```

- CV 不會保存通知；沒有等待者時的通知，不能留給未來的 `wait()` 使用。
- predicate 不會消除 spurious wakeup，而是避免醒來後在條件不成立時繼續。
- `wait` 與 `notify` 不必一對一配對；要驗證的是需要繼續的人能否重新檢查條件。
- 這是共享狀態上的條件等待，不必稱為 event-driven 架構；也不是每題都需要 CV。

## 4. 最後才優化：Synchronization refinement

**維持 invariant，再減少不必要的持鎖時間與競爭。**

| 調整方向 | 意義 | 必須重新確認 |
|:---|:---|:---|
| 縮短持鎖時間 | 把不碰共享狀態的工作移到鎖外 | 解鎖後，先前的檢查結果是否仍有效？ |
| 拆分鎖的粒度 | 不同資料由不同 mutex 保護 | 跨資料操作的一致性與鎖順序 |
| 精準通知 | 分開管理不同類型的等待者 | 是否可能喚醒不合適的人，或漏掉進度所需的通知？ |

**持鎖時間與鎖的粒度是不同決策；scope 越小不保證越快。** 拆鎖、頻繁 lock／unlock 也有成本，效能改善需要量測。

Dining Philosophers 的例子：持鎖檢查兩叉可用並同時標為占用，再解鎖吃飯；此時保留的是叉子的使用權，不必一直持有保護狀態陣列的 mutex。吃完後重新持鎖釋放資源。預留與釋放必須涵蓋完整生命週期，例外路徑也要考慮。

H2O 則不能只為縮鎖而把輸出搬出去：若計數先完成並重設，上一組 callback 尚未完成時，下一組就可能開始輸出。需保護的也包含輸出分組順序。

## 每個階段都要檢查

- **安全性**：所有相關存取是否一致受保護？檢查到更新之間會不會失效？物件生命週期是否足夠？
- **進度**：條件不成立時，誰能讓它成立並通知？有沒有 deadlock、漏通知或永久忙等？
- **公平性**：能繼續執行不等於人人都輪得到；若要求 starvation-free，需要另訂服務順序與排程假設。
- **驗證**：測試不同到達順序與多個同類 thread；ThreadSanitizer 可協助找 data race，不能據此證明沒有 deadlock／starvation。

## 練習對照

| 題目 | 增加的設計重點 |
|:---|:---|
| LC 1115. Print FooBar Alternately | 用 turn flag 表示交替執行的條件 |
| LC 1188. Design Bounded Blocking Queue | 容量／資料條件、兩類等待者與 directional notification |
| LC 1226. The Dining Philosophers | 多資源預留、縮短持鎖時間；FIFO 公平性為另做的 follow-up |
| LC 1117. Building H2O | 多個同類 thread、每組配額與組間輸出順序 |

## 經典教材如何對照

### C++ Concurrency in Action — Anthony Williams

- 第 3 章從共享資料的 invariant 與 race condition 出發；3.2.3 討論介面組合的競爭，3.2.8 討論適當的鎖粒度。
- 第二版 6.2.2～6.2.3 對照使用 locks＋CV 的 queue 與細粒度版本；可從[出版社章節目錄](https://livebook.manning.com/learn/c-plus-plus-concurrency-in-action-second-edition)查閱位置。
- 作者公開範例：[Listing 6.4](https://github.com/anthonywilliams/ccia_code_samples/blob/main/listings/listing_6.4.cpp) 是未同步的 linked queue；[Listing 6.2](https://github.com/anthonywilliams/ccia_code_samples/blob/main/listings/listing_6.2.cpp) 提供 mutex＋CV 的 queue；[Listing 6.6](https://github.com/anthonywilliams/ccia_code_samples/blob/main/listings/listing_6.6.cpp) 使用 head／tail 分鎖，並將配置移到持鎖範圍外。

這些範例支持由基本操作、完整保護到增加並行機會的思路；排列是本文的概念對照，並非書中固定四步順序。來源碼用於閱讀設計，不代表已在本筆記中執行驗證。

### Operating Systems: Three Easy Pieces — Remzi／Andrea Arpaci-Dusseau

[第 30 章 Condition Variables](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf) 從 spinning 的成本切入，再以共享狀態、mutex、wait／signal 建立等待機制；錯誤範例說明為何不能只有通知而沒有狀態，以及為何醒來後要重新檢查條件。

書中使用 POSIX／C 示範；初始 spinning 範例用來呈現問題，不能將其中的 `volatile` 當作 C++ thread synchronization 解法。

### C++ Core Guidelines — 同步設計準則

- [CP.42：Don’t wait without a condition](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rconc-wait)：條件判斷是等待機制的一部分。
- [CP.43：Minimize time spent in a critical section](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rconc-time)：減少不必要的持鎖時間。

## 使用邊界與延伸閱讀

此流程針對 lock-based 設計；不要求先寫出有 data race 的多執行緒版本再修正。即時音訊 callback 不應直接套用 mutex、阻塞等待或動態配置，需將工作移到非即時路徑，再設計資料交接方式。

- [Threading：API 與基本用法](C%2B%2B11_threading.md)
- [Mutex ownership 與跨 thread 交棒](C%2B%2B11_mutex_ownership_and_thread_handoff.md)
- [std::lock：deadlock avoidance 實作研究](C%2B%2B11_std_lock_deadlock_avoidance.md)
