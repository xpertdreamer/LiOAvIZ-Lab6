#include "../include/backend/matrix_gen.h"

int main(int argc, char* argv[]) {
    constexpr int n = 5;
    auto loops = new int[n]();

    int **adjMatrix = create_adjacent_matrix(n, loops, 0.3, 1);
    print_matrix(adjMatrix, n, n, "ADJ");

    delete_matrix(adjMatrix, n);
    delete[] loops;
    return 0;
}
