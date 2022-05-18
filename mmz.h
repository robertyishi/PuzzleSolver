#ifndef MMZ_H
#define MMZ_H
#include "puzzle.h"

/**
 * @brief Defines a position of a Mummy Maze game.
 */
class MMzPosition : public Position {
private:
    std::uint64_t pos;

public:
    MMzPosition(std::uint64_t pos = 0);
    virtual ~MMzPosition() override;

    std::uint64_t getPos() const;

    // Position interface
    virtual std::size_t hash() const override;
    virtual bool operator ==(const Position &other_) const override;
    virtual Position *getCopy() const override;
};

class MMzMove : public Move {
public:
    enum PossibleMoves {WAIT, UP, LEFT, DOWN, RIGHT};
    const static int NUM_POSSIBLE_MOVES = 5;

private:
    int direction;

public:
    MMzMove(int move = WAIT);
    virtual ~MMzMove() override;

    int getDirection() const;

    // Move interface
    virtual std::string toString() const override;
};

class MMz : public Puzzle {
private:
    bool initialized;
    std::size_t rows;
    std::size_t cols;
    std::size_t worldRows;
    std::size_t worldCols;
    std::string world;
    std::uint64_t initPos;

public:
    MMz();                              // Construct uninitialized maze.
    MMz(const std::string &fileName);   // Construct from file.
    virtual ~MMz() override;

    bool readFromFile(const std::string &fileName);
    std::string asString(const MMzPosition* mmzPos) const;

    // Puzzle interface
    virtual Position *getInitialPosition() const override;
    virtual bool isPrimitivePosition(const Position *pos_) const override;
    virtual std::vector<Move *> getMoves(const Position *pos_) const override;
    virtual Position *doMove(const Position *pos_, const Move *move_) const override;
    virtual Puzzle *getCopy() const override;

private:
    std::uint64_t getDestLoc(std::uint64_t pos, std::size_t chrIdx, int direction) const;
    bool isValidMove(const MMzPosition *mmzPos, const MMzMove *move) const;

    std::uint64_t moveNPC(uint64_t &pos, std::uint64_t chrIdx, bool &gateToggled) const;
    bool moveNPCs(std::uint64_t &pos, bool walking, bool &gateToggled) const;
};

#endif // MMZ_H
