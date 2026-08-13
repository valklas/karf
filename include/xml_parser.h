#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <stdbool.h>
#include <stddef.h>

bool xml_swap_image_resource(const char *template_path, 
                             const char *new_image_path, 
                             char *temp_output_path, 
                             size_t temp_path_size);
void xml_cleanup_temp_file(const char *temp_file_path);

#endif // XML_PARSER_H
