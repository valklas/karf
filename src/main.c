#include <stdio.h>
#include <stdlib.h>
#include "cli.h"
#include "xml_parser.h"
#include "renderer.h"

int main(int argc, char *argv[]) {
    CliOptions opts;
    cli_options_init(&opts);

    if (!cli_parse_args(argc, argv, &opts)) {
        return EXIT_FAILURE;
    }

    if (opts.show_help) {
        cli_print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    if (opts.show_version) {
        cli_print_version();
        return EXIT_SUCCESS;
    }

    if (!cli_validate_options(&opts)) {
        return EXIT_FAILURE;
    }

    char temp_xml_path[512] = {0};

    printf("Swapping image resource '%s' into template '%s'...\n", opts.input_image, opts.template_file);

    if (!xml_swap_image_resource(opts.template_file, opts.input_image, temp_xml_path, sizeof(temp_xml_path))) {
        fprintf(stderr, "Error: Failed to swap image resource in XML template.\n");
        return EXIT_FAILURE;
    }

    printf("Rendering project to '%s' using MLT engine...\n", opts.output_video);

    bool render_ok = renderer_execute(temp_xml_path, opts.output_video, RENDER_ENGINE_MELT);

    xml_cleanup_temp_file(temp_xml_path);

    if (!render_ok) {
        fprintf(stderr, "Error: Video rendering failed.\n");
        return EXIT_FAILURE;
    }

    printf("Done! Rendered output: %s\n", opts.output_video);
    return EXIT_SUCCESS;
}
