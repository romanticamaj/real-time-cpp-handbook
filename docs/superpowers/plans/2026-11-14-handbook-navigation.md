# Handbook Navigation Implementation Plan

> **For agentic workers:** Use superpowers:executing-plans to implement this documentation plan task-by-task. Steps use checkbox syntax for tracking.

**Goal:** 建立 Real-Time C++ Handbook 的首頁、分區索引與 Embedded TODO。

**Architecture:** 保留現有文件位置，以 README 提供問題導向與完整主題索引。Embedded 先提供 roadmap，不建立空白技術文章。

**Tech Stack:** GitHub Markdown、相對連結、Python 驗證。

**Spec:** [Approved navigation design](../specs/2026-11-14-handbook-navigation-design.md)

## Global Constraints

- 現有技術本文、程式碼、PDF 不變。
- 保留使用者既有 working-tree 修改。
- 不更名遠端、不提交、不推送。

## Review Focus

- 空白、中文與 `+` 路徑使用 URL encoding。
- 每篇主要技術文件都能從分區索引抵達。
- 新增文件不得把 Embedded TODO 描述成完成內容。
- 工具 README 修正舊 string 文件路徑；其他技術文件不改。
- 驗證原有技術文件的雜湊與修改前一致。

## Task 1: 導覽

- [x] 新增根目錄及各主題 README，建立學習與查詢路徑。
- [x] 納入目前存在的 Modern C++ 文件，僅修正既有工具 README 連結。
- [x] 驗證新增本地連結與索引覆蓋。

## Task 2: Embedded Roadmap

- [x] 新增 `Embedded/README.md` 與 `Embedded/TODO.md`。
- [x] 依優先順序列出主題、練習成果與官方參考入口。
- [x] 驗證首頁連通、待辦狀態與技術本文雜湊，執行 `git diff --check`。

## 執行紀錄

已完成文件與索引；本地連結、章節錨點、主要分區文件覆蓋及本文完整性檢查通過。Embedded 共 57 個待補主題。使用者既有 threading／reference_wrapper 文件保持原樣。

執行調整：本次為文件導覽變更，直接在目前工作目錄完成，不另外搬移工作樹或提交。以本地連結／錨點檢查、文件覆蓋、原始檔案雜湊與 whitespace 檢查驗收，不新增應用程式測試。
