#define _DEFAULT_SOURCE

#include "xml_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static bool is_special_generator_resource(const char *res) {
    if (!res) return false;
    return (strcmp(res, "black") == 0 ||
            strcmp(res, "color") == 0 ||
            strcmp(res, "noise") == 0 ||
            strcmp(res, "blank") == 0 ||
            strncmp(res, "color:", 6) == 0);
}

static bool is_image_producer(xmlNode *producer_node) {
    if (!producer_node) return false;

    for (xmlNode *cur = producer_node->children; cur != NULL; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE && xmlStrcmp(cur->name, (const xmlChar *)"property") == 0) {
            xmlChar *name_attr = xmlGetProp(cur, (const xmlChar *)"name");
            if (name_attr) {
                xmlChar *content = xmlNodeGetContent(cur);
                if (content) {
                    if (xmlStrcmp(name_attr, (const xmlChar *)"mlt_service") == 0) {
                        if (xmlStrcmp(content, (const xmlChar *)"qimage") == 0 ||
                            xmlStrcmp(content, (const xmlChar *)"pixbuf") == 0 ||
                            xmlStrcmp(content, (const xmlChar *)"image") == 0) {
                            xmlFree(content);
                            xmlFree(name_attr);
                            return true;
                        }
                    }
                    if (xmlStrcmp(name_attr, (const xmlChar *)"kdenlive:clip_type") == 0) {
                        if (xmlStrcmp(content, (const xmlChar *)"2") == 0) { // 2 = Image clip in Kdenlive
                            xmlFree(content);
                            xmlFree(name_attr);
                            return true;
                        }
                    }
                    if (xmlStrcmp(name_attr, (const xmlChar *)"kdenlive:playlistid") == 0) {
                        if (xmlStrcmp(content, (const xmlChar *)"black_track") == 0) {
                            xmlFree(content);
                            xmlFree(name_attr);
                            return false;
                        }
                    }
                    xmlFree(content);
                }
                xmlFree(name_attr);
            }
        }
    }

    return false;
}

static void traverse_and_replace_resource(xmlNode *node, const char *new_image_path, bool *replaced) {
    for (xmlNode *cur = node; cur != NULL; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE) {
            // Target <producer> elements specifically
            if (xmlStrcmp(cur->name, (const xmlChar *)"producer") == 0 ||
                xmlStrcmp(cur->name, (const xmlChar *)"chain") == 0) {

                bool should_replace = is_image_producer(cur);

                // Find and replace resource property inside this producer
                for (xmlNode *p = cur->children; p != NULL; p = p->next) {
                    if (p->type == XML_ELEMENT_NODE && xmlStrcmp(p->name, (const xmlChar *)"property") == 0) {
                        xmlChar *name_attr = xmlGetProp(p, (const xmlChar *)"name");
                        if (name_attr) {
                            if (xmlStrcmp(name_attr, (const xmlChar *)"resource") == 0) {
                                xmlChar *content = xmlNodeGetContent(p);
                                if (content) {
                                    const char *res_str = (const char *)content;
                                    // Replace if marked as image producer or if resource is not special generator
                                    if (should_replace || (!is_special_generator_resource(res_str) && strchr(res_str, '/'))) {
                                        xmlNodeSetContent(p, (const xmlChar *)new_image_path);
                                        *replaced = true;
                                    }
                                    xmlFree(content);
                                }
                            }
                            xmlFree(name_attr);
                        }
                    }
                }
            }
        }

        if (cur->children) {
            traverse_and_replace_resource(cur->children, new_image_path, replaced);
        }
    }
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

    // Set root attribute to current directory or template directory so MLT doesn't resolve relative paths to ~/Videos
    char template_dir[PATH_MAX];
    strncpy(template_dir, template_path, sizeof(template_dir) - 1);
    template_dir[sizeof(template_dir) - 1] = '\0';
    char *dir_name = dirname(template_dir);

    xmlSetProp(root_element, (const xmlChar *)"root", (const xmlChar *)dir_name);

    bool replaced = false;
    traverse_and_replace_resource(root_element, new_image_path, &replaced);

    if (!replaced) {
        fprintf(stderr, "Warning: No image resource producer found in template '%s'.\n", template_path);
    }

    // Generate secure temporary file
    snprintf(temp_output_path, temp_path_size, "/tmp/karf_tmpl_XXXXXX.kdenlive");
    int fd = mkstemps(temp_output_path, 9); // ".kdenlive" suffix length = 9
    if (fd == -1) {
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
