#include "clipboard-manager_clipboard.h"
#include "clipboard-manager_utils.h"
#include "clipboard-manager_error.h"
#include <Windows.h>

bool cm_clipboard_copy(const char *text){
    if(!text){
        cm_error_handle(EINVAL);
        return false;
    }
    wchar_t *wcs = cm_mbstowcs((char *)text);
    if(!wcs) return false;
    size_t sizeText = (wcslen(wcs) + 1) * sizeof(wchar_t);
    HANDLE globalMem = GlobalAlloc(GMEM_MOVEABLE, sizeText);
    if(!globalMem){
        free(wcs);
        cm_error_handle_win32();
        return false;
    }
    if(memcpy_s(GlobalLock(globalMem), sizeText, wcs, sizeText)){
        free(wcs);
        cm_error_handle(errno);
        GlobalFree(globalMem);
        return false;
    }
    if(!GlobalUnlock(globalMem) && GetLastError()){
        free(wcs);
        GlobalFree(globalMem);
        cm_error_handle_win32();
        return false;
    }
    if(!OpenClipboard(NULL)){
        free(wcs);
        GlobalFree(globalMem);
        cm_error_handle_win32();
        return false;
    }
    if(!EmptyClipboard()){
        free(wcs);
        GlobalFree(globalMem);
        CloseClipboard();
        cm_error_handle_win32();
        return false;
    }
    if(!SetClipboardData(CF_UNICODETEXT, globalMem)){
        free(wcs);
        GlobalFree(globalMem);
        CloseClipboard();
        cm_error_handle_win32();
        return false;
    }
    CloseClipboard();
    free(wcs);
    return true;
}
char *cm_clipboard_paste(void){
    if(!OpenClipboard(NULL)){
        cm_error_handle_win32();
        return NULL;
    }
    HANDLE clipboardData = GetClipboardData(CF_UNICODETEXT);
    if(!clipboardData){
        CloseClipboard();
        cm_error_handle_win32();
        return NULL;
    }
    wchar_t *clipboardText = GlobalLock(clipboardData);
    if(!clipboardText){
        CloseClipboard();
        cm_error_handle_win32();
        return NULL;
    }
    size_t sizeInBytes = (wcslen(clipboardText) + 1) * sizeof(wchar_t);
    wchar_t *ret = malloc(sizeInBytes);
    if(!ret){
        CloseClipboard();
        cm_error_handle(ENOMEM);
        return NULL;
    }
    if(memcpy_s(ret, sizeInBytes, clipboardText, sizeInBytes)){
        CloseClipboard();
        free(ret);
        cm_error_handle(errno);
        return NULL;
    }
    if(!GlobalUnlock(clipboardData) && GetLastError()){
        CloseClipboard();
        free(ret);
        cm_error_handle_win32();
        return NULL;
    }
    CloseClipboard();
    char *mbs = cm_wcstombs(ret);
    if(!mbs){
        free(ret);
        return NULL;
    }
    free(ret);
    return mbs;
}