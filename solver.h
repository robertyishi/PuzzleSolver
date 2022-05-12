#ifndef SOLVER_H
#define SOLVER_H
#include "puzzle.h"
#include <unordered_map>

class Solver {
private:
    bool solved;
    Puzzle *puzzle;
    std::unordered_map<Position *, int, PositionHasher, PositionEqualFn> data;

public:
    Solver(Puzzle *puzzle = nullptr);
    Solver(const Solver &other);
    ~Solver();

    int solve(const Puzzle *puzzle);

};

#endif // SOLVER_H
