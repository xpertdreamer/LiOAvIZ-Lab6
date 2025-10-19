//
// Created by IWOFLEUR on 13.10.2025.
//

#ifndef CONSOLE_ADAPTER_H
#define CONSOLE_ADAPTER_H

#include "../core/console.h"

class GraphConsoleAdapter {
    public:
    GraphConsoleAdapter();
    ~GraphConsoleAdapter();

    void run();
    private:
    Console console;

    bool graph_created;
    int** adjMatrix;
    int* loops;
    int n;
};

#endif //CONSOLE_ADAPTER_H
