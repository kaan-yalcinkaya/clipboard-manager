#if !defined(CLIPBOARD_MANAGER_CLI_H)
#define CLIPBOARD_MANAGER_CLI_H

#include <stdbool.h>
#include <Windows.h>
#include "clipboard-manager_list.h"

extern char *cm_clipboard_text;
extern cm_list_t cm_history;
extern HANDLE cm_mutex;

bool cm_command_line();
bool cm_monitor();

#endif // CLIPBOARD_MANAGER_CLI_H
