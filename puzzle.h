#ifndef PUZZLE_H
#define PUZZLE_H
#include "move.h"
#include "position.h"
#include <vector>

class Puzzle {
public:
    Puzzle();
    virtual ~Puzzle() = 0;

    virtual Position *getInitialPosition() const = 0;
    virtual bool isPrimitivePosition(const Position *pos) const = 0;
    virtual std::vector<Move *> getMoves(const Position *pos) const = 0;
    virtual Position *doMove(const Position *pos, const Move *move) const = 0;
    virtual Puzzle *getCopy() const = 0;
    virtual std::size_t hashSize() const = 0;
};

#endif // PUZZLE_H
