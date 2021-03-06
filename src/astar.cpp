#include "astar.h"

#include "PriorityQueue.h"
#include <cmath>
#include <map>

namespace {
struct NodeInfo {
    NodeInfo(Node _node, float _totalCost, float _heuristic, Node _predecessor)
        : node(std::move(_node)), totalCost(_totalCost), heuristic(_heuristic),
          predecessor(std::move(_predecessor)) {}

    Node  node;
    float totalCost;   // g-value
    float heuristic;   // h-value
    Node  predecessor; // to recreate path
};

// Comparator to find nodes in PriorityQueue.
struct ValueCompare {
    bool operator()(const NodeInfo &a, const NodeInfo &b) const { return a.node < b.node; }
};

// Comparator to put cheapest nodes first.
struct PrioCompare {
    bool operator()(const NodeInfo &a, const NodeInfo &b) const {
        return a.totalCost + a.heuristic > b.totalCost + b.heuristic;
    }
};

// Heuristic distance calculations
// http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#heuristics-for-grid-maps
#define D 1
#define D2 1.414213562373f

#ifndef GRAPH_DIAGONAL_MOVEMENT
// Manhattan distance
float heuristicDistance(const Position &a, const Position &b) {
    auto dx = std::abs(a.x - b.x);
    auto dy = std::abs(a.y - b.y);
    return (float) (D * (dx + dy));
}
#else
// Diagonal distance
float heuristicDistance(const Position &a, const Position &b) {
    auto dx = std::abs(a.x - b.x);
    auto dy = std::abs(a.y - b.y);
    return D * (dx + dy) + (D2 - 2 * D) * std::min(dx, dy);
}
#endif

} // namespace

// Implementation like in https://de.wikipedia.org/wiki/A*-Algorithmus#Funktionsweise
std::vector<Node> aStar(const Graph &graph, const Position &source, const Position &destination) {
    if (source == destination)
        return {{graph, destination}};

    // Open and closed list
    PriorityQueue<NodeInfo, ValueCompare, PrioCompare> open;
    // (Tree) map seems to perform _much_ better than the unordered hash map!
    std::map<Node, Node> closed; // store predecessor as value to recreate path

    // Begin at source
    const Node sourceNode(graph, source);
    open.emplace(sourceNode, 0.0f, 0.0f, sourceNode);

    while (!open.empty()) {
        const auto current = open.top();
        open.pop();

        // Reached destination! Restore path and return.
        if (current.node.position() == destination) {
            std::vector<Node> result = {current.node, current.predecessor};

            for (auto it = closed.find(result.back()); it->second != result.back();
                 it = closed.find(result.back())) {
                result.push_back(it->second);
            }

            std::reverse(result.begin(), result.end());

            return result;
        }

        closed.emplace(current.node, current.predecessor);

        // Expand node
        for (const auto &neighbor : current.node.neighbors()) {
            const auto &nbNode = neighbor.first;
            const auto &nbStepCost = neighbor.second;

            // Already visited (cycle)
            if (closed.count(nbNode) != 0)
                continue;

            const auto nbTotalCost = current.totalCost + nbStepCost;
            const auto nbIt = open.find({nbNode, 0.0f, 0.0f, current.node});

            // Node already queued for visiting and other path cost is equal or better
            if (nbIt != open.end() && nbIt->totalCost <= nbTotalCost)
                continue;

#if 0
            const auto nbHeuristic = (destination - nbNode.position()).length();
#else
            const auto nbHeuristic = heuristicDistance(nbNode.position(), destination);
#endif

            if (nbIt != open.end())
                open.update(nbIt, {nbNode, nbTotalCost, nbHeuristic, current.node});
            else
                open.emplace(nbNode, nbTotalCost, nbHeuristic, current.node);
        }
    }

    // No path found
    return {};
}
