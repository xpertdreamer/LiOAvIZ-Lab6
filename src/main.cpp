#include "../include/backend/matrix_gen.h"

int main(int argc, char* argv[]) {
    constexpr int n = 5;
    auto loops = new int[n]();
    auto loops2 = new int[n]();

    int **adjMatrix1 = create_adjacent_matrix(n, loops, 0.3, 0.15);
    print_matrix(adjMatrix1, n, n, "ADJ");

    int** adjMatrix2 = create_adjacent_matrix(n, loops, 0.3, 0.15);
    print_matrix(adjMatrix2, n, n, "ADJ 2");

    const std::vector<std::vector<int>> adj_list = convert_to_adjacent_list(adjMatrix1, n, loops);
    print_list(adj_list, "ADJ LIST 1");

    delete_matrix(adjMatrix2, n);
    delete_matrix(adjMatrix1, n);
    delete[] loops;
    delete[] loops2;
    return 0;
}
