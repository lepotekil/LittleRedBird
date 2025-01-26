# LittleRedBird

## Introduction

**Little Red Bird** is a Windows-based program designed to demonstrate key concepts in Windows system programming, including process elevation checks, mutex handling, and file system operations. This project is intended for educational purposes only and should not be used for any malicious or unauthorized activities. The program provides hands-on insights into interacting with the Windows API and manipulating files and directories programmatically.

## Features

1. **Singleton Execution**:
   - Ensures only one instance of the program is running at a time using a named mutex.

2. **Administrator Privilege Check**:
   - Verifies that the program is executed with administrator privileges, which are required for its operations.

3. **Process Detection**:
   - Checks for the presence of a specific process, `NLSvc.exe`, to ensure it is running.

4. **User Path Construction**:
   - Dynamically retrieves the current username and constructs paths to the user's desktop and a predefined application folder (`.az-client`).

5. **File and Directory Operations**:
   - Recursively traverses specified directories and overwrites the content of all files with dummy data for demonstration purposes.

6. **Clean Resource Management**:
   - Ensures all allocated resources (e.g., memory, handles) are properly freed before program termination.

## Program Workflow

1. **Initialization**:
   - A mutex named `LittleRedBird` is created to prevent multiple instances from running simultaneously.

2. **Privilege Check**:
   - The program checks if it has administrator privileges. If not, it terminates with an error message.

3. **Process Verification**:
   - The program searches for the `NLSvc.exe` process in the system's process list. If the process is not found, execution is aborted.

4. **User Information Retrieval**:
   - The username of the current user is retrieved.

5. **Path Construction**:
   - Constructs two critical paths based on the username:
     - Path to `.az-client` within the user's AppData directory.
     - Path to the user's desktop.

6. **Directory Validation**:
   - Validates the existence of the `.az-client` folder. If the folder does not exist, the program terminates.

7. **Recursive File Rewriting**:
   - Overwrites the content of all files within the `.az-client` folder and the desktop recursively with dummy data.

8. **Resource Cleanup**:
   - Frees allocated memory, closes handles, and terminates gracefully.

## System Requirements

To compile and execute this program, the following requirements must be met:

### Software Requirements:
- **Operating System**: Windows 10 or later.
- **Compiler**: Microsoft C Compiler (MSVC).
- **Build System**: `nmake` for building the program using the provided Makefile.

### Hardware Requirements:
- **Processor**: 1 GHz or faster processor.
- **Memory**: 1 GB of RAM or more.
- **Disk Space**: At least 10 MB of free space.

## Compilation and Execution Instructions

1. **Setup the Environment**:
   - Ensure that the Microsoft Visual Studio environment is installed and configured correctly. MSVC and `nmake` should be available in the system PATH.

2. **Download the Source Code**:
   - Place the provided source files (`main.c` and `Makefile`) in the same directory.

3. **Build the Program**:
   - Open a developer command prompt.
   - Navigate to the directory containing the source files.
   - Run the following command:
     ```
     nmake
     ```

4. **Run the Executable**:
   - After successful compilation, the output executable will be located in the `output` directory. Execute the program as an administrator:
     ```
     output\littleredbird.exe
     ```

5. **Clean Build Artifacts**:
   - To clean up generated files, run:
     ```
     nmake clean
     ```

## Makefile Overview

The provided Makefile simplifies the build process by automating compilation and linking steps. Key details:

- **Compiler Flags**:
  - `/Ox` for optimization.
  - `/MT` for using the multithreaded runtime.
  - `/W4` for high warning level.
  - `/EHsc` for standard exception handling.

- **Linker Flags**:
  - Links against `Advapi32.lib` and `User32.lib` for Windows API functions.

- **Target Structure**:
  - The Makefile creates an `output` directory and places the final executable (`littleredbird.exe`) inside it.

## Disclaimer

This program is strictly for educational purposes. It demonstrates system programming concepts and should not be used for malicious or unauthorized purposes. Always ensure compliance with applicable laws and ethical guidelines when using or modifying this code.