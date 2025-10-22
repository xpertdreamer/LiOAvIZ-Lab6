//
// Created by IWOFLEUR on 19.10.2025.
//

#include "../../include/backend/matrix_gen.h"

#include <chrono>

Graph create_graph(const int n, const double edgeProb, const double loopProb, const unsigned int seed) {
    Graph graph;
    graph.n = n;

    // Matrix memory allocating
    graph.adj_matrix = new int*[n];
    for (int i = 0; i < n; i++) {
        graph.adj_matrix[i] = new int[n];
        for (int j = 0; j < n; j++) {
            graph.adj_matrix[i][j] = 0;
        }
    }

    // List initialization
    graph.adj_list.resize(n);

    static unsigned int counter = 0;
    const auto now = std::chrono::high_resolution_clock::now();
    const auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
    unsigned int state = seed == 0 ? static_cast<unsigned int>(nanos) + counter++ : seed;

    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            state = (state * 1664525 + 1013904223) & 0x7fffffff;
            const int rand_value = static_cast<int>(state) % 100;

            graph.adj_matrix[i][j] = graph.adj_matrix[j][i] = i == j
                ? (rand_value < static_cast<int>(loopProb * 100) ? (graph.adj_list[i].push_back(i), 1) : 0)
                : (rand_value < static_cast<int>(edgeProb * 100) ? (graph.adj_list[i].push_back(j) ,1) : 0);
        }
    }

    return graph;
}

void print_matrix(int **matrix, const int rows, const int cols, const char *name) {
    std::cout << name << ": " << std::endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << std::setw(2) << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void delete_graph(Graph& graph, const int n) {
    for (int i = 0; i < n; i++) {
        delete[] graph.adj_matrix[i];
    }
    delete[] graph.adj_matrix;
    graph.adj_matrix = nullptr;
    graph.n = 0;
    graph.adj_list.resize(0);
}

std::vector<std::vector<int>> convert_to_adjacent_list(int** matrix, const int n, const int* loops) {
    std::vector<std::vector<int>> result(n);

    for (int i = 0; i < n; i++) {
        loops[i] == 1 ? result[i].push_back(i) : (void)0;

        for (int j = 0; j < n; j++) {
            i != j && matrix[i][j] == 1 ? result[i].push_back(j) : (void)0;
        }
    }

    return result;
}

void print_list(const std::vector<std::vector<int> > &list, const char* name) {
    std::cout << name << ":" << std::endl;
    for (int i = 0; i < list.size(); i++) {
        std::cout << i << ": ";
        for (const int neigh : list[i]) {
            std::cout << neigh << " ";
        }
        std::cout << std::endl;
    }
}
