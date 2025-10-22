#include "../include/backend/matrix_gen.h"
#include "../include/adapters/console_adapter.h"

int main(int argc, char* argv[]) {
    // constexpr int n = 5;
    //
    // Graph graph = create_graph(n, 0.4, 0.15);
    // print_matrix(graph.adj_matrix, n, n, "ADJ");
    // print_list(graph.adj_list, "ADJ LIST 1");
    //
    // // Graph graph2 = create_graph(n, 0.4, 0.15);
    // // print_matrix(graph2.adj_matrix, n, n, "ADJ 2");
    // // print_list(graph2.adj_list, "ADJ LIST 2");
    //
    // identify_vertices(graph, 1, 2);
    // print_matrix(graph.adj_matrix, graph.n, graph.n, "ADJ");
    // print_list(graph.adj_list, "ADJ LIST 1");
    //
    // delete_graph(graph, n);
    // // delete_graph(graph2, n);

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
