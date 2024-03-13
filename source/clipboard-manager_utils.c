#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <Windows.h>
#include "clipboard-manager_utils.h"
#include "clipboard-manager_error.h"

char cm_user_input_buffer[CM_USERINPUT_MAX];

void *cm_file_open(const char *fileName, const char *mode){
    if(!fileName || !mode){
        cm_error_handle(EINVAL);
        return NULL;
    }
    FILE *file = NULL;
    if(fopen_s(&file, fileName, mode)){
        cm_error_handle(errno);
        return NULL;
    }
    return file;
}

bool cm_file_write(const char *text, const char *fileName){
    FILE *file = cm_file_open(fileName, "w");
    if(!file) return false;
    fprintf_s(file, "%s", text);
    fclose(file);
    return true;
}
bool cm_file_append(const char *text, const char *fileName){
    FILE *file = cm_file_open(fileName, "a");
    if(!file) return false;
    fprintf_s(file, "%s", text);
    fclose(file);
    return true;
}
char *cm_file_read(const char *fileName){
    FILE *file = cm_file_open(fileName, "rb");
    if(!file)
        return NULL;
    if(fseek(file, 0, SEEK_END)){
        fclose(file);
        cm_error_handle(errno);
        return NULL;
    }
    long size = ftell(file);
    if(size == -1L){
        fclose(file);
        cm_error_handle(errno);
        return NULL;
    }
    if(fseek(file, 0 ,SEEK_SET)){
        fclose(file);
        cm_error_handle(errno);
        return NULL;
    }
    char *ret = malloc(size + 1);
    if(!ret){
        fclose(file);
        cm_error_handle(ENOMEM);
        return NULL;
    }
    if(!fread_s(ret, size + 1, sizeof(char), size, file) && ferror(file)){
        fclose(file);
        free(ret);
        cm_error_handle(errno);
        return NULL;
    }
    fclose(file);
    ret[size] = '\0';
    return ret;
}
char *cm_read_user_input(){
    if(!fgets(cm_user_input_buffer, CM_USERINPUT_MAX, stdin) && ferror(stdin)){
        cm_error_handle(errno);
        return NULL;
    }
    size_t size = strlen(cm_user_input_buffer);
    if(cm_user_input_buffer[size - 1] == '\n')
        cm_user_input_buffer[size - 1] = '\0';
    return cm_user_input_buffer;
}

cm_list_t cm_parser_str(char *str, const char *delimiter){
    if(!str || !delimiter){
        cm_error_handle(EINVAL);
        return NULL;
    }
    char copyStr[CM_USERINPUT_MAX];
    if(strcpy_s(copyStr, CM_USERINPUT_MAX, str)){
        cm_error_handle(errno);
        return NULL;
    }
    cm_list_t tokens = cm_list_init();
    if(!tokens) return NULL;
    char *token;
    char *token_next;
    token = strtok_s(str, delimiter, &token_next);
    while (token)
    {
        if(!cm_list_push(tokens, token)) {
            cm_list_destroy(&tokens);
            return NULL;
        }
        token = strtok_s(NULL, delimiter, &token_next);
    }
    return tokens;
}

char *cm_wcstombs(wchar_t *wcs){
    int sizeMBS = WideCharToMultiByte(
        CP_UTF8,
        0,
        wcs,
        -1,
        NULL,
        0,
        NULL,
        NULL);
    if(!sizeMBS){
        cm_error_handle_win32();
        return NULL;
    }
    char *mbs = malloc(sizeMBS);
    if(!mbs){
        cm_error_handle(ENOMEM);
        return NULL;
    }
    int result = WideCharToMultiByte(
        CP_UTF8,
        0,
        wcs,
        -1,
        mbs,
        sizeMBS,
        NULL,
        NULL
    );
    if(!result){
        cm_error_handle_win32();
        free(mbs);
        return NULL;
    }
    return mbs;
}
wchar_t *cm_mbstowcs(char *mbs){
    int sizeWCS = MultiByteToWideChar(
        CP_UTF8,
        0,
        mbs,
        -1,
        NULL,
        0
    );
    if(!sizeWCS){
        cm_error_handle_win32();
        return NULL;
    }
    wchar_t *wcs = malloc(sizeWCS * sizeof(wchar_t));
    if(!wcs){
        cm_error_handle(ENOMEM);
        return NULL;
    }    
    int result = MultiByteToWideChar(
        CP_UTF8,
        0,
        mbs,
        -1,
        wcs,
        sizeWCS
    );
    if(!result){
        free(wcs);
        cm_error_handle_win32();
        return NULL;
    }
    return wcs;
}