#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include "clipboard-manager_list.h"
#include "clipboard-manager_clipboard.h"
#include "clipboard-manager_error.h"
#include "clipboard-manager_utils.h"

#define CM_MAX_BUFFER 2048

char cm_buffer[CM_MAX_BUFFER];
char *cm_clipboard_text = NULL;
cm_list_t cm_history = NULL;
HANDLE cm_mutex = NULL;

bool cm_parser_command(cm_list_t tokens)
{
    char *token = cm_list_each(tokens);
    if (!token)
        return false;
    if (!strcmp(token, "copy"))
    {
        cm_buffer[0] = '\0';
        do
        {
            token = cm_list_each(NULL);
            if (token && !strcmp(token, "-f"))
            {
                token = cm_list_each(NULL);
                if (!token)
                {
                    cm_error_handle(EINVAL);
                    return false;
                }
                char *data = cm_file_read(token);
                if (!data)
                    return false;
                if (strcat_s(cm_buffer, CM_MAX_BUFFER, data))
                {
                    free(data);
                    return false;
                }
                free(data);
                if (strcat_s(cm_buffer, CM_MAX_BUFFER, " "))
                    return false;
            }
            else if (token)
            {
                if (strcat_s(cm_buffer, CM_MAX_BUFFER, token))
                    return false;
                if (strcat_s(cm_buffer, CM_MAX_BUFFER, " "))
                    return false;
            }
        } while (token);
        return cm_clipboard_copy(cm_buffer);
    }
    else if (!strcmp(token, "paste"))
    {
        char mode = 'w';
        char *text = cm_clipboard_paste();
        if (!text)
            return false;
        if (strcpy_s(cm_buffer, CM_MAX_BUFFER, text))
        {
            free(text);
            return false;
        }
        free(text);
        do
        {
            token = cm_list_each(NULL);
            if (token && !strcmp(token, "-f"))
            {
                token = cm_list_each(NULL);
                if (!token)
                {
                    cm_error_handle(EINVAL);
                    return false;
                }
                if (mode == 'w')
                    return cm_file_write(cm_buffer, token);
                else if (mode == 'a')
                    return cm_file_append(cm_buffer, token);
                else
                {
                    cm_error_handle(EINVAL);
                    return false;
                }
            }
            else if (token && !strcmp(token, "-m"))
            {
                token = cm_list_each(NULL);
                if (!token)
                {
                    cm_error_handle(EINVAL);
                    return false;
                }
                mode = token[0];
            }
            else if (token)
            {
                cm_error_handle(EINVAL);
                return false;
            }
            else
            {
                fprintf(stdout, "%s\n", cm_buffer);
            }
        } while (token);
    }
    else if (!strcmp(token, "clear"))
    {
        return cm_clipboard_copy("");
    }
    else if (!strcmp(token, "list"))
    {
        char mode[] = "w";
        do
        {
            token = cm_list_each(NULL);
            if (token && !strcmp(token, "-f"))
            {
                token = cm_list_each(NULL);
                if(!token){
                    cm_error_handle(EINVAL);
                    return false;
                }
                FILE *file = cm_file_open(token, mode);
                if (!file)
                    return false;
                if (WaitForSingleObject(cm_mutex, INFINITE) == WAIT_OBJECT_0)
                {
                    if (!cm_list_write(cm_history, file))
                    {
                        fclose(file);
                        if(!ReleaseMutex(cm_mutex))
                            cm_error_handle_win32();
                        return false;
                    }
                    if(!ReleaseMutex(cm_mutex)){
                        cm_error_handle_win32();
                        return false;
                    }
                }
                fclose(file);
                return true;
            }
            else if (token && !strcmp(token, "-m"))
            {
                token = cm_list_each(NULL);
                if (!token)
                    return false;
                mode[0] = token[0];
            }
            else if (token)
            {
                cm_error_handle(EINVAL);
                return false;
            }
            else
            {
                if (WaitForSingleObject(cm_mutex, INFINITE) == WAIT_OBJECT_0)
                {
                    if (!cm_list_write(cm_history, stdout)){
                        ReleaseMutex(cm_mutex);
                        return false;
                    }
                    ReleaseMutex(cm_mutex);
                }
            }
        } while (token);
    }
    else if (!strcmp(token, "exit"))
    {
        ExitProcess(EXIT_SUCCESS);
    }
    else if (!strcmp(token, "help"))
    {
        puts("copy[-f file]: copyies entered text or text from file to the clipboard");
        puts("paste[-m mode -f file]: Writes text from the clipboard to the screen");
        puts("or to a file in the specified mode(w/a)");
        puts("clear: cleans the clipboard");
        puts("list[-m mode -f file]: writes clipboard history to the screen or file");
        puts("in the specified mode");
        puts("help: provides informatin about commands");
        puts("exit: exits the program");
    }
    else
    {
        fprintf(stdout, "Error: invalid command! type 'help' for commands.\n");
    }
    return true;
}

bool cm_command_line()
{
    while (TRUE)
    {
        printf("command> ");
        char *input = cm_read_user_input();
        if (!input)
            continue;
        cm_list_t tokens = cm_parser_str(input, " ");
        if (!tokens)
            continue;
        if (!cm_parser_command(tokens))
        {
            cm_list_destroy(&tokens);
            continue;
        }
        cm_list_destroy(&tokens);
    }
}
bool cm_monitor()
{
    while (TRUE)
    {
        char *current = cm_clipboard_paste();
        if (!current){
            Sleep(1000);
            continue;
        }
        if (strcmp(cm_clipboard_text, current))
        {
            free(cm_clipboard_text);
            cm_clipboard_text = current;
            if (WaitForSingleObject(cm_mutex, INFINITE) == WAIT_OBJECT_0)
            {
                cm_list_push_front(cm_history, cm_clipboard_text);
                ReleaseMutex(cm_mutex);
            }
        }
        Sleep(1000);
    }
}