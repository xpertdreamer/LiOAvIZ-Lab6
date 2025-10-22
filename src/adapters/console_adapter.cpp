#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define NOCRYPT
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#endif

#include "../include/adapters/console_adapter.h"
#include "../include/backend/matrix_gen.h"
#include <filesystem>
#include <fstream>
#include <utility>

namespace fs = std::filesystem;

GraphConsoleAdapter::GraphConsoleAdapter(const std::string& config_path, const std::string& aliases_path): graphs_created(false), graph1(nullptr), graph2(nullptr), graph(nullptr), n(0) {
    // const std::string config_file = ("../../resources/config_files/graph_console.conf");
    // const std::string aliases_file = ("../../resources/config_files/aliases.conf");

    std::string actual_config_path = config_path;
    std::string actual_aliases_path = aliases_path;

    if (actual_config_path.empty()) {
        actual_config_path = find_config_file("graph_console.conf", {
            "./resources/config_files/",
            "../resources/config_files/",
            "../../resources/config_files/",
            "config/",
            "../config/",
            "./",
            get_default_config_path()
        });
    }

    if (actual_aliases_path.empty()) {
        actual_aliases_path = find_config_file("aliases.conf", {
            "./resources/config_files/",
            "../resources/config_files/",
            "../../resources/config_files/",
            "config/",
            "../config/",
            "./",
            get_default_config_path()
        });
    }

    console.load_config(actual_config_path);
    console.load_aliases(actual_aliases_path);

    register_graph_commands();
}

GraphConsoleAdapter::~GraphConsoleAdapter() {
    cleanup();
}

void GraphConsoleAdapter::run() {
    console.run();
}


void GraphConsoleAdapter::cleanup() {
    if (graph1 != nullptr) {
        delete_graph(*graph1, graph1->n);
        graph1.reset();
    }
    if (graph2 != nullptr) {
        delete_graph(*graph2, graph2->n);
        graph2.reset();
    }
    n = 0;
    graphs_created = false;
}

std::string GraphConsoleAdapter::find_config_file(const std::string &filename, const std::vector<std::string> &search_paths) {
    for (const auto& path : search_paths) {
        if (std::string full_path = path + filename; fs::exists(full_path)) {
            return full_path;
        }
    }

    return search_paths[0] + filename;
}

std::string GraphConsoleAdapter::get_default_config_path() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        return std::string(path) + "/graph_console/";
    }
    return "./";
#else
    const char* home = getenv("HOME");
    if (home == nullptr) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    return std::string(home) + "/.config/graph_console/";
#endif
}

void GraphConsoleAdapter::register_graph_commands() {
    console.register_command("create",
            [this](const std::vector<std::string>& args) { this->cmd_create(args); },
            "Create a new graph system",
            {"vertices", "edge_probability", "loop_probability"},
            "create <n> <edgeProb> <loopProb>"
        );

    console.register_command("print",
        [this](const std::vector<std::string>&) { this->cmd_print(); },
        "Print current graph system"
    );

    console.register_command("clear",
        [this](const std::vector<std::string>&) { this->cmd_clear(); },
        "Clear console screen"
    );

    console.register_command("cleanup",
        [this](const std::vector<std::string>&) { this->cmd_cleanup(); },
        "Cleanup graph system and free memory"
    );

    console.register_command("identify",
        [this](const std::vector<std::string>& args) { this->cmd_identify(args); },
            "Identify two vertices of graph",
            {"graphNum", "v", "u"}
    );

    console.register_command("contract",
        [this](const std::vector<std::string>& args) { this->cmd_contract(args); },
        "Contract an edge between two vertices of graph",
        {"graphNum", "v", "u"}
    );

    console.register_command("split",
        [this](const std::vector<std::string>& args) { this->cmd_split(args); },
        "Split a vertex",
        {"graphNum", "v"}
    );

    console.register_command("union",
        [this](const std::vector<std::string>&) { this->cmd_union(); },
        "Union graphs",
        {"graph1", "graph2"}
    );

    console.register_command("intersect",
        [this](const std::vector<std::string>&) { this->cmd_intersection(); },
        "Intersect graphs"
    );

    console.register_command("ring",
        [this](const std::vector<std::string>&) { this->cmd_ring(); },
        "Ring sum of graphs"
    );

    console.register_command("product",
        [this](const std::vector<std::string>&) { this->cmd_cartesian(); },
            "Cartesian product of graphs"
    );

    // console.register_command("save",
    //     [this](const std::vector<std::string>& args) { this->cmd_save(args); },
    //     "Save graph to file",
    //     {"filename"},
    //     "save <filename>"
    // );
    //
    // console.register_command("load",
    //     [this](const std::vector<std::string>& args) { this->cmd_load(args); },
    //     "Load graph from file",
    //     {"filename"},
    //     "load <filename>"
    // );

    console.register_command("help",
        [this](const std::vector<std::string>& args) { this->cmd_help(args); },
        "Show help for commands",
        {"command"},
        "help [command]"
    );

    console.register_command("exit",
        [this](const std::vector<std::string>&) { this->cmd_exit(); },
        "Exit the application"
    );

    console.register_command("history",
        [this](const std::vector<std::string>&) { this->cmd_history(); },
            "Show history of commands"
        );
}

void GraphConsoleAdapter::cmd_create(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "Usage: create <n> <edgeProb> <loopProb>" << std::endl;
        return;
    }

    try {
        const int new_n = std::stoi(args[0]);
        const double new_edge_prob = std::stod(args[1]);
        const double new_loop_prob = std::stod(args[2]);

        if (new_n <= 0) {
            std::cout << "Invalid number of vertices." << std::endl;
            return;
        }
        if (new_edge_prob <= 0 || new_edge_prob > 1 || new_loop_prob <= 0 || new_loop_prob > 1) {
            std::cout << "Probabilities must be between 0 and 1" << std::endl;
            return;
        }

        cleanup();

        n = new_n;
        graph1 = std::make_unique<Graph>(create_graph(n, new_edge_prob, new_loop_prob, 0));
        graph2 = std::make_unique<Graph>(create_graph(n, new_edge_prob, new_loop_prob, 0));
        graphs_created = true;

        std::cout << "Created two graphs with " << n << " vertices" << std::endl;
        std::cout << "  Edge probability: " << new_edge_prob << ", Loop probability: " << new_loop_prob << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Error creating graphs: " << e.what() << std::endl;
        std::cout << "Usage: create <vertices> <edge_probability> <loop_probability>" << std::endl;
    }
}

void GraphConsoleAdapter::cmd_print() const {
    if (!graphs_created) {
        std::cout << "No graphs created. Use 'create' command first." << std::endl;
        return;
    }

    std::cout << "=== GRAPH 1 ===" << std::endl;
    print_matrix(graph1->adj_matrix, graph1->n, graph1->n, "Adjacency Matrix 1");
    print_list(graph1->adj_list, "Adjacency List 1");

    std::cout << "=== GRAPH 2 ===" << std::endl;
    print_matrix(graph2->adj_matrix, graph2->n, graph2->n, "Adjacency Matrix 2");
    print_list(graph2->adj_list, "Adjacency List 2");

    if (graph) {
        std::cout << "=== GRAPH 3 ===" << std::endl;
        print_matrix(graph->adj_matrix, graph->n, graph->n, "Adjacency Matrix 3");
        print_list(graph->adj_list, "Adjacency List 3");
    }
}

void GraphConsoleAdapter::cmd_clear() {
    console.clear_screen();
}

void GraphConsoleAdapter::cmd_cleanup() {
    cleanup();
    std::cout << "Graph system cleaned up" << std::endl;
}

void GraphConsoleAdapter::cmd_exit() {
    cleanup();
    console.stop();
}

void GraphConsoleAdapter::cmd_help(const std::vector<std::string>& args) {
    if (args.empty()) {
        console.print_help();
    } else {
        console.show_command_help(args[0]);
    }
}

void GraphConsoleAdapter::cmd_history() {
    console.show_history();
}

void GraphConsoleAdapter::cmd_identify(const std::vector<std::string> &args) const {
    if (args.size() < 3) {
        std::cout << "Usage: identify <graphNum> <v> <u>" << std::endl;
        return;
    }

    try {
        const auto graphNum = std::stoi(args[0]);
        const auto v = stoi(args[1]);
        const auto u = stoi(args[2]);
        Graph* target = nullptr;
        if (graphNum == 1) target = graph1.get();
        else if (graphNum == 2) target = graph2.get();
        else {
            std::cout << "Invalid graph number (must be 1 or 2)" << std::endl;
            return;
        }
        if (v > target->n || v < 0 || u > target->n || u < 0 || v == u) {
            std::cout << "Invalid vertice number" << std::endl;
            return;
        }
        identify_vertices(*target, v, u);
        cmd_print();
    } catch (const std::exception& e) {
        std::cout << "Error identifying vertices: " << e.what() << std::endl;
    }
}

void GraphConsoleAdapter::cmd_contract(const std::vector<std::string> &args) const {
    if (args.size() < 3) {
        std::cout << "Usage: contract <graphNum> <v> <u>" << std::endl;
        return;
    }

    try {
        const auto graphNum = std::stoi(args[0]);
        const auto v = stoi(args[1]);
        const auto u = stoi(args[2]);
        Graph* target = nullptr;
        if (graphNum == 1) target = graph1.get();
        else if (graphNum == 2) target = graph2.get();
        else {
            std::cout << "Invalid graph number (must be 1 or 2)" << std::endl;
            return;
        }
        if (v > target->n || v < 0 || u > target->n || u < 0 || v == u) {
            std::cout << "Invalid vertice number" << std::endl;
            return;
        }
        contract_edge(*target, v, u);
        cmd_print();
    } catch (const std::exception& e) {
        std::cout << "Error identifying vertices: " << e.what() << std::endl;
    }
}

void GraphConsoleAdapter::cmd_split(const std::vector<std::string> &args) const {
    if (args.size() < 2) {
        std::cout << "Usage: split <graphNum> <v>" << std::endl;
        return;
    }

    try {
        const auto graphNum = std::stoi(args[0]);
        const auto v = stoi(args[1]);
        Graph* target = nullptr;
        if (graphNum == 1) target = graph1.get();
        else if (graphNum == 2) target = graph2.get();
        else {
            std::cout << "Invalid graph number (must be 1 or 2)" << std::endl;
            return;
        }
        if (v > target->n || v < 0) {
            std::cout << "Invalid vertice number" << std::endl;
            return;
        }
        split_vertex(*target, v, get_neighbors(*target, v));
        cmd_print();
    } catch (const std::exception& e) {
        std::cout << "Error identifying vertices: " << e.what() << std::endl;
    }
}

void GraphConsoleAdapter::cmd_union() {
    try {
        const auto source_1 = graph1.get();
        const auto source_2 = graph2.get();
        GraphConsoleAdapter::graph = std::make_unique<Graph>(graph_union(*source_1, *source_2));
    } catch (const std::exception& e) {
        std::cout << "Error while union: " << e.what() << std::endl;
    }
}

void GraphConsoleAdapter::cmd_intersection() {
    try {
        const auto source_1 = graph1.get();
        const auto source_2 = graph2.get();
        GraphConsoleAdapter::graph = std::make_unique<Graph>(graph_intersection(*source_1, *source_2));
    } catch (const std::exception& e) {
        std::cout << "Error while intersection: " << e.what() << std::endl;
    }
}

void GraphConsoleAdapter::cmd_ring() {
    try {
        const auto source_1 = graph1.get();
        const auto source_2 = graph2.get();
        GraphConsoleAdapter::graph = std::make_unique<Graph>(ring_sum(*source_1, *source_2));
    } catch (const std::exception& e) {
        std::cout << "Error while intersection: " << e.what() << std::endl;
    }
}

void GraphConsoleAdapter::cmd_cartesian() {
    try {
        const auto source_1 = graph1.get();
        const auto source_2 = graph2.get();
        GraphConsoleAdapter::graph = std::make_unique<Graph>(graph_cartesian_product(*source_1, *source_2));
    } catch (const std::exception& e) {
        std::cout << "Error while production: " << e.what() << std::endl;
    }
}
