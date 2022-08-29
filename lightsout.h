#ifndef LIGHTSOUT_H
#define LIGHTSOUT_H
#include "puzzle.h"

class LightsOutPosition : public Position {
private:
    std::size_t pos;

public:
    LightsOutPosition(std::size_t pos = 0);
    std::size_t getPos() const;

    // Position interface
    virtual ~LightsOutPosition() override;
    virtual std::size_t hash() const override;
    virtual bool operator ==(const Position &other) const override;
    virtual Position *getCopy() const override;
};

class LightsOutMove : public Move {
private:
    std::size_t i;
    std::size_t j;

public:
    LightsOutMove(std::size_t i = 0, std::size_t j = 0);
    std::size_t get_i() const;
    std::size_t get_j() const;

    // Move interface
    virtual ~LightsOutMove() override;
    virtual std::string toString() const override;
};

class LightsOut : public Puzzle {
private:
    std::size_t rows;
    std::size_t cols;

public:
    LightsOut(std::size_t rows = 3, std::size_t cols = 3);

    // Puzzle interface
    virtual ~LightsOut() override;
    virtual Position *getInitialPosition() const override;
    virtual bool isPrimitivePosition(const Position *pos_) const override;
    virtual std::vector<Move *> getMoves(const Position *pos) const override;
    virtual Position *doMove(const Position *pos_, const Move *move_) const override;
    virtual Puzzle *getCopy() const override;
    virtual std::size_t hashSize() const override;
};

#endif // LIGHTSOUT_H
