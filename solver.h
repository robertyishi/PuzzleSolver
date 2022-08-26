#ifndef SOLVER_H
#define SOLVER_H
#include "puzzle.h"
#include <iostream>
#include <mutex>
#include <unordered_map>

class Solver {
private:
    bool solved;
    Puzzle *puzzle;
    std::unordered_map<Position *, int, PositionHasher, PositionEqualFn> data;
    std::mutex dataLock;

public:
    Solver(const Puzzle *puzzle = nullptr);
    Solver(const Solver &other);
    ~Solver();

    int solve();
    void printShortestPath(std::ostream &outs);
    void printInfo(std::ostream &outs) const;
};

#endif // SOLVER_H
