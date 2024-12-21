#ifndef PARALLEL_ULLMAN_HPP
#define PARALLEL_ULLMAN_HPP

#include <tuple>

#include "utils.hpp"

std::vector<int> color_graph(const SimpleGraph& g);
std::vector<BinaryMatrix> parallel_ullman(const SimpleGraph& g1, const SimpleGraph& g2);

#endif // PARALLEL_ULLMAN_HPP