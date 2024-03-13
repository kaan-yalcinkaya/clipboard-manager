#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "clipboard-manager_error.h"

char cm_error_message[CM_ERRMSG_MAX];

void cm_error_handle(errno_t err){
    strerror_s(cm_error_message, CM_ERRMSG_MAX, err);
    fprintf(stderr, "Error: %s\n", cm_error_message);
}

void cm_error_handle_win32() {
    DWORD error = GetLastError();
    LPSTR buffer = NULL;

    DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buffer,
        0,
        NULL);

    if (size > 0) {
        fprintf(stderr, "Error: %s\n", buffer);
        LocalFree(buffer);
    }
}