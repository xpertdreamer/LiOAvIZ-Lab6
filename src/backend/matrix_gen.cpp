//
// Created by IWOFLEUR on 19.10.2025.
//

#include "../../include/backend/matrix_gen.h"

int** create_adjacent_matrix(const int n, int* loops, const double edgeProb, const double loopProb) {
    const auto G = new int*[n];
    unsigned int state = time(nullptr);

    for (int i = 0; i < n; i++) {
        G[i] = new int[n];
        for (int j = 0; j < n; j++) {
            G[i][j] = 0;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            state = (state * 1664525 + 1013904223) & 0x7FFFFFFF;
            const int rand_value = static_cast<int>(state) % 100;

            G[i][j] = G[j][i] = (i == j)
                ? (rand_value < static_cast<int>(loopProb * 100) ? (loops[i] = 1, 1) : (loops[i] = 0, 0))
                : (rand_value < static_cast<int>(edgeProb * 100) ? 1 : 0);
        }
    }

    return G;
}

void print_matrix(int **matrix, const int rows, const int cols, const char *name) {
    cout << name << ": " << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << setw(2) << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void delete_matrix(int** matrix, const int n) {
    for (int i = 0; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

vector<vector<int>> convert_to_adjacent_list(int** matrix, const int n, const int* loops) {
    vector<vector<int>> result(n);

    for (int i = 0; i < n; i++) {
        loops[i] == 1 ? result[i].push_back(i) : (void)0;

        for (int j = 0; j < n; j++) {
            i != j && matrix[i][j] == 1 ? result[i].push_back(j) : (void)0;
        }
    }

    return result;
}

void print_list(const vector<vector<int> > &list, const char* name) {
    cout << name << ":" << endl;
    for (int i = 0; i < list.size(); i++) {
        std::cout << i << ": ";
        for (const int neigh : list[i]) {
            std::cout << neigh << " ";
        }
        std::cout << std::endl;
    }
}
