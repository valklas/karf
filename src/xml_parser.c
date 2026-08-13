#define _DEFAULT_SOURCE

#include "xml_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static bool traverse_and_replace_resource(xmlNode *node, const char *new_image_path, bool *replaced) {
    for (xmlNode *cur = node; cur != NULL; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE) {
            // Check <property name="resource">...</property>
            if (xmlStrcmp(cur->name, (const xmlChar *)"property") == 0) {
                xmlChar *name_attr = xmlGetProp(cur, (const xmlChar *)"name");
                if (name_attr) {
                    if (xmlStrcmp(name_attr, (const xmlChar *)"resource") == 0) {
                        xmlChar *content = xmlNodeGetContent(cur);
                        if (content) {
                            xmlNodeSetContent(cur, (const xmlChar *)new_image_path);
                            *replaced = true;
                            xmlFree(content);
                        }
                    }
                    xmlFree(name_attr);
                }
            }

            // Check resource attribute directly on node if present
            xmlChar *res_attr = xmlGetProp(cur, (const xmlChar *)"resource");
            if (res_attr) {
                xmlSetProp(cur, (const xmlChar *)"resource", (const xmlChar *)new_image_path);
                *replaced = true;
                xmlFree(res_attr);
            }
        }

        if (cur->children) {
            traverse_and_replace_resource(cur->children, new_image_path, replaced);
        }
    }
    return *replaced;
}

bool xml_swap_image_resource(const char *template_path, 
                             const char *new_image_path, 
                             char *temp_output_path, 
                             size_t temp_path_size) {
    if (!template_path || !new_image_path || !temp_output_path || temp_path_size == 0) {
        return false;
    }

    LIBXML_TEST_VERSION

    xmlDoc *doc = xmlReadFile(template_path, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Error: Failed to parse XML template '%s'\n", template_path);
        return false;
    }

    xmlNode *root_element = xmlDocGetRootElement(doc);
    if (root_element == NULL) {
        fprintf(stderr, "Error: Empty XML template document in '%s'\n", template_path);
        xmlFreeDoc(doc);
        return false;
    }

    bool replaced = false;
    traverse_and_replace_resource(root_element, new_image_path, &replaced);

    if (!replaced) {
        fprintf(stderr, "Warning: No resource property found in template '%s'. Proceeding with output copy.\n", template_path);
    }

    // Generate secure temporary file
    snprintf(temp_output_path, temp_path_size, "/tmp/karf_tmpl_XXXXXX.kdenlive");
    int fd = mkstemps(temp_output_path, 9); // ".kdenlive" suffix length = 9
    if (fd == -1) {
        // Fallback without suffix if mkstemps fails
        snprintf(temp_output_path, temp_path_size, "/tmp/karf_tmpl_XXXXXX");
        fd = mkstemp(temp_output_path);
    }

    if (fd == -1) {
        fprintf(stderr, "Error: Failed to create temporary file.\n");
        xmlFreeDoc(doc);
        return false;
    }
    close(fd);

    int bytes_written = xmlSaveFormatFileEnc(temp_output_path, doc, "UTF-8", 1);
    xmlFreeDoc(doc);

    if (bytes_written <= 0) {
        fprintf(stderr, "Error: Failed to write modified XML to '%s'\n", temp_output_path);
        unlink(temp_output_path);
        return false;
    }

    return true;
}

void xml_cleanup_temp_file(const char *temp_file_path) {
    if (temp_file_path && temp_file_path[0] != '\0') {
        unlink(temp_file_path);
    }
}
