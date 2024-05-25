#include <stdio.h>
#include <windows.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: wine_injector.exe (dll) (executable) (optional args to exe)\n");
        return EXIT_FAILURE;
    }

    STARTUPINFOA si = {.cb = sizeof si};
    PROCESS_INFORMATION pi = {0};

    size_t size_of_args = 0;

    for (int i = 2; i < argc; i++)
        size_of_args += strlen(argv[i]) + 1; // argv + space/null

    char *args = (char *)malloc(size_of_args);
    if (!args) {
        printf("Failed to allocate args.");
        return EXIT_FAILURE;
    }
    ZeroMemory(args, size_of_args);

    for (int i = 2; i < argc; i++) {
        memcpy(args + strlen(args), argv[i], strlen(argv[i]));        // append argv
        memcpy(args + strlen(args), i == (argc - 1) ? "\0" : " ", 1); // if its the last arg put null, otherwise space
    }

    // for debugging purposes
    // printf("dll: %s\n", argv[1]);
    // printf("exe: %s\n", argv[2]);
    // printf("args: %s\n", args);

    if (CreateProcessA(argv[2], args, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0) {
        printf("CreateProcessA error: 0x%08x\n", GetLastError());
        return EXIT_FAILURE;
    }

    if (args)
        free(args);

    LPVOID alloc = VirtualAllocEx(pi.hProcess, NULL, strlen(argv[1]), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!alloc) {
        printf("VirtualAllocEx error: 0x%08x\n", GetLastError());
        return EXIT_FAILURE;
    }

    if (WriteProcessMemory(pi.hProcess, alloc, argv[1], strlen(argv[1]), NULL) == 0) {
        printf("WriteProcessMemory error: 0x%08x\n", GetLastError());
        return EXIT_FAILURE;
    }

    HANDLE handle = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, alloc, 0, NULL);
    if (!handle) {
        printf("CreateRemoteThread error: 0x%08x\n", GetLastError());
        return EXIT_FAILURE;
    }

    WaitForSingleObject(handle, 5000);
    VirtualFreeEx(pi.hProcess, alloc, 0, MEM_RELEASE);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
