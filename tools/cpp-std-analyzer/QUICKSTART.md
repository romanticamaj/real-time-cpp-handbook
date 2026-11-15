# C++ STL Analyzer — 快速開始

[← 工具說明](README.md)

從這個 repo 啟動 Claude Code 或 Codex；設定為 repo scope，不需要複製到使用者的全域目錄。

## Codex

在對話內輸入：

```text
$cpp-std-analyzer 分析 std::vector 的 API 設計，先檢查現有筆記，避免重複。
```

設定：[.agents/skills/cpp-std-analyzer/SKILL.md](../../.agents/skills/cpp-std-analyzer/SKILL.md)。可用 `/skills` 或輸入 `$` 查找；若目前 session 尚未顯示新增 skill，重新開啟 session 後再試。

## Claude Code skill

在對話內輸入：

```text
/cpp-std-analyzer 分析 std::vector 的 API 設計，重點放在配置與 iterator invalidation。
```

設定：[.claude/skills/cpp-std-analyzer/SKILL.md](../../.claude/skills/cpp-std-analyzer/SKILL.md)。這個入口讀取共用 workflow，適合在主對話中完成分析。

## Claude Code agent

在對話內指定：

```text
請使用 cpp-std-analyzer agent，分析 std::vector 的 API 設計，完成後回報文件位置與驗證限制。
```

設定：[.claude/agents/cpp-std-analyzer.md](../../.claude/agents/cpp-std-analyzer.md)。可從 `/agents` 檢查或重新載入專案 agent。Agent 使用父對話的模型，讀取共用 workflow，編輯範圍限於任務相關 Markdown。

Agent 沒有 Bash；需要編譯範例或執行 `git diff --check` 時，回到主對話執行。

## 手動使用與 Roo Code

- 其他對話工具：提供 [system_prompt.md](system_prompt.md)，再指定 component、使用情境與輸出位置。
- Roo Code：保留 [`.roomodes`](../../.roomodes)，選擇 C++ STL Analyzer mode；它也讀取共用 workflow。

## 提出有效的分析請求

```text
分析 std::function 的 API 設計，說明 ownership、type erasure 與可能的配置成本。
先檢查既有筆記，只補缺少的內容；以即時／嵌入式使用情境說明取捨。
```

Component 與範圍已明確時直接開始；沒有指定特殊情境時，依現有文件脈絡安排。先閱讀現有文章，例如 [String API 設計](../../CppDesignConcepts/Container/cpp_string_api_reference.md)，再決定新增或更新。

## 完成時檢查

- 有獨立可用的 TL;DR、設計取捨、必要程式碼與常見陷阱。
- 一般 component 分析以約 150 行為目標，深入研究依使用者要求調整。
- 區分標準契約與具體標準庫／平台行為，提供必要來源。
- 新文章已加入分區 README，修改的連結能到達正確文件。
- 清楚交代實際執行的驗證，以及仍未驗證的部分。
