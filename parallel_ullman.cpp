#include <omp.h>
#include <tuple>

#include "utils.hpp"

std::vector<std::vector<int>> color_graph(const SimpleGraph& g)
{
    int vertices = g.vertex_count();
    std::vector<int> colors(vertices, -1);

    colors[0] = 0;

    std::vector<bool> used(vertices, false);

    for (int i = 1; i < vertices; i++) {
        std::fill(used.begin(), used.end(), false);

        for (int neighbor : g.neighbors(i)) {
            if (colors[neighbor] != -1) {
                used[colors[neighbor]] = true;
            }
        }

        for (int color = 0; color < vertices; color++) {
            if (!used[color]) {
                colors[i] = color;
                break;
            }
        }
    }

    std::vector<std::vector<int>> nodes_by_color(*std::max_element(colors.begin(), colors.end()) + 1);
    for (int i = 0; i < vertices; i++) {
        nodes_by_color[colors[i]].push_back(i);
    }

    return nodes_by_color;
}

bool is_isomorphism(const SimpleGraph& g, const SimpleGraph& h, const BinaryMatrix& M)
{
    int n = g.vertex_count();

    std::vector<int> mapping(n, -1);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (M.get(i, j) == 1) {
                mapping[i] = j;
                break;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        int mappedI = mapping[i];
        for (int j = 0; j < n; j++) {
            int mappedJ = mapping[j];
            if (g.has_edge(i, j) != h.has_edge(mappedI, mappedJ)) {
                return false;
            }
        }
    }

    return true;
}

void prune(const SimpleGraph& g, const SimpleGraph& h,
    const std::vector<std::vector<int>>& g_colors, BinaryMatrix& M)
{
    int n = g.vertex_count();
    int max_color = g_colors.size() - 1;

    for (int color = 0; color <= max_color; color++) {
#pragma omp parallel for
        for (size_t idx = 0; idx < g_colors[color].size(); idx++) {
            int i = g_colors[color][idx];
            for (int j = 0; j < n; j++) {
                if (M.get(i, j) == 1) {
                    for (auto neighbor_g : g.neighbors(i)) {
                        bool can_map = false;
                        for (auto neighbor_h : h.neighbors(j)) {
                            if (M.get(neighbor_g, neighbor_h) == 1) {
                                can_map = true;
                                break;
                            }
                        }
                        if (!can_map) {
                            M.set(i, j, 0);
                            break;
                        }
                    }
                }
            }
        }
#pragma omp barrier
    }
}

void parallel_ullman_recurse(BinaryMatrix used_colmns,
    int cur_row,
    const SimpleGraph& g,
    const SimpleGraph& h,
    const std::vector<std::vector<int>>& g_colors,
    const BinaryMatrix M,
    std::vector<BinaryMatrix>& isos)
{
    int n = g.vertex_count();

    if (cur_row == n) {
        if (is_isomorphism(g, h, M)) {
            isos.push_back(M);
        }
        return;
    }

    BinaryMatrix M_prime = M;
    prune(g, h, g_colors, M_prime);
    M_prime.set_row_to_zero(cur_row);

    for (int i = 0; i < n; i++) {
        if (M.get(cur_row, i) == 1 && used_colmns.get(0, i) == 0) {
            M_prime.set(cur_row, i, 1);
            used_colmns.set(0, i, 1);
            parallel_ullman_recurse(used_colmns, cur_row + 1, g, h, g_colors, M_prime, isos);
            M_prime.set(cur_row, i, 0);
            used_colmns.set(0, i, 0);
        }
    }
}

std::vector<BinaryMatrix> parallel_ullman(const SimpleGraph& g, const SimpleGraph& h)
{
    std::vector<BinaryMatrix> isos;

    int n = g.vertex_count();
    int m = h.vertex_count();

    std::vector<std::vector<int>> g_colors = color_graph(g);

    if (n != m) {
        return isos;
    }

    BinaryMatrix M(n, m, 1);
    BinaryMatrix used_colmns(1, n, 0);

#pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (g.degree(i) != h.degree(j)) {
                M.set(i, j, 0);
            }
            int neighborDegreeSumG = 0;
            for (auto ng : g.neighbors(i)) {
                neighborDegreeSumG += g.degree(ng);
            }
            int neighborDegreeSumH = 0;
            for (auto nh : h.neighbors(j)) {
                neighborDegreeSumH += h.degree(nh);
            }
            if (neighborDegreeSumG != neighborDegreeSumH) {
                M.set(i, j, 0);
            }
        }
    }

    parallel_ullman_recurse(used_colmns, 0, g, h, g_colors, M, isos);

    return isos;
}
