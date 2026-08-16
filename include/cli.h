#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

#include "version.h"

typedef struct {
    char *input_image;
    char *output_video;
    char *template_file;
    bool show_help;
    bool show_version;
} CliOptions;

void cli_options_init(CliOptions *opts);
bool cli_parse_args(int argc, char *argv[], CliOptions *opts);
bool cli_validate_options(const CliOptions *opts);
void cli_print_help(const char *prog_name);
void cli_print_version(void);

#endif // CLI_H
