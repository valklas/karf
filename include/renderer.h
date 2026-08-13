#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

typedef enum {
    RENDER_ENGINE_MELT,
    RENDER_ENGINE_KDENLIVE
} RenderEngine;

bool renderer_execute(const char *xml_project_path, 
                     const char *output_video_path, 
                     RenderEngine engine);
bool renderer_check_dependency(RenderEngine engine);

#endif // RENDERER_H
