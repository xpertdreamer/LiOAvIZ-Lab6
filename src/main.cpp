#include "../include/adapters/console_adapter.h"

int main(int argc, char* argv[]) {
    try {
        GraphConsoleAdapter console;
        console.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
