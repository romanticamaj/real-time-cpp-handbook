---
name: cpp-std-analyzer
description: Analyze C++ standard-library API design and write or update concise Traditional Chinese playbook notes covering parameter semantics, return values, performance guarantees, trade-offs, and practical pitfalls. Use when asked to create an STL component analysis; ordinary C++ questions do not require this workflow.
---

# C++ STL Analyzer

Analyze why a C++ standard-library interface has its shape, not just how to call it. This is a repository-scoped writing workflow shared by Codex, Claude Code, and the legacy Roo mode.

## Scope and inputs

- Follow the repository's `AGENTS.md` and the user's requested scope.
- If the component is already specified, begin directly. Ask only when the component or requested output is genuinely unclear.
- Search for existing coverage before writing. Update a relevant note rather than creating a duplicate, and preserve unrelated technical prose and working-tree changes.
- This workflow creates or edits Markdown notes and their navigation. The original Roo mode's Markdown edit scope remains a workflow rule here; do not edit source code, runtime configuration, commit, or push as a side effect.

## References

Resolve these paths relative to this file; their location is stable even when the repo is renamed:

- Read the [analysis guide](../../../tools/cpp-std-analyzer/system_prompt.md) before drafting.
- Use the [document template](../../../tools/cpp-std-analyzer/template.md) as a structural reference; keep only relevant sections.
- Consult the [string API example](../../../CppDesignConcepts/Container/cpp_string_api_reference.md) for design granularity, not as a line-count target.

## Analysis and output

1. Select the core APIs relevant to the request, typically 5–10 for a broad component analysis; narrow topics need fewer.
2. Explain important parameters, defaults, return values, ownership/lifetime, preconditions, iterator invalidation, and time/space costs where applicable.
3. Separate standard guarantees from implementation details. Verify version-specific or implementation-specific claims against primary sources; do not present a common growth strategy as a standard requirement.
4. Write Traditional Chinese prose with English technical terms and concise modern C++ examples. Target about 150 lines for a normal component playbook; explicit user requests for deeper research take precedence.
5. Include a standalone TL;DR, useful comparison tables, design principles, and common mistakes. Use the requested application context: LeetCode, general C++, real-time, or embedded. Do not imply that a general-purpose STL facility is suitable for a real-time path without analyzing its costs.
6. Link new notes from the nearest topic README. Add a homepage entry only when it answers a distinct reader question; avoid adding every article to the homepage.

## Verification and handoff

- Check the actual Markdown links and any edited navigation. Run `git diff --check` and inspect the diff for unrelated changes.
- If code was not compiled or behavior was not exercised, say so. Reasoned analysis and executed verification are separate forms of evidence.
- Report the resulting file, the design question it answers, and material verification limits.
