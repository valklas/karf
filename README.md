# KARF

KARF (**K**denlive **A**utomated **R**ender **F**low) is a lightweight C command-line tool designed to programmatically swap media assets inside Kdenlive templates and render video outputs headlessly. See the [wiki](https://github.com/valklas/wiki-karf.git) for more info.

## Getting Started

### Requirements
- Operating System: Linux
- Dependencies: `kdePackages.mlt` (`melt`), `libxml2`, `gcc`, `make`

### Quickstart with NixOS
If you are using Nix, enter the development environment with all required dependencies pre-installed:

```bash
nix develop
```

### Building from Source

Build the project with the included Makefile:

```bash
make
```

The compiled binary is written to `bin/karf`. A prebuilt executable is also
included at that path in the repository, so you can run it directly:

```bash
./bin/karf -i input.png -o output.mp4 -t template.kdenlive
```

## Usage

```
karf -i <input_image> -t <template.kdenlive> -o <output_video>

Options:
  -i, --input <path>     Path to the input replacement image (e.g. image.png)
  -t, --template <path>  Path to the .kdenlive XML template file
  -o, --output <path>    Path for the output rendered video (e.g. output.mp4|output.gif)
  -h, --help             Displays this help user manual
  -v, --version          Displays current version string
```

Supported output formats: `.mp4`, `.mov`, `.m4v`, `.gif`, `.webm`.

Paths may be relative, absolute, or use `~` (tilde) home directory shorthand.

## Known Limitations

> **FFmpeg `dc` Deprecation Warning**
>
> When rendering, you may see the following message:
> ```
> [libx264 @ ...] The "dc" option is deprecated: deprecated; use intra_dc_precision for MPEG-2 instead
> ```
> This is a **cosmetic warning** emitted internally by MLT's `avformat` consumer when passing a legacy preset option to `libx264`. It does **not** affect rendering quality, codec output, or file correctness, and can safely be ignored.

## Project Architecture

```plaintext
karf/
├── .gitignore
├── AGENTS.md
├── flake.lock
├── flake.nix
├── LICENSE
├── Makefile
├── README.md
├── bin/
│   └── karf
├── include/
│   ├── cli.h
│   ├── renderer.h
│   ├── version.h
│   └── xml_parser.h
└── src/
    ├── cli.c
    ├── main.c
    ├── renderer.c
    └── xml_parser.c

4 directories, 16 files
```

## License

This Repo is under [MIT](LICENSE).
