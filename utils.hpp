#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>

class BinaryMatrix {
private:
    size_t rows, cols, words_per_row;
    std::vector<uint64_t> data;

    inline size_t index(size_t row, size_t col) const {
        return row * words_per_row + col / 64;
    }

    inline uint64_t mask(size_t col) const {
        return 1ULL << (col % 64);
    }

public:
    BinaryMatrix(size_t rows, size_t cols, bool value = 0)
        : rows(rows), cols(cols), words_per_row((cols + 63) / 64), data(rows * words_per_row, value ? ~0ULL : 0ULL) {}

    BinaryMatrix(const BinaryMatrix& other)
        : rows(other.rows), cols(other.cols), words_per_row(other.words_per_row), data(other.data) {}

    BinaryMatrix& operator=(const BinaryMatrix& other) {
        if (this != &other) {
            rows = other.rows;
            cols = other.cols;
            words_per_row = other.words_per_row;
            data = other.data;
        }
        return *this;
    }

    bool get(size_t row, size_t col) const {
        return (data[index(row, col)] & mask(col)) != 0;
    }

    void set(size_t row, size_t col, bool value) {
        if (value) {
            data[index(row, col)] |= mask(col);
        } else {
            data[index(row, col)] &= ~mask(col);
        }
    }


    void set_row_to_zero(size_t row) {
        size_t start = row * words_per_row;
        std::memset(&data[start], 0, words_per_row * sizeof(uint64_t));
    }
};

class SimpleGraph {
public:
    SimpleGraph(int vertices)
        : adj_list_(vertices)
    {
    }

    SimpleGraph(const std::string& filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        readGraph(in);
    }

    void add_edge(int src, int dest)
    {
        adj_list_[src].push_back(dest);
    }

    bool has_edge(int src, int dest) const
    {
        const auto& edges = adj_list_[src];
        return std::find(edges.begin(), edges.end(), dest) != edges.end();
    }

    const std::vector<int>& neighbors(int vertex) const
    {
        return adj_list_[vertex];
    }

    int degree(int vertex) const
    {
        return adj_list_[vertex].size();
    }

    int vertex_count() const
    {
        return adj_list_.size();
    }

    void print() const
    {
        for (int i = 0; i < vertex_count(); ++i) {
            std::cout << i << ": ";
            for (int neighbor : neighbors(i)) {
                std::cout << neighbor << ' ';
            }
            std::cout << '\n';
        }
    }

private:
    std::vector<std::vector<int>> adj_list_;

    unsigned short readWord(std::ifstream &in)
    {
        unsigned char b1, b2;
        in.read(reinterpret_cast<char*>(&b1), 1);
        in.read(reinterpret_cast<char*>(&b2), 1);
        return b1 | (b2 << 8);
    }

    
    void readGraph(std::ifstream &in)
    {
        int nodes = readWord(in);
        adj_list_.resize(nodes);

        for (int i = 0; i < nodes; ++i) {
            int edges = readWord(in);

            for (int j = 0; j < edges; ++j) {
                int target = readWord(in);
                add_edge(i, target);
            }
        }
    }
};

#endif // UTILS_HPP
