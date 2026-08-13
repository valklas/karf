# AGENTS.md — Guidelines for AI Coding Assistants

Welcome to **`karf`** (**K**denlive **A**utomated **R**ender **F**low). 

## MANDATORY INTERACTION PROTOCOL (ASK FIRST)
1. **Ask before implementing:** Outline logic and file edits, wait for confirmation.
2. **Ask before Git actions:** Do NOT commit, merge, tag, or push without confirmation.
3. **Ask before editing structural files:** `flake.nix`, `Makefile`, `README.md`, `AGENTS.md`.

## Tech Stack
- C11, Linux, NixOS (`nix develop` with `fish`).
- XML Parsing: `libxml2`.
- Engine: `kdePackages.mlt` / `kdePackages.kdenlive`.

## Binary Rules
- Binary output must be `bin/karf`.
- Must have executable permissions set in Git (`chmod +x bin/karf`).

## Git Rules
- Work on `dev` branch. `main` is for stable tags.
- Commit atomically with standard conventional messages (`feat`, `fix`, `docs`, `build`).
- Commit each feture indipendently, meaning do atomic commits.

## README Structure
- REAME.md structure should be same as it is.
