#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

void cli_options_init(CliOptions *opts) {
    if (!opts) return;
    opts->input_image = NULL;
    opts->output_video = NULL;
    opts->template_file = NULL;
    opts->show_help = false;
    opts->show_version = false;
}

bool cli_parse_args(int argc, char *argv[], CliOptions *opts) {
    if (!opts) return false;

    static struct option long_options[] = {
        {"input",    required_argument, 0, 'i'},
        {"output",   required_argument, 0, 'o'},
        {"template", required_argument, 0, 't'},
        {"help",     no_argument,       0, 'h'},
        {"version",  no_argument,       0, 'v'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    // Reset getopt optind
    optind = 1;

    while ((opt = getopt_long(argc, argv, "i:o:t:hv", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                opts->input_image = optarg;
                break;
            case 'o':
                opts->output_video = optarg;
                break;
            case 't':
                opts->template_file = optarg;
                break;
            case 'h':
                opts->show_help = true;
                break;
            case 'v':
                opts->show_version = true;
                break;
            case '?':
            default:
                return false;
        }
    }

    return true;
}

bool cli_validate_options(const CliOptions *opts) {
    if (!opts) return false;
    if (opts->show_help || opts->show_version) return true;

    if (!opts->input_image || !opts->template_file || !opts->output_video) {
        fprintf(stderr, "Error: Missing mandatory arguments.\n");
        fprintf(stderr, "Required options: -i/--input, -t/--template, -o/--output.\n");
        fprintf(stderr, "Use --help for usage details.\n");
        return false;
    }

    if (access(opts->input_image, R_OK) != 0) {
        fprintf(stderr, "Error: Cannot read input image file '%s'.\n", opts->input_image);
        return false;
    }

    if (access(opts->template_file, R_OK) != 0) {
        fprintf(stderr, "Error: Cannot read template XML file '%s'.\n", opts->template_file);
        return false;
    }

    return true;
}

void cli_print_help(const char *prog_name) {
    printf("KARF - Kdenlive Automated Render Flow (%s)\n", KARF_VERSION);
    printf("Automates media asset swapping inside Kdenlive templates and renders video outputs.\n\n");
    printf("Usage:\n");
    printf("  %s -i <input_image> -t <template.kdenlive> -o <output.mp4>\n\n", prog_name ? prog_name : "karf");
    printf("Options:\n");
    printf("  -i, --input <path>     Path to the input replacement image (e.g. image.png)\n");
    printf("  -t, --template <path>  Path to the .kdenlive XML template file\n");
    printf("  -o, --output <path>    Path for the output rendered video (e.g. output.mp4)\n");
    printf("  -h, --help             Displays this help user manual\n");
    printf("  -v, --version          Displays current version string\n\n");
    printf("Examples:\n");
    printf("  %s -i assets/photo.png -t templates/intro.kdenlive -o render.mp4\n", prog_name ? prog_name : "karf");
}

void cli_print_version(void) {
    printf("karf %s\n", KARF_VERSION);
}
