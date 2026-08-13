# KARF

KARF (**K**denlive **A**utomated **R**ender **F**low) is a lightweight C command-line tool designed to programmatically swap media assets inside Kdenlive templates and render video outputs headlessly. See the [wiki](https://github.com/valklas/wiki-karf.git) for more info.

## Getting Started

### Requirements
- Operating System: Linux
- Dependencies: `kdePackages.mlt` / `kdePackages.kdenlive`, `libxml2`, `gcc`, `make`

### Quickstart with NixOS
If you are using Nix, enter the development environment with all required dependencies pre-installed:

```bash
nix develop
```

### Building from Source

```bash
make
./bin/karf -i input.png -o output.mp4 -t template.kdenlive
```

## Project Architecture

```plaintext
karf/
├── AGENTS.md
├── flake.nix
├── LICENSE
├── Makefile
├── bin/
│   └── karf
├── include/
│   ├── cli.h
│   ├── renderer.h
│   └── xml_parser.h
├── src/
│   ├── cli.c
│   ├── main.c
│   ├── renderer.c
│   └── xml_parser.c
└── README.md

4 directories, 12 files
```

## License

This Repo is under [MIT](LICENSE).
