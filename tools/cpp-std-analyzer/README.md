# C++ STL Analyzer

[← Handbook](../../README.md) · [快速開始](QUICKSTART.md)

從 API 設計角度分析 C++ 標準庫，產出繁體中文心法文件：參數與回傳值語義、效能保證、設計取捨、適用情境與常見錯誤。

## 支援的入口

| 工具 | 設定 | 使用方式 |
|---|---|---|
| Codex | [Repo skill](../../.agents/skills/cpp-std-analyzer/SKILL.md) | `$cpp-std-analyzer 分析 std::vector 的 API 設計` |
| Claude Code skill | [Skill](../../.claude/skills/cpp-std-analyzer/SKILL.md) | `/cpp-std-analyzer 分析 std::vector 的 API 設計` |
| Claude Code agent | [Subagent](../../.claude/agents/cpp-std-analyzer.md) | `請使用 cpp-std-analyzer agent 分析 std::vector 的 API 設計` |
| Roo Code（舊入口） | [`.roomodes`](../../.roomodes) | 選擇 C++ STL Analyzer mode |

所有入口均讀取同一份 [共用 workflow](../../.agents/skills/cpp-std-analyzer/SKILL.md)。Repo 共用規則維護於 [AGENTS.md](../../AGENTS.md)，[CLAUDE.md](../../CLAUDE.md) 使用 `@AGENTS.md` 匯入。

這些設定隨 repo 使用，不會安裝到使用者的全域設定。Codex skill 在目前對話內執行流程；Claude Code agent 可在獨立 subagent context 執行。

## 產出原則

- 解釋「為何這樣設計」，提供可獨立使用的 TL;DR、必要範例與比較表。
- 一般 component 分析以約 150 行為目標；深入研究依使用者要求調整。
- 使用者的應用情境優先：一般 C++、LeetCode、Real-Time 或 Embedded。
- 區分標準保證、實作細節與實驗結果；未執行的驗證不宣稱通過。
- 先找現有文章，完成後補入適當分區索引。

## 編輯範圍

原 Roo mode 的 `fileRegex` 限制只適用於 Roo。轉換後的 agent／skill 以工作流程指示維持「只編輯本次任務相關 Markdown」；這不等於工具層的副檔名權限限制。仍需遵循各工具本身的 permissions。

Claude agent 不開放 Bash，因此編譯與 `git diff --check` 應由主對話執行並回報結果。

## 維護位置

- [共用 workflow](../../.agents/skills/cpp-std-analyzer/SKILL.md)：範圍、流程與驗證要求。
- [詳細分析指南](system_prompt.md)：設計分析與格式參考，也可手動帶入其他對話使用。
- [文件模板](template.md)：文章結構參考。
- [String API 範例](../../CppDesignConcepts/Container/cpp_string_api_reference.md)：分析粒度參考。
- [舊版 Roo Mode 規格](MODE_SPEC.md)：保留原工具格式說明。

## 格式參考

- [Codex skills 官方說明](https://developers.openai.com/codex/skills)
- [Claude Code skills 官方說明](https://code.claude.com/docs/en/skills)
- [Claude Code subagents 官方說明](https://code.claude.com/docs/en/sub-agents)
- [Claude Code memory imports](https://code.claude.com/docs/en/memory)
