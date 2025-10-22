//
// Created by IWOFLEUR on 19.10.2025.
//

#ifndef MATRIX_GEN_H
#define MATRIX_GEN_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

struct Graph {
    int** adj_matrix;
    std::vector<std::vector<int>> adj_list;
    int n;
};

// Function for allocating memory for a graph
extern Graph create_graph(int n, double edgeProb = 0.4, double loopProb = 0.15, unsigned int seed = 0);

// Function to display the matrix
extern void print_matrix(int **matrix, int rows, int cols, const char *name);

// Free matrix memory
extern void delete_graph(Graph& graph, int n);

// Convert exiting adj matrix to adj list
extern std::vector<std::vector<int>> convert_to_adjacent_list(int** matrix, int n, const int* loops);

// Display adj list
extern void print_list(const std::vector<std::vector<int>> &list, const char *name);

/**
 * Identify two vertices of graph
 * @param graph Modifiable graph
 * @param v First vertex number 0 - n-1
 * @param u Second vertex number 0 - n-1
 */
extern void identify_vertices(Graph& graph, int v, int u);

/**
 * Contract an edge between two vertices of graph
 * @param graph Modifiable graph
 * @param v First vertex number 0 - n-1
 * @param u Second vertex number 0 - n-1
 */
extern void contract_edge(Graph &graph, int v, int u);
#endif //MATRIX_GEN_H