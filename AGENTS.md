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
- Must have executable permissions set in Git: `chmod +x bin/karf && git add --chmod=+x bin/karf`.
- Rebuild binary after any source change: `make clean all`.

## Git Rules
- Work on `dev` branch. `main` is for stable tags only.
- Commit atomically with standard conventional messages (`feat`, `fix`, `docs`, `build`).
- Commit each feature independently — do atomic commits.
- Always rebuild the binary and commit it alongside source changes.

## Commit Workflow
```bash
# Stage and commit source changes (example)
git add src/<file>.c
git commit -m "feat|fix|docs|build(<scope>): <short description>"

# Always rebuild and commit the binary after source changes
make clean all
git add --chmod=+x bin/karf
git commit -m "build: update bin/karf executable binary"
```

## Tagging & Release Protocol
Tags are applied to `main` after merging `dev`. Always ask the user before any tag, merge, or push.

```bash
# 1. Merge dev into main (ask first)
git checkout main
git merge --no-ff dev -m "chore: release <version>"

# 2. Tag the release on main (ask first)
git tag -a <version> -m "Release <version>"

# 3. Push branch + tag (ask first)
git push origin main
git push origin dev
git push origin <version>
```

Tag naming convention: `v<major>.<minor>.<patch>[-<pre-release>]`
- Example stable: `v0.1.0`
- Example alpha:  `v0.1.0-alpha.1`

## README Structure
- README.md structure should remain consistent with the existing layout.

## Known Issues
- The `[libx264] The "dc" option is deprecated` message is a cosmetic warning from MLT's
  internal `avformat` consumer. It does not affect rendering output and can be ignored.
