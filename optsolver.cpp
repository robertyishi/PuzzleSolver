#include "optsolver.h"
#include <fstream>
#include <queue>
#include <unordered_set>
#include <cassert>

typedef std::queue<Position *> PositionQueue;
typedef std::vector<Move *> MoveVector;

OptSolver::OptSolver(const Puzzle *puzzle) {
    std::size_t hashSize = puzzle->hashSize();
    this->valid = hashSize > 0;
    this->solved = false;
    this->puzzle = puzzle->getCopy();
    if (this->valid) {
        this->data = new char[hashSize];
        for (std::size_t i = 0; i < hashSize; ++i) {
            this->data[i] = -1;
        }
    }
}

OptSolver::OptSolver(const OptSolver &other) {
    this->valid = other.valid;
    this->solved = other.solved;
    this->puzzle = other.puzzle->getCopy();
    if (other.solved) {
        std::size_t hashSize = this->puzzle->hashSize();
        this->data = new char[hashSize];
        for (std::size_t i = 0; i < hashSize; ++i) {
            this->data[i] = other.data[i];
        }
    }
}

OptSolver::~OptSolver() {
    delete this->puzzle;
    if (this->valid) {
        delete[] this->data;
    }
}

int OptSolver::solve() {
    if (!this->valid) {
        return -1;
    } else if (!this->solved) {
        this->rmt = -1;
        PositionQueue fringe;
        fringe.push(this->puzzle->getInitialPosition());
        char rmt = 0;
        std::size_t remPosInQueue = 1;
        std::size_t numPosNextLevel = 0;
        while (fringe.size()) {
            Position *currPos = fringe.front();
            fringe.pop();
            if (data[currPos->hash()] == -1) {
                data[currPos->hash()] = rmt;
                if (rmt > this->rmt) {
                    this->rmt = rmt;
                }
                /* Expand current possition and enqueue all children positions. */
                MoveVector moves = puzzle->getMoves(currPos);
                numPosNextLevel += moves.size();
                for (Move *move : moves) {
                    Position *nextPos = puzzle->doMove(currPos, move);
                    fringe.push(nextPos);
                    delete move;
                }
            }
            delete currPos;
            if (--remPosInQueue == 0) {
                remPosInQueue = numPosNextLevel;
                numPosNextLevel = 0;
                ++rmt;
            }
        }
        this->solved = true;
    }
    return this->rmt;
}

void OptSolver::saveData(const std::string &filename) const {
    std::ofstream of;
    of.open(filename, std::fstream::out | std::fstream::binary);
    of.write(this->data, this->puzzle->hashSize());
    of.close();
}

void OptSolver::printShortestPathFrom(const Position *pos, std::ostream &outs) {
    this->solve();
    int rmt = this->data[pos->hash()];
    if (rmt == -1) {
        outs << "[NO SOLUTION]" << std::endl;
        return;
    }
    Position *currPos = pos->getCopy();
    Position *nextPos;
    while (rmt) {
        MoveVector validMoves = this->puzzle->getMoves(currPos);
        for (Move *move : validMoves) {
            nextPos = this->puzzle->doMove(currPos, move);
            std::size_t hash = nextPos->hash();
            assert(this->data[hash] != -1);
            int nextRmt = this->data[hash];
            if (nextRmt < rmt) {
                outs << "[rmt " << rmt << ": " << move->toString() << "]->";
                delete currPos;
                currPos = nextPos;
                break;
            } else {
                delete nextPos;
            }
        }
        /* We should have found next move, otherwise there is a bug. */
        for (Move *move : validMoves) {
            delete move;
        }
        --rmt;
    }
    delete currPos;
    outs << "[END]" << std::endl;
}
