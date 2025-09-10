#pragma once

#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>

typedef struct {
    std::string inputFile;
    std::string outputFile;
    std::string proxyFile;
    std::string usernameFile;
    std::string passwordFile;
    int threads;
} ConsoleArgs;

int parse_arguments(int argc, char *argv[], ConsoleArgs *);

#endif // CONSOLE_H
