#include <cassert>
#include <queue>
#include "solver.h"
#include <unordered_set>
#include <vector>
#define RMT_MAX INT_MAX

typedef std::unordered_map<Position *, std::vector<Position *>, PositionHasher, PositionEqualFn> PositionGraph;
typedef std::unordered_set<Position *, PositionHasher, PositionEqualFn> PositionSet;
typedef std::vector<Position *> PositionVector;
typedef std::queue<Position *> PositionQueue;
typedef std::vector<Move *> MoveVector;
typedef std::unordered_map<Position *, int, PositionHasher, PositionEqualFn> SolverData;

Solver::Solver(Puzzle *puzzle) {
    this->solved = false;
    this->puzzle = puzzle->getCopy();
}

Solver::Solver(const Solver &other) {
    this->solved = other.solved;
    this->puzzle = other.puzzle->getCopy();
    if (other.solved) {
        this->data = other.data;
    }
}

Solver::~Solver() {
    delete this->puzzle;
    for (auto it = this->data.begin(); it != this->data.end(); ++it) {
        delete it->first;
    }
}

namespace {
bool contains(const PositionSet &set, Position *pos) {
    return set.find(pos) != set.end();
}

void addParent(PositionGraph &graph, Position *child, Position *parent) {
    Position *parentCpy = parent->getCopy();
    auto it = graph.find(child);
    if (it == graph.end()) {
        graph.emplace(child->getCopy(), std::vector<Position *>(1, parentCpy));
    } else {
        it->second.push_back(parentCpy);
    }
}

void findPrimitives(const Puzzle *puzzle, PositionVector &primitives,
                    PositionGraph &backwardGraph, SolverData& data) {
    /* Memory handling: If a position was closed when it is visited, deallocate it immediately;
     * otherwise, store the pointer in closed and deallocate when finished. */
    Position *initial_position = puzzle->getInitialPosition();
    PositionQueue fringe;
    PositionSet closed;
    fringe.push(initial_position);

    /* Initialize the root node in the backward graph. We do this extra
     * step so that we do not need to check if root is included in the
     * backward graph in the backward-traversing step. */
    backwardGraph.emplace(initial_position->getCopy(), PositionVector());

    while (fringe.size()) {
        Position *curr_pos = fringe.front();
        fringe.pop();
        if (contains(closed, curr_pos)) {
            /* If current position is closed, deallocate it in memory and
             * continue loop. */
            delete curr_pos;
            continue;
        }
        closed.insert(curr_pos);
        data.emplace(curr_pos->getCopy(), RMT_MAX);
        if (puzzle->isPrimitivePosition(curr_pos)) {
            /* Current position is a primitive and therefore has no children.
             * Add it to the list of primitive positions. */
            primitives.push_back(curr_pos->getCopy());
        } else {
            /* Current position is not a primitive, expand it and
             * enqueue all children positions. */
            MoveVector moves = puzzle->getMoves(curr_pos);
            for (Move *move : moves) {
                Position *next_pos = puzzle->doMove(curr_pos, move);
                fringe.push(next_pos);
                addParent(backwardGraph, next_pos, curr_pos);
                delete move;
            }
        }
    }
    /* Deallocate all position pointers in closed set. */
    for (Position *pos : closed) {
        delete pos;
    }
}

void updateRemoteness(SolverData &data, Position *pos, int rmt) {
    assert(data.find(pos) != data.end());
    data[pos] = std::min(data[pos], rmt);
}

void updateRemotenessFrom(SolverData &data, PositionGraph& backwardGraph, Position *primitive) {
    /* Similar logic as in findPrimitives(). */
    PositionQueue fringe;
    PositionSet closed;
    fringe.push(primitive);
    int rmt = 0;
    size_t rem = 1;
    size_t numPosNextLevel = 0;

    while (fringe.size()) {
        Position *curr_pos = fringe.front();
        fringe.pop();
        if (contains(closed, curr_pos)) {
            delete curr_pos;
        } else {
            closed.insert(curr_pos);
            /* Update remoteness of current position. */
            updateRemoteness(data, curr_pos, rmt);
            for (Position *next_pos : backwardGraph[curr_pos]) {
                fringe.push(next_pos->getCopy());
                ++numPosNextLevel;
            }
        }
        if (--rem == 0) {
            rem = numPosNextLevel;
            numPosNextLevel = 0;
            ++rmt;
        }
    }
    for (Position *pos : closed) {
        delete pos;
    }
}

// TODO: Parallelize
void calcRemoteness(SolverData &data, PositionGraph& backwardGraph, const PositionVector &primitives) {
    for (Position *pos : primitives) {
        updateRemotenessFrom(data, backwardGraph, pos);
    }
}

void deallocatePositionVector(const PositionVector &v) {
    for (Position *pos : v) {
        delete pos;
    }
}

void deallocatePositionGraph(PositionGraph &graph) {
    for (auto it = graph.begin(); it != graph.end(); ++it) {
        deallocatePositionVector(it->second);
        delete it->first;
    }
}
}

int Solver::solve() {
    if (!this->solved) {
        /* Backward graph. */
        PositionGraph backwardGraph;
        /* Vector of all primitive states. */
        PositionVector primitives;

        /* Step 1: Run BFS from initial position to find all primitive states,
         * constructing the backward graph and initializing solver data in
         * the meantime. */
        findPrimitives(this->puzzle, primitives, backwardGraph, this->data);

        /* Step 2: Run BSF from every primitive state, find remoteness of each
         * position to each primitive state, and take the minimum as the actual
         * remoteness of the position. */
        calcRemoteness(this->data, backwardGraph, primitives);

        /* Step 3: Deallocate backwardGraph. No need to deallocated primitives
         * as they are already deallocated in Step 2. */
        deallocatePositionGraph(backwardGraph);

        this->solved = true;
    }
    /* Retrieve remotenes of the initial position. */
    Position *initPos = this->puzzle->getInitialPosition();
    int rmt = this->data.find(initPos)->second;
    delete initPos;
    return rmt;
}

void Solver::printShortestPath(std::ostream &outs) {
    int rmt = solve();
    if (rmt == RMT_MAX) {
        outs << "[NO SOLUTIONS]" << std::endl;
        return;
    }

    Position *currPos = this->puzzle->getInitialPosition();
    Position *nextPos;
    while (rmt) {
        MoveVector validMoves = this->puzzle->getMoves(currPos);
        for (Move *move : validMoves) {
            nextPos = this->puzzle->doMove(currPos, move);
            assert(this->data.find(nextPos) != this->data.end());
            int nextRmt = this->data[nextPos];
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



















