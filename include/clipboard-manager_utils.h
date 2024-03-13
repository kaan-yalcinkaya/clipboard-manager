#if !defined(CLIPBOARD_MANAGER_UTILS_H)
#define CLIPBOARD_MANAGER_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include "clipboard-manager_list.h"

#define CM_USERINPUT_MAX 256

extern char cm_user_input_buffer[CM_USERINPUT_MAX];

extern void *cm_file_open(const char *fileName, const char *mode);
extern bool cm_file_write(const char *text, const char *fileName);
extern bool cm_file_append(const char *text, const char *fileName);
extern char *cm_file_read(const char *fileName);
extern char *cm_read_user_input();
extern cm_list_t cm_parser_str(char *str, const char *delimiter);
extern char *cm_wcstombs(wchar_t *wcs);
extern wchar_t *cm_mbstowcs(char *mbs);

#endif // CLIPBOARD_MANAGER_UTILS_H
