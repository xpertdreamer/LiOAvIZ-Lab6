// Created by IWOFLEUR on 19.10.2025

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

            if (i == j) {
                if (rand_value < static_cast<int>(loopProb * 100)) {
                    graph.adj_matrix[i][j] = 1;
                    graph.adj_list[i].push_back(i);
                }
            } else {
                if (rand_value < static_cast<int>(edgeProb * 100)) {
                    graph.adj_matrix[i][j] = graph.adj_matrix[j][i] = 1;
                    graph.adj_list[i].push_back(j);
                    graph.adj_list[j].push_back(i);
                }
            }
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

void identify_vertices(Graph &graph, int v, int u) {
    if (u == v || u >= graph.n || v >= graph.n || u < 0 || v < 0) {
        return;
    }

    const int n = graph.n;
    const int keep = u < v ? u : v;
    const int remove = u < v ? v : u;
    const int new_n = n - 1;

    // Merge edges into keep
    for (int j = 0; j < n; j++) {
        if (j != keep && j != remove) {
            graph.adj_matrix[keep][j] = graph.adj_matrix[keep][j] || graph.adj_matrix[remove][j];
            graph.adj_matrix[j][keep] = graph.adj_matrix[j][keep] || graph.adj_matrix[j][remove];
        }
    }

    // Merge self-loops and the edge between keep and remove into keep's self-loop
    graph.adj_matrix[keep][keep] = graph.adj_matrix[keep][keep] || graph.adj_matrix[remove][remove] || graph.adj_matrix[keep][remove];

    // Create new matrix without remove
    const auto new_matrix = new int*[new_n];
    for (int i = 0; i < new_n; i++) {
        new_matrix[i] = new int[new_n];
    }

    for (int i = 0, new_i = 0; i < n; i++) {
        if (i == remove) continue;

        for (int j = 0, new_j = 0; j < n; j++) {
            if (j == remove) continue;

            new_matrix[new_i][new_j] = graph.adj_matrix[i][j];
            new_j++;
        }
        new_i++;
    }

    // Clean up old matrix
    for (int i = 0; i < n; i++) {
        delete[] graph.adj_matrix[i];
    }
    delete[] graph.adj_matrix;
    graph.adj_matrix = new_matrix;
    graph.n = new_n;

    // Add non-self, non-keep neighbors from remove to keep, if not already present
    for (int neigh : graph.adj_list[remove]) {
        if (neigh != keep && neigh != remove) {
            bool found = false;
            for (const int ex : graph.adj_list[keep]) {
                if (ex == neigh) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                graph.adj_list[keep].push_back(neigh);
            }
        }
    }

    // Check if needs self-loop on keep from remove's self-loop or edge to keep
    bool needs_self = false;
    for (const int neigh : graph.adj_list[remove]) {
        if (neigh == keep || neigh == remove) {
            needs_self = true;
            break;
        }
    }
    bool has_self = false;
    for (const int ex : graph.adj_list[keep]) {
        if (ex == keep) {
            has_self = true;
            break;
        }
    }
    if (needs_self && !has_self) {
        graph.adj_list[keep].push_back(keep);
    }

    // For all vertices i != remove (including keep): remove entries to remove, and if removed any, add to keep if not already has
    for (int i = 0; i < n; i++) {
        if (i == remove) continue;

        bool had_remove = false;
        auto it = graph.adj_list[i].begin();
        while (it != graph.adj_list[i].end()) {
            if (*it == remove) {
                it = graph.adj_list[i].erase(it);
                had_remove = true;
            } else {
                ++it;
            }
        }
        if (had_remove) {
            bool has_keep = false;
            for (const int ex : graph.adj_list[i]) {
                if (ex == keep) {
                    has_keep = true;
                    break;
                }
            }
            if (!has_keep) {
                graph.adj_list[i].push_back(keep);
            }
        }
    }

    // Erase remove's list
    graph.adj_list.erase(graph.adj_list.begin() + remove);

    // Renumber entries > remove in all lists
    for (auto & i : graph.adj_list) {
        for (int & j : i) {
            if (j > remove) {
                --j;
            }
        }
    }
}

void contract_edge(Graph &graph, const int v, const int u) {
    if (u == v || u >= graph.n || v >= graph.n || u < 0 || v < 0) {
        return;
    }

    const int n = graph.n;
    const int keep = u < v ? u : v;
    const int remove = u < v ? v : u;
    const int new_n = n - 1;

    if (!graph.adj_matrix[u][v] && !graph.adj_matrix[v][u]) {
        std::cout << "No such edge" << std::endl;
        return;
    }

    // Merge edges into keep
    for (int j = 0; j < n; j++) {
        if (j != keep && j != remove) {
            graph.adj_matrix[keep][j] = graph.adj_matrix[keep][j] || graph.adj_matrix[remove][j];
            graph.adj_matrix[j][keep] = graph.adj_matrix[j][keep] || graph.adj_matrix[j][remove];
        }
    }

    // Merge self-loops (but not the edge between keep and remove)
    graph.adj_matrix[keep][keep] = graph.adj_matrix[keep][keep] || graph.adj_matrix[remove][remove];

    // Create new matrix without remove
    const auto new_matrix = new int*[new_n];
    for (int i = 0; i < new_n; i++) {
        new_matrix[i] = new int[new_n];
    }

    for (int i = 0, new_i = 0; i < n; i++) {
        if (i == remove) continue;

        for (int j = 0, new_j = 0; j < n; j++) {
            if (j == remove) continue;

            new_matrix[new_i][new_j] = graph.adj_matrix[i][j];
            new_j++;
        }
        new_i++;
    }

    // Clean up old matrix
    for (int i = 0; i < n; i++) {
        delete[] graph.adj_matrix[i];
    }
    delete[] graph.adj_matrix;
    graph.adj_matrix = new_matrix;
    graph.n = new_n;

    // Add non-self, non-keep neighbors from remove to keep, if not already present
    for (int neigh : graph.adj_list[remove]) {
        if (neigh != keep && neigh != remove) {
            bool found = false;
            for (const int ex : graph.adj_list[keep]) {
                if (ex == neigh) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                graph.adj_list[keep].push_back(neigh);
            }
        }
    }

    // Check if needs self-loop on keep from remove's self-loop only
    bool needs_self = false;
    for (const int neigh : graph.adj_list[remove]) {
        if (neigh == remove) {
            needs_self = true;
            break;
        }
    }
    bool has_self = false;
    for (const int ex : graph.adj_list[keep]) {
        if (ex == keep) {
            has_self = true;
            break;
        }
    }
    if (needs_self && !has_self) {
        graph.adj_list[keep].push_back(keep);
    }

    // Special handling for keep: remove entries to remove, but do not add self-loop from it
    auto it_keep = graph.adj_list[keep].begin();
    while (it_keep != graph.adj_list[keep].end()) {
        if (*it_keep == remove) {
            it_keep = graph.adj_list[keep].erase(it_keep);
        } else {
            ++it_keep;
        }
    }

    // For all vertices i != remove and i != keep: remove entries to remove, and if removed any, add to keep if not already has
    for (int i = 0; i < n; i++) {
        if (i == remove || i == keep) continue;

        bool had_remove = false;
        auto it = graph.adj_list[i].begin();
        while (it != graph.adj_list[i].end()) {
            if (*it == remove) {
                it = graph.adj_list[i].erase(it);
                had_remove = true;
            } else {
                ++it;
            }
        }
        if (had_remove) {
            bool has_keep = false;
            for (const int ex : graph.adj_list[i]) {
                if (ex == keep) {
                    has_keep = true;
                    break;
                }
            }
            if (!has_keep) {
                graph.adj_list[i].push_back(keep);
            }
        }
    }

    // Erase remove's list
    graph.adj_list.erase(graph.adj_list.begin() + remove);

    // Renumber entries > remove in all lists
    for (auto & i : graph.adj_list) {
        for (int & j : i) {
            if (j > remove) {
                --j;
            }
        }
    }
}