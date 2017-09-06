#pragma once

#include "Graph.h"
#include "Node.h"
#include "Position.h"
#include <vector>

std::vector<Node> aStar(const Graph &graph, const Position &source, const Position &destination);
