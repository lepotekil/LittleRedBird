#ifndef UTILS_H
#define UTILS_H

// Function declarations
// Terminates the program with error code -1
void exit_system(void);

// Checks if the current process has elevated privileges
BOOL check_elevation(void);

// Verifies if NLSvc.exe process is running
BOOL check_nlsvc(void);

// Retrieves the current username
char* get_username(void);

// Constructs a full path by combining base path, username and suffix
char* build_path(const char* base_path, const char* username, const char* suffix);

// Recursively rewrites files in the specified directory
void rewriter(const char *dir);

#endif 