# Repository Guide

## Purpose and scope

**Real-Time C++ Handbook** is a Traditional Chinese knowledge base for C++ engineering in real-time and embedded systems. Audio systems are one application domain. The repository contains Markdown notes, reference material, and illustrative C++ examples; it has no unified build or test suite covering all examples.

Embedded-specific articles are an incremental expansion. `Embedded/TODO.md` describes planned work, not completed technical coverage.

## Navigation and structure

- `README.md`: reader entrypoint, problem-oriented lookup, topic map, and reading routes.
- `Modern C++/`: C++11–23 features, resource management, callables, threading, synchronization, and implementation research. Its README indexes topics and versions.
- `CppDesignConcepts/`: containers, algorithms and cheatsheets, iterators, memory, and function design.
- `Audio Programming/`: latency, callbacks, concurrency, buffering, SIMD, and audio architecture. Preserve the existing series numbering, including the gap at 06.
- `Embedded/`: expansion roadmap and future embedded-specific notes.
- `Software Testing/` and `Reading/`: supporting coursework and reference material.
- `tools/cpp-std-analyzer/`: shared analysis guide, template, and usage instructions.

Inspect current files rather than assuming a remembered directory or document still exists. General distributed-system notes belong in the separate `system-design-note` repository; do not recreate that category here as a side effect.

## Editing conventions

- Write Traditional Chinese prose, preserving established English technical terms and API names. Match the surrounding document's style.
- Respect the user's requested scope. For presentation/navigation work, preserve technical prose, code, and existing paths unless explicitly authorized otherwise.
- Inspect existing coverage before creating notes; extend or cross-link suitable material instead of duplicating it.
- Favor concise, self-contained explanations of invariants, semantics, design trade-offs, and failure conditions. Keep short references short; let requested source-code research and experiments use the detail they need.
- For algorithm analyses, cover the core concept, use cases, essential C++ example, and time/space complexity. Adapt the structure to the actual subject rather than padding empty sections.
- Name new topic files descriptively in English, following nearby conventions. Preserve established Chinese names and sequential series paths.
- Use relative Markdown links. Encode spaces and `+` in link targets when needed; keep link labels readable.
- Add new articles to the nearest topic README. Keep homepage entries selective and problem-oriented.
- Mark Embedded TODOs complete only after the article and index entry exist, with verification recorded.

## Technical evidence and real-time constraints

- Distinguish standard guarantees, implementation choices, platform-specific behavior, and measured results. Cite primary sources for version-sensitive claims; pin source revisions when using line-level code links.
- Explain ownership, lifetime, memory ordering, blocking, allocation, and boundedness where they affect the design.
- In real-time audio callbacks, avoid allocation/deallocation, locks, blocking waits, and I/O. Put setup and thread-lifecycle management on non-real-time paths.
- Do not equate lock-free with bounded execution time or assume an atomic operation is lock-free on every target. State relevant platform assumptions.
- For embedded-specific examples, identify the MCU/CPU, toolchain, execution context, and resource constraints when known.

## Verification and workspace care

- Preserve unrelated working-tree edits. Do not commit, push, rename the remote, or publish unless the user asks.
- Check edited navigation links and run `git diff --check`. Check new/untracked Markdown for whitespace too, since ordinary `git diff` excludes it.
- Compile or run examples when needed to substantiate a requested claim; do not imply all repository examples were tested. Distinguish reasoning from executed verification and identify the environment used.
- Documentation-only edits do not require adding application tests or a build system.

## Optional analysis workflow

For requests to create or update an STL API design analysis, use the shared [cpp-std-analyzer skill](.agents/skills/cpp-std-analyzer/SKILL.md). Ordinary C++ questions and unrelated repo maintenance do not require that workflow.

Claude Code imports this guide through `CLAUDE.md`; Codex reads `AGENTS.md` directly. Maintain shared repository instructions here.
