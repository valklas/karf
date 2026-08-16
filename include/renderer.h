#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

bool renderer_execute(const char *xml_project_path, 
                     const char *output_video_path);
bool renderer_check_dependency(void);

#endif // RENDERER_H
