#ifndef CLIPBOARD_MANAGER_ERROR_H
#define CLIPBOARD_MANAGER_ERROR_H

#define CM_ERRMSG_MAX 256

extern void cm_error_handle(errno_t);
extern void cm_error_handle_win32(void);

#endif