#include "lightsout.h"
#include <iostream>
#include <sstream>

LightsOutPosition::LightsOutPosition(std::size_t pos) {
    this->pos = pos;
}

std::size_t LightsOutPosition::getPos() const {
    return this->pos;
}

LightsOutPosition::~LightsOutPosition() {}

std::size_t LightsOutPosition::hash() const {
    return this->pos;
}

bool LightsOutPosition::operator ==(const Position &other) const {
    const LightsOutPosition *otherPtr = static_cast<const LightsOutPosition *>(&other);
    return this->pos == otherPtr->pos;
}

Position *LightsOutPosition::getCopy() const {
    return new LightsOutPosition(this->pos);
}

LightsOutMove::LightsOutMove(std::size_t i, std::size_t j) {
    this->i = i;
    this->j = j;
}

std::size_t LightsOutMove::get_i() const {
    return this->i;
}

std::size_t LightsOutMove::get_j() const {
    return this->j;
}

LightsOutMove::~LightsOutMove() {}

std::string LightsOutMove::toString() const {
    std::stringstream ss;
    ss << '(' << this->i << ", " << this->j << ')';
    return ss.str();
}

LightsOut::LightsOut(std::size_t rows, std::size_t cols) {
    if (rows == 0 || cols == 0) {
        std::cout << "Grid cannot be empty. Falling back on default values.\n";
        rows = cols = 3;
    }
    if (rows > 64 || cols > 64 || rows * cols > 64) {
        std::cout << "Grid dimensions cannot exceed 64 cells in total."
                     " Falling back on default values.\n";
        rows = cols = 3;
    }
    this->rows = rows;
    this->cols = cols;
}

LightsOut::~LightsOut() {}

Position *LightsOut::getInitialPosition() const {
    return new LightsOutPosition;
}

bool LightsOut::isPrimitivePosition(const Position *pos_) const {
    const LightsOutPosition *pos = static_cast<const LightsOutPosition *>(pos_);
    return pos->hash() == 0;
}

std::vector<Move *> LightsOut::getMoves(const Position *pos) const {
    (void)pos; // Unused.
    std::vector<Move *> moves;
    for (std::size_t i = 0; i < this->rows; ++i) {
        for (std::size_t j = 0; j < this->cols; ++j) {
            moves.push_back(new LightsOutMove(i, j));
        }
    }
    return moves;
}

Position *LightsOut::doMove(const Position *pos_, const Move *move_) const {
    const LightsOutPosition *pos = static_cast<const LightsOutPosition *>(pos_);
    const LightsOutMove *move = static_cast<const LightsOutMove *>(move_);
    std::size_t i = move->get_i();
    std::size_t j = move->get_j();
    std::size_t newPos = pos->getPos();
    if (i >= this->rows || j >= this->cols) {
        /* Invalid move. */
        return new LightsOutPosition(newPos);
    }
    /* Toggle light at center cell. */
    newPos ^= std::size_t(1) << (i * this->cols + j);
    /* Toggle left cell if possible. */
    newPos ^= (std::size_t(1) << (i * this->cols + j - 1)) * (j > 0);
    /* Toggle right cell if possible. */
    newPos ^= (std::size_t(1) << (i * this->cols + j + 1)) * (j < this->cols - 1);
    /* Toggle top cell if possible. */
    newPos ^= (std::size_t(1) << ((i - 1) * this->cols + j)) * (i > 0);
    /* Toggle bottom cell if possible. */
    newPos ^= (std::size_t(1) << ((i + 1) * this->cols + j)) * (i < this->rows - 1);
    return new LightsOutPosition(newPos);
}

Puzzle *LightsOut::getCopy() const {
    return new LightsOut(this->rows, this->cols);
}

std::size_t LightsOut::hashSize() const {
    return std::size_t(1) << (this->rows * this->cols);
}
