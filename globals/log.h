#pragma once
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <windows.h>  // For Windows-specific color control
#include <cstdarg>    // For variadic arguments

// Function to set console text color
inline void SetConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Helper function to print formatted messages
inline void logMessage(const char* format, va_list args) {
    vprintf(format, args);
    printf("\n");
}

// Log error messages in red
inline void errorLog(const char* format, ...) {
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);  // Red text
    printf("[-] ");

    va_list args;
    va_start(args, format);
    logMessage(format, args);
    va_end(args);

    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  // Reset to white
}

// Log console messages in blue
inline void consoleLog(const char* format, ...) {
    SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);  // Blue text
    printf("[+] ");

    va_list args;
    va_start(args, format);
    logMessage(format, args);
    va_end(args);

    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  // Reset to white
}

inline void pause() {
    while (true) {};
}

#endif // LOG_H
