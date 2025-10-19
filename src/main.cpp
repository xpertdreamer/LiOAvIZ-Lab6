#include "../include/backend/matrix_gen.h"

int main(int argc, char* argv[]) {
    constexpr int n = 5;

    Graph graph = create_graph(n, 0.4, 0.15);
    print_matrix(graph.adj_matrix, n, n, "ADJ");
    print_list(graph.adj_list, "ADJ LIST 1");

    Graph graph2 = create_graph(n, 0.4, 0.15);
    print_matrix(graph2.adj_matrix, n, n, "ADJ 2");
    print_list(graph2.adj_list, "ADJ LIST 2");

    delete_graph(graph, n);
    delete_graph(graph2, n);
    return 0;
}
