#include "clipboard-manager_cli.h"
#include "clipboard-manager_clipboard.h"
#include "clipboard-manager_error.h"

#define CM_NUM_THREADS 2

int main()
{
    if(!cm_clipboard_copy("")) return false;
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    cm_mutex = CreateMutex(NULL, FALSE, NULL);
    if (!cm_mutex)
    {
        cm_error_handle_win32();
        return 1;
    }
    HANDLE threads[CM_NUM_THREADS];
    threads[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cm_command_line, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cm_monitor, NULL, 0, NULL);
    if (!threads[0] || !threads[1])
    {
        cm_error_handle_win32();
        return 1;
    }
    cm_clipboard_text = cm_clipboard_paste();
    if (!cm_clipboard_text)
        return 1;
    cm_history = cm_list_init();
    if (!cm_history)
        return 1;
    WaitForMultipleObjects(CM_NUM_THREADS, threads, TRUE, INFINITE);
}