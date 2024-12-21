#include <iostream>
#include <omp.h>

#include "parallel_ullman.hpp"

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <graph1_file> <graph2_file>" << std::endl;
        return 1;
    }

    std::string g_file = argv[1];
    std::string h_file = argv[2];

    SimpleGraph g(g_file);
    SimpleGraph h(h_file);

    double start_time = omp_get_wtime();
    std::vector<BinaryMatrix> isos = parallel_ullman(g, h);
    double end_time = omp_get_wtime();
    std::cout << end_time - start_time << std::endl;
}