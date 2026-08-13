#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        // Child process
        if (engine == RENDER_ENGINE_MELT) {
            char consumer_arg[1024];
            snprintf(consumer_arg, sizeof(consumer_arg), "avformat:%s", output_video_path);

            char *args[] = {
                "melt",
                (char *)xml_project_path,
                "-consumer",
                consumer_arg,
                "real_time=-1",
                "vcodec=libx264",
                "acodec=aac",
                NULL
            };
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
