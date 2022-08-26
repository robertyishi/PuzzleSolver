#ifndef TERNARY_H
#define TERNARY_H
#include "puzzle.h"

const std::size_t INIT_POS = 0b10010001100;

class TernaryPosition : public Position {
private:
    std::size_t pos;

public:
    TernaryPosition(std::size_t pos = INIT_POS);

    // Position interface
    virtual ~TernaryPosition() override;
    virtual std::size_t hash() const override;
    virtual bool operator ==(const Position &other) const override;
    virtual Position *getCopy() const override;
};

class TernaryMove : public Move {
private:
    bool isRot;

public:
    TernaryMove(bool isRot = true);
    bool isRotate() const;

    // Move interface
    virtual ~TernaryMove() override;
    virtual std::string toString() const override;
};

class Ternary : public Puzzle {
public:
    Ternary();

    // Puzzle interface
    virtual ~Ternary() override;
    virtual Position *getInitialPosition() const override;
    virtual bool isPrimitivePosition(const Position *pos) const override;
    virtual std::vector<Move *> getMoves(const Position *pos) const override;
    virtual Position *doMove(const Position *pos_, const Move *move_) const override;
    virtual Puzzle *getCopy() const override;
};

#endif // TERNARY_H
