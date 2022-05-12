#ifndef POSITION_H
#define POSITION_H
#include <cstdlib>

class Position {
public:
    Position();
    virtual ~Position() = 0;

    virtual std::size_t hash() const = 0;
    virtual bool operator==(const Position &other) const = 0;

    virtual Position *getCopy() const = 0;
};

class PositionHasher {
public:
    std::size_t operator()(const Position *pos) const {
        return pos->hash();
    }
};

class PositionEqualFn {
public:
    bool operator()(const Position *lhs, const Position *rhs) const {
        return *lhs == *rhs;
    }
};

#endif // POSITION_H
