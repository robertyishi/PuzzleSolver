#ifndef OPTSOLVER_H
#define OPTSOLVER_H
#include "puzzle.h"

class OptSolver {
private:
    bool valid;
    bool solved;
    Puzzle *puzzle;
    char *data;
    int rmt;

public:
    OptSolver(const Puzzle *puzzle = nullptr);
    OptSolver(const OptSolver &other);
    ~OptSolver();

    int solve();
    void saveData(const std::string &filename) const;
    void printShortestPathFrom(const Position *pos, std::ostream &outs);
};

#endif // OPTSOLVER_H
