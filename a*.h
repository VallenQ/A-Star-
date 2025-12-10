#ifndef A_STAR_H
#define A_STAR_H

#include <vector>
#include <string>

struct CellPos { int i, j; };

std::vector<std::vector<char>> parse_file_to_grid(
    const std::string &filename, 
    CellPos &start, 
    CellPos &goal
);

std::vector<std::vector<char>> a_star_algorithm(
    const std::vector<std::vector<char>> &grid,
    const CellPos &start,
    const CellPos &goal
);

#endif
