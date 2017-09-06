#include "Graph.h"
#include "astar.h"
#include <chrono>
#include <iostream>

int main() {
    // Generate graph and obstacles
    Graph graph(500, 500);
    graph.generateObstacles();

    const Position source{10, 20};
    const Position destination{graph.width() - 10, graph.height() - 20};

    // A* test run
    std::cout << " ----- A* test run..." << std::endl;
    const auto start = std::chrono::high_resolution_clock::now();
    const auto path = aStar(graph, source, destination);
    const auto stop = std::chrono::high_resolution_clock::now();

    // Print timing
    std::cout << "Time for graph (" << graph.width() << ", " << graph.height()
              << "): " << std::chrono::duration<double>(stop - start).count() << " seconds"
              << std::endl;

    // Print graph to image
    graph.toPfm("AStar.pfm", path);

#ifdef _WIN32
    std::cout << "\nPress ENTER to continue..." << std::flush;
    std::cin.ignore();
#endif
    return 0;
}
