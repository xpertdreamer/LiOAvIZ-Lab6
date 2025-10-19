//
// Created by IWOFLEUR on 19.10.2025.
//

#ifndef MATRIX_GEN_H
#define MATRIX_GEN_H

#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std;

// Function for allocating memory for a matrix
inline int** create_adjacent_matrix(const int n, int* loops, double edgeProb, double loopProb) {
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

// Function to display the matrix
inline void print_matrix(int **matrix, const int rows, const int cols, const char *name) {
    cout << name << ": " << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << setw(2) << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

inline void delete_matrix(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

#endif //MATRIX_GEN_H
