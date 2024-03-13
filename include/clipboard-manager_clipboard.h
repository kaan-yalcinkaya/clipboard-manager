#if !defined(CLIPBOARD_MANAGER_CLIPBOARD_H)
#define CLIPBOARD_MANAGER_CLIPBOARD_H

#include <stdbool.h>

extern bool cm_clipboard_copy(const char *text);
char *cm_clipboard_paste(void);

#endif // CLIPBOARD_MANAGER_CLIPBOARD_H
