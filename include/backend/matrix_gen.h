//
// Created by IWOFLEUR on 19.10.2025.
//

#ifndef MATRIX_GEN_H
#define MATRIX_GEN_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

// Function for allocating memory for a matrix
extern int** create_adjacent_matrix(int n, int* loops, double edgeProb, double loopProb);

// Function to display the matrix
extern void print_matrix(int **matrix, int rows, int cols, const char *name);

// Free matrix memory
extern void delete_matrix(int** matrix, int n);

// Convert exiting adj matrix to adj list
extern vector<vector<int>> convert_to_adjacent_list(int** matrix, int n, const int* loops);

// Display adj list
extern void print_list(const vector<vector<int>> &list, const char *name);

#endif //MATRIX_GEN_H