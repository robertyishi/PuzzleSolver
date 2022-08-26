#ifndef TOH_H
#define TOH_H
#include "puzzle.h"

/**
 * @brief Position of a Towers of Hanoi game.
 *
 * For a game of M rods and N disks:
 *
 * Rods are labeled from 0 to M - 1, where the destination
 * rod is labeled 0, and all other rods are labeled from
 * 1 to M - 1.
 *
 * Disks are labeled from 0 to N - 1 in increasing size
 * order. That is, the smallest disk is labeled 0, and the
 * largest disk is labeled N - 1.
 *
 * Each possible position is uniquely represented by a N-
 * digit integer, where the i-th least significant digit
 * represents the index of the rod where disk i resides.
 * For example, M = 3 and N = 3, the following position
 *    |   |   |
 *    |   |   1
 *    0   |   2
 * ---------------
 * is represented by the integer 001.
 *
 * Limits: 1 <= M <= 10, 1 <= N <= 19.
 * M cannot be greater than 10 because we only have 10 digits
 * 0 through 9 for each rod.
 * N cannot be greater than 19 because we use 64-bit unsigned
 * intergers to represent positions.
 */
class ToHPosition : public Position {
private:
    std::size_t pos;

public:
    ToHPosition(std::size_t pos = 0);

    std::size_t getPos() const;

    // Position interface
    virtual ~ToHPosition() override;
    virtual std::size_t hash() const override;
    virtual bool operator ==(const Position &other) const override;
    virtual Position *getCopy() const override;
};

/**
 * @brief Defines a move in a Towers of Hanoi game.
 * Moves disk with index RODIDX to rod with index
 * RODIDX.
 */
class ToHMove : public Move {
private:
    std::size_t diskIdx;
    std::size_t rodIdx;

public:
    ToHMove(std::size_t diskIdx = 0, std::size_t rodIdx = 0);
    virtual ~ToHMove() override;

    std::size_t getDiskIdx() const;
    std::size_t getRodIdx() const;

    // Move interface
    virtual std::string toString() const override;
};

/**
 * @brief A Towers of Hanoi game instance.
 */
class ToH : public Puzzle {
public:
    const static std::size_t MIN_RODS = 1;
    const static std::size_t MIN_DISKS = 1;
    const static std::size_t MAX_RODS = 10;
    const static std::size_t MAX_DISKS = 19;
    const static std::size_t DEFAULT_RODS = 3;
    const static std::size_t DEFAULT_DISKS = 3;

private:
    std::size_t rods;
    std::size_t disks;

public:
    ToH(std::size_t disks = DEFAULT_DISKS, std::size_t rods = DEFAULT_RODS);

    // Puzzle interface
    virtual ~ToH() override;
    virtual Position *getInitialPosition() const override;
    virtual bool isPrimitivePosition(const Position *pos_) const override;
    virtual std::vector<Move *> getMoves(const Position *pos_) const override;
    virtual Position *doMove(const Position *pos_, const Move *move_) const override;
    virtual Puzzle *getCopy() const override;
};

#endif // TOH_H
