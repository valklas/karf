#define _DEFAULT_SOURCE

#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>

void cli_options_init(CliOptions *opts) {
    if (!opts) return;
    opts->input_image = NULL;
    opts->output_video = NULL;
    opts->template_file = NULL;
    opts->show_help = false;
    opts->show_version = false;
}

static char *resolve_to_absolute_path(const char *path, bool must_exist) {
    if (!path) return NULL;

    char resolved[PATH_MAX];
    if (realpath(path, resolved) != NULL) {
        return strdup(resolved);
    }

    if (must_exist) {
        return NULL;
    }

    // For output files that might not exist yet, resolve parent dir
    char path_copy[PATH_MAX];
    strncpy(path_copy, path, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';

    char *dir = dirname(path_copy);
    char dir_resolved[PATH_MAX];
    if (realpath(dir, dir_resolved) != NULL) {
        strncpy(path_copy, path, sizeof(path_copy) - 1);
        char *base = basename(path_copy);
        snprintf(resolved, sizeof(resolved), "%s/%s", dir_resolved, base);
        return strdup(resolved);
    }

    // Fallback to original path copy if directory resolution fails
    return strdup(path);
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

    // Resolve paths to absolute paths
    char *abs_input = resolve_to_absolute_path(opts->input_image, true);
    char *abs_template = resolve_to_absolute_path(opts->template_file, true);
    char *abs_output = resolve_to_absolute_path(opts->output_video, false);

    if (!abs_input || !abs_template || !abs_output) {
        fprintf(stderr, "Error: Failed to resolve absolute file paths.\n");
        free(abs_input);
        free(abs_template);
        free(abs_output);
        return false;
    }

    // Cast away const to store canonical absolute paths
    CliOptions *mutable_opts = (CliOptions *)opts;
    mutable_opts->input_image = abs_input;
    mutable_opts->template_file = abs_template;
    mutable_opts->output_video = abs_output;

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
