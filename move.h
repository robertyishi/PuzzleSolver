#ifndef MOVE_H
#define MOVE_H
#include <string>

class Move {
public:
    Move();
    virtual ~Move() = 0;

    virtual std::string toString() const = 0;
};

#endif // MOVE_H
