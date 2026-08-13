#define _DEFAULT_SOURCE

#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static bool ends_with_icase(const char *str, const char *suffix) {
    if (!str || !suffix) return false;
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) return false;
    return (strcasecmp(str + str_len - suffix_len, suffix) == 0);
}

bool renderer_check_dependency(RenderEngine engine) {
    const char *binary_name = (engine == RENDER_ENGINE_MELT) ? "melt" : "kdenlive";
    char command[256];
    snprintf(command, sizeof(command), "which %s > /dev/null 2>&1", binary_name);
    int status = system(command);
    return (status == 0);
}

bool renderer_execute(const char *xml_project_path, 
                     const char *output_video_path, 
                     RenderEngine engine) {
    if (!xml_project_path || !output_video_path) return false;

    if (!renderer_check_dependency(engine)) {
        const char *engine_name = (engine == RENDER_ENGINE_MELT) ? "melt" : "kdenlive";
        fprintf(stderr, "Error: Required rendering engine '%s' is not installed or not in PATH.\n", engine_name);
        return false;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return false;
    } else if (pid == 0) {
        // Child process: set environment variables to suppress FFmpeg / MLT deprecation warnings
        setenv("AV_LOG_LEVEL", "error", 1);
        setenv("MLT_LOG_LEVEL", "error", 1);

        if (engine == RENDER_ENGINE_MELT) {
            char consumer_arg[1024];
            snprintf(consumer_arg, sizeof(consumer_arg), "avformat:%s", output_video_path);

            char *args[16];
            int arg_idx = 0;
            args[arg_idx++] = "melt";
            args[arg_idx++] = (char *)xml_project_path;
            args[arg_idx++] = "-consumer";
            args[arg_idx++] = consumer_arg;
            args[arg_idx++] = "real_time=-1";
            args[arg_idx++] = "dc=0";
            args[arg_idx++] = "mlt_log=quiet";

            if (ends_with_icase(output_video_path, ".gif")) {
                args[arg_idx++] = "vcodec=gif";
            } else if (ends_with_icase(output_video_path, ".mp4") ||
                       ends_with_icase(output_video_path, ".mov") ||
                       ends_with_icase(output_video_path, ".m4v")) {
                args[arg_idx++] = "vcodec=libx264";
                args[arg_idx++] = "acodec=aac";
            } else if (ends_with_icase(output_video_path, ".webm")) {
                args[arg_idx++] = "vcodec=libvpx-vp9";
                args[arg_idx++] = "acodec=libopus";
            }
            args[arg_idx] = NULL;

            execvp("melt", args);
            perror("execvp melt failed");
            exit(EXIT_FAILURE);
        } else {
            char *args[] = {
                "kdenlive",
                "--render",
                (char *)xml_project_path,
                (char *)output_video_path,
                NULL
            };
            execvp("kdenlive", args);
            perror("execvp kdenlive failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return false;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Rendering completed successfully: %s\n", output_video_path);
            return true;
        } else {
            fprintf(stderr, "Error: Rendering failed with exit code %d.\n", WEXITSTATUS(status));
            return false;
        }
    }
}
