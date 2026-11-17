# 三緩衝區：SPSC 最新快照交換

完整範例：[06 · TripleBuffer](06-Triple_Buffer_Implementation.cpp)。此設計適合交換參數、分析結果或可略過中間版本的完整快照。它不是 FIFO；需要保留每一塊音訊資料時，應使用有明確滿／空處理的 SPSC queue。

## Ownership 與使用條件

三個 buffer 在任何交接完成後，分別由消費者 `front`、生產者 `back` 與共享 `middle` 持有。生產者只寫 `back`，消費者只讀 `front`，交接透過同一個原子變數完成。

- 僅一個生產者呼叫 `Write()`／`Commit()`，僅一個消費者呼叫 `Read()`；同一端不得重入。
- 建構完成後才啟動兩端，兩端停止後才能解構。
- `Write()` 的引用僅可使用至下一次 `Commit()`；提交後不可繼續讀寫該引用。
- `Read()` 回傳 `const T&`，引用僅可使用至下一次 `Read()`。在此期間，生產者不會取得該 buffer。
- 沒有更新時，`Read()` 保留現有快照；首次提交前讀到預設初始化的 `T`。
- `T` 應自行持有快照資料；若內含指向外部可變物件的指標，本協定不會保護外部物件。

## 單一原子狀態

實作使用 `std::atomic<std::uint32_t>`，低兩個 bit 編碼 buffer 索引 `0`、`1`、`2`，bit 2 為 `dirty`。初始配置為 `front=0`、`back=1`、`middle=2`，且沒有更新。

不能把「取得 middle」與「歸還 front」拆成獨立的 load／store。例如舊實作可能出現：

1. 生產者提交 buffer 1，middle 指向 1，自己取得 buffer 2。
2. 消費者 load middle，暫存 buffer 1，但尚未歸還 front。
3. 生產者提交 buffer 2，取得 middle 中的 buffer 1，準備寫入。
4. 消費者將 front 設為先前讀到的 buffer 1，並 store 舊 front 到 middle。

此時兩端同時持有 buffer 1，失去獨佔權。把所有操作改成 `seq_cst` 也不能把兩次操作合成一次交接。獨立的更新旗標還可能被消費者的清除操作覆蓋，遺失新提交的通知。

## 交接與記憶體順序

| 操作 | 順序 | 作用 |
|---|---|---|
| 生產者 `middle.exchange(back | dirty)` | `acq_rel` | 發布寫入，並接收可重用的舊 middle |
| 消費者 `middle.load()` | `relaxed` | 只判斷是否嘗試交接，不讀取 payload |
| 消費者 `middle.exchange(front)` | `acq_rel` | 取得快照，同時歸還舊 front 並清除 dirty |

消費者看到 dirty 後，只有它自己會清除 dirty；期間生產者即使再次提交，也只會換成另一個 dirty 狀態。因此消費者的 exchange 會取得一份已提交的快照，允許略過較舊的版本。

同步必須涵蓋兩個方向：

- **發布**：生產者先寫 payload，再 release；消費者取得該狀態時 acquire，之後才能讀取 payload。
- **歸還**：消費者讀取完舊 front，再 release；生產者接收它時 acquire，之後才能覆寫。

所以生產者的 exchange 也需要 acquire，不能只用 release。生產者連續交換時，中間的 `acq_rel` 操作也會傳遞同步關係。上述推理依據 C++ 的 [release/acquire 同步規則](https://eel.is/c%2B%2Bdraft/atomics.order) 與 [data race 定義](https://eel.is/c%2B%2Bdraft/intro.races)。

## 即時性與平台限制

交接路徑沒有 mutex、等待迴圈或動態配置，但這不等於硬即時或 wait-free 保證：

- 在初始化路徑呼叫 `IsLockFree()`，檢查目標平台的原子型別是否 lock-free；依據 [C++ atomic lock-free properties](https://eel.is/c%2B%2Bdraft/atomics.lockfree)，不能由型別名稱推論所有平台皆無鎖。
- 即使原子操作 lock-free，也沒有由此取得單次操作的最壞執行時間上界；仍需目標平台、工具鏈與 deadline 分析。
- 範例的 `alignas(64)` 是示範用對齊選擇，不保證符合所有 CPU 的 cache line 大小。
- `T` 的建構與使用者執行的填值操作也需另行分析。避免在 callback 內執行配置、釋放或 I/O。
- `main()` 中的 `cout`、`sleep_for`、thread 建立與 `join` 用於一般執行緒示範，不可直接搬進 audio callback。

## 驗證方式

從 repo 根目錄執行，產物放在暫存目錄：

```sh
clang++ -std=c++17 -O1 -g -pthread -fsanitize=thread -Wall -Wextra -Wpedantic \
  'Audio Programming/Multi-Buffering/tests/triple_buffer_test.cpp' \
  -o /tmp/handbook-triple-test
/tmp/handbook-triple-test
```

回歸測試涵蓋初始狀態、保留最新版本、沒有更新時重讀、消費者持有快照時生產者連續提交，以及 200,000 次提交下的版本單調性與 payload 一致性。

2026-11-17 在 macOS arm64、Apple Clang 17.0.0 上，測試先在原始程式重現 ThreadSanitizer data race 與 payload assertion 失敗；修正後通過。這是 host-side 動態驗證，加上上述 ownership／同步推理，不是對所有排程、CPU 或硬即時上界的證明。

## 延伸參考

- [How I Learned to Stop Worrying and Love Juggling C++ Atomics](https://brilliantsugar.github.io/posts/how-i-learned-to-stop-worrying-and-love-juggling-c++-atomics/)
- [cppreference: std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic)
- [cppreference: std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)
