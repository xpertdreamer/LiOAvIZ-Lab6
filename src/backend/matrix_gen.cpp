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

std::vector<int> get_neighbors(const Graph& graph, const int v) {
    if (v < 0 || v >= graph.n) {
        return {};
    }

    std::vector<int> neighbors;
    for (int j = 0; j < graph.n; j++) {
        if (graph.adj_matrix[v][j] == 1) {
            neighbors.push_back(j);
        }
    }
    return neighbors;
}

void split_vertex(Graph &graph, const int v, const std::vector<int> &neighbors_for_v2) {
    if (v >= graph.n || v < 0) {
        return;
    }

    const int old_n = graph.n;
    const int new_v = old_n;
    const int new_n = old_n + 1;

    // Create new matrix with one more row/column
    const auto new_matrix = new int*[new_n];
    for (int i = 0; i < new_n; i++) {
        new_matrix[i] = new int[new_n];
        for (int j = 0; j < new_n; j++) {
            new_matrix[i][j] = 0;
        }
    }

    // Copy old matrix
    for (int i = 0; i < old_n; i++) {
        for (int j = 0; j < old_n; j++) {
            new_matrix[i][j] = graph.adj_matrix[i][j];
        }
    }

    // Clean up old matrix
    for (int i = 0; i < old_n; i++) {
        delete[] graph.adj_matrix[i];
    }
    delete[] graph.adj_matrix;
    graph.adj_matrix = new_matrix;
    graph.n = new_n;

    // Resize adj_list and initialize new_v's list
    graph.adj_list.resize(new_n);

    // Add edge between v and new_v
    graph.adj_matrix[v][new_v] = graph.adj_matrix[new_v][v] = 1;
    graph.adj_list[v].push_back(new_v);
    graph.adj_list[new_v].push_back(v);

    // Move the specified neighbors to new_v
    for (int neigh : neighbors_for_v2) {
        if (neigh >= 0 && neigh < old_n && graph.adj_matrix[v][neigh]) {  // Check if actual neighbor
            // Special case if neigh == v (self-loop)
            if (neigh == v) {
                // Move loop to new_v
                graph.adj_matrix[v][v] = 0;
                graph.adj_matrix[new_v][new_v] = 1;
                // Update lists: remove v from adj_list[v] (loop)
                if (auto it = std::ranges::find(graph.adj_list[v], v); it != graph.adj_list[v].end()) {
                    graph.adj_list[v].erase(it);
                }
                // Add loop to new_v
                graph.adj_list[new_v].push_back(new_v);
            } else {
                // Disconnect from v
                graph.adj_matrix[v][neigh] = graph.adj_matrix[neigh][v] = 0;
                // Connect to new_v
                graph.adj_matrix[new_v][neigh] = graph.adj_matrix[neigh][new_v] = 1;
                // Update lists
                // Remove neigh from adj_list[v]
                if (auto it_v = std::find(graph.adj_list[v].begin(), graph.adj_list[v].end(), neigh); it_v != graph.adj_list[v].end()) {
                    graph.adj_list[v].erase(it_v);
                }
                // Remove v from adj_list[neigh]
                if (auto it_neigh = std::ranges::find(graph.adj_list[neigh], v); it_neigh != graph.adj_list[neigh].end()) {
                    graph.adj_list[neigh].erase(it_neigh);
                }
                // Add neigh to adj_list[new_v]
                graph.adj_list[new_v].push_back(neigh);
                // Add new_v to adj_list[neigh]
                graph.adj_list[neigh].push_back(new_v);
            }
        }
    }
}

Graph graph_union(const Graph &g1, const Graph &g2) {
    Graph g;
    g.n = g1.n > g2.n ? g1.n : g2.n;

    // Allocate new matrix
    g.adj_matrix = new int*[g.n];
    for (int i = 0; i < g.n; i++) {
        g.adj_matrix[i] = new int[g.n];
        for (int j = 0; j < g.n; j++) {
            // Union: an edge exists if it is in g1 OR in g2
            g.adj_matrix[i][j] = g1.adj_matrix[i][j] || g2.adj_matrix[i][j];
        }
    }

    // Initialize adj_list
    g.adj_list.resize(g.n);

    // Merging adjacency lists
    for (int i = 0; i < g.n; i++) {
        // Copying neighbors from the first graph
        g.adj_list[i] = g1.adj_list[i];

        // Add neighbors from the second graph that do not exist yet
        for (const int neigh : g2.adj_list[i]) {
            if (std::ranges::find(g.adj_list[i], neigh) == g.adj_list[i].end()) {
                g.adj_list[i].push_back(neigh);
            }
        }
    }

    return g;
}

Graph graph_intersection(const Graph &g1, const Graph &g2) {
    Graph g;
    g.n = g1.n > g2.n ? g1.n : g2.n;

    // Allocate new matrix
    g.adj_matrix = new int*[g.n];
    for (int i = 0; i < g.n; i++) {
        g.adj_matrix[i] = new int[g.n];
        for (int j = 0; j < g.n; j++) {
            // Intersection: an edge exists if it is in g1 AND in g2
            g.adj_matrix[i][j] = g1.adj_matrix[i][j] && g2.adj_matrix[i][j];
        }
    }

    // Initialize adj_list
    g.adj_list.resize(g.n);

    // Build adjacency list from the intersection matrix
    for (int i = 0; i < g.n; i++) {
        for (int j = 0; j < g.n; j++) {
            if (g.adj_matrix[i][j] == 1) {
                g.adj_list[i].push_back(j);
            }
        }
    }

    return g;
}

Graph ring_sum(const Graph &g1, const Graph &g2) {
    Graph g;
    g.n = g1.n > g2.n ? g1.n : g2.n;

    // Allocate new matrix
    g.adj_matrix = new int*[g.n];
    for (int i = 0; i < g.n; i++) {
        g.adj_matrix[i] = new int[g.n];
        for (int j = 0; j < g.n; j++) {
            int val1 = (i < g1.n && j < g1.n) ? g1.adj_matrix[i][j] : 0;
            int val2 = (i < g2.n && j < g2.n) ? g2.adj_matrix[i][j] : 0;
            g.adj_matrix[i][j] = val1 ^ val2;
        }
    }

    // Build adjacency list and check for isolated vertices
    g.adj_list.resize(g.n);
    std::vector<bool> has_real_edges(g.n, false);

    for (int i = 0; i < g.n; i++) {
        for (int j = 0; j < g.n; j++) {
            if (g.adj_matrix[i][j] == 1) {
                g.adj_list[i].push_back(j);
                // Считаем только ребра между разными вершинами
                if (i != j) {
                    has_real_edges[i] = true;
                    has_real_edges[j] = true;
                }
            }
        }
    }

    // Remove isolated vertices (including those with only self-loops)
    std::vector<int> vertices_with_edges;
    for (int i = 0; i < g.n; i++) {
        if (has_real_edges[i]) {
            vertices_with_edges.push_back(i);
        }
    }

    // Create new graph without isolated vertices
    if (vertices_with_edges.size() < g.n) {
        Graph new_g;
        new_g.n = static_cast<int>(vertices_with_edges.size());

        if (new_g.n > 0) {
            std::vector index_map(g.n, -1);
            for (size_t i = 0; i < vertices_with_edges.size(); i++) {
                index_map[vertices_with_edges[i]] = i;
            }

            new_g.adj_matrix = new int*[new_g.n];
            for (int i = 0; i < new_g.n; i++) {
                new_g.adj_matrix[i] = new int[new_g.n]{0};
            }

            new_g.adj_list.resize(new_g.n);

            for (const int old_i : vertices_with_edges) {
                const int new_i = index_map[old_i];
                for (const int old_j : vertices_with_edges) {
                    int new_j = index_map[old_j];
                    if (g.adj_matrix[old_i][old_j] == 1) {
                        new_g.adj_matrix[new_i][new_j] = 1;
                        new_g.adj_list[new_i].push_back(new_j);
                    }
                }
            }
        }

        // Clean up
        for (int i = 0; i < g.n; i++) {
            delete[] g.adj_matrix[i];
        }
        delete[] g.adj_matrix;
        return new_g;
    }

    return g;
}

// Graph graph_cartesian_product(const Graph &g1, const Graph &g2) {
//     Graph g;
//     // The number of vertices in Cartesian product is |V1| * |V2|
//     g.n = g1.n * g2.n;
//
//     // Allocate memory for the new adjacency matrix
//     g.adj_matrix = new int*[g.n];
//     for (int i = 0; i < g.n; i++) {
//         g.adj_matrix[i] = new int[g.n];
//         // Initialize with zeros (no edges)
//         for (int j = 0; j < g.n; j++) {
//             g.adj_matrix[i][j] = 0;
//         }
//     }
//
//     // Initialize adjacency list with empty vectors
//     g.adj_list.resize(g.n);
//
//     // Build Cartesian product graph
//     for (int u1 = 0; u1 < g1.n; u1++) {
//         for (int v1 = 0; v1 < g2.n; v1++) {
//             // Current vertex index in product graph
//             const int current_idx = u1 * g2.n + v1;
//
//             // Case 1: Connect to vertices with same u1 but adjacent v's in g2
//             for (const int v2_neigh : g2.adj_list[v1]) {
//                 // Add edge only in one direction to avoid duplicates
//                 if (const int neighbor_idx = u1 * g2.n + v2_neigh; !g.adj_matrix[current_idx][neighbor_idx]) {
//                     g.adj_matrix[current_idx][neighbor_idx] = 1;
//                     g.adj_matrix[neighbor_idx][current_idx] = 1;
//                 }
//             }
//
//             // Case 2: Connect to vertices with same v1 but adjacent u's in g1
//             for (const int u2_neigh : g1.adj_list[u1]) {
//                 // Add edge only in one direction to avoid duplicates
//                 if (const int neighbor_idx = u2_neigh * g2.n + v1; !g.adj_matrix[current_idx][neighbor_idx]) {
//                     g.adj_matrix[current_idx][neighbor_idx] = 1;
//                     g.adj_matrix[neighbor_idx][current_idx] = 1;
//                 }
//             }
//         }
//     }
//
//     // Build adjacency list from the final adjacency matrix
//     for (int i = 0; i < g.n; i++) {
//         g.adj_list[i].clear(); // Clear any previously added neighbors
//         for (int j = 0; j < g.n; j++) {
//             if (g.adj_matrix[i][j] == 1) {
//                 g.adj_list[i].push_back(j);
//             }
//         }
//         // Sort the adjacency list for consistency
//         std::sort(g.adj_list[i].begin(), g.adj_list[i].end());
//     }
//
//     return g;
// }

Graph graph_cartesian_product(const Graph &g1, const Graph &g2) {
    Graph g;
    // The number of vertices in Cartesian product is |V1| * |V2|
    g.n = g1.n * g2.n;

    // Allocate memory for the new adjacency matrix
    g.adj_matrix = new int*[g.n];
    for (int i = 0; i < g.n; i++) {
        g.adj_matrix[i] = new int[g.n];
        // Initialize with zeros (no edges)
        for (int j = 0; j < g.n; j++) {
            g.adj_matrix[i][j] = 0;
        }
    }

    // Build Cartesian product graph using adjacency matrices
    for (int u1 = 0; u1 < g1.n; u1++) {
        for (int v1 = 0; v1 < g2.n; v1++) {
            // Current vertex index
            const int i = u1 * g2.n + v1;

            // Case 1: Same u1, adjacent v's in g2
            for (int v2 = 0; v2 < g2.n; v2++) {
                if (g2.adj_matrix[v1][v2] == 1) {  // v1 and v2 adjacent in g2
                    const int j = u1 * g2.n + v2;
                    if (i != j) {  // Avoid self-loops from this rule
                        g.adj_matrix[i][j] = 1;
                        g.adj_matrix[j][i] = 1;
                    }
                }
            }

            // Case 2: Same v1, adjacent u's in g1
            for (int u2 = 0; u2 < g1.n; u2++) {
                if (g1.adj_matrix[u1][u2] == 1) {  // u1 and u2 adjacent in g1
                    const int j = u2 * g2.n + v1;
                    g.adj_matrix[i][j] = 1;
                    g.adj_matrix[j][i] = 1;
                    // Note: self-loops allowed here if u1 == u2 and there's a loop in g1
                }
            }
        }
    }

    // Build adjacency list from the final adjacency matrix
    g.adj_list.resize(g.n);
    for (int i = 0; i < g.n; i++) {
        g.adj_list[i].clear();
        for (int j = 0; j < g.n; j++) {
            if (g.adj_matrix[i][j] == 1) {
                g.adj_list[i].push_back(j);
            }
        }
        // Remove duplicates and sort
        std::sort(g.adj_list[i].begin(), g.adj_list[i].end());
        g.adj_list[i].erase(std::unique(g.adj_list[i].begin(), g.adj_list[i].end()), g.adj_list[i].end());
    }

    return g;
}