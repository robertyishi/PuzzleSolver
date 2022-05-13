#include "toh.h"
#include <sstream>

/* class ToHPosition */

ToHPosition::ToHPosition(std::size_t pos) {
    this->pos = pos;
}

std::size_t ToHPosition::getPos() const {
    return this->pos;
}

ToHPosition::~ToHPosition() {}

std::size_t ToHPosition::hash() const {
    return this->pos;
}

bool ToHPosition::operator ==(const Position &other) const {
    const ToHPosition *otherPtr = static_cast<const ToHPosition *>(&other);
    return this->pos == otherPtr->pos;
}

Position *ToHPosition::getCopy() const {
    return new ToHPosition(this->pos);
}

/* class ToHMove */

ToHMove::ToHMove(std::size_t diskIdx, std::size_t rodIdx) {
    this->diskIdx = diskIdx;
    this->rodIdx = rodIdx;
}

ToHMove::~ToHMove() {}

std::size_t ToHMove::getDiskIdx() const {
    return this->diskIdx;
}

std::size_t ToHMove::getRodIdx() const {
    return this->rodIdx;
}

std::string ToHMove::toString() const {
    std::stringstream ss;
    ss << "Move disk " << this->diskIdx << " to rod " << this->rodIdx;
    return ss.str();
}

/* class ToH */

ToH::ToH(std::size_t disks, std::size_t rods) {
    /* Reset number of disks and rods to default values
     * if either is invalid. */
    if (disks < MIN_DISKS || disks > MAX_DISKS ||
            rods < MIN_RODS || rods > MAX_RODS) {
        disks = DEFAULT_DISKS;
        rods = DEFAULT_RODS;
    }
    this->rods = rods;
    this->disks = disks;
}

ToH::~ToH() {}

Position *ToH::getInitialPosition() const {
    size_t pos = 0;
    for (size_t i = 0; i < this->disks; ++i) {
        pos = pos * 10 + 1;
    }
    return new ToHPosition(pos);
}

bool ToH::isPrimitivePosition(const Position *pos_) const {
    const ToHPosition *pos = static_cast<const ToHPosition *>(pos_);
    return pos->getPos() == 0;
}

namespace {
std::size_t tenToThe(std::size_t power) {
    std::size_t res = 1;
    for (size_t i = 0; i < power; ++i) {
        res *= 10;
    }
    return res;
}

/**
 * @brief Returns the index of the smallest disk on rod ROD.
 * Returns ToH::MAX_DISKS if no disks are on ROD.
 */
std::size_t smallestDiskOnRod(const ToHPosition *pos, std::size_t rod) {
    std::size_t diskIdx = 0;
    for (std::size_t posVal = pos->getPos(); posVal > 0; posVal /= 10) {
        if (posVal % 10 == rod) {
            return diskIdx;
        }
        ++diskIdx;
    }
    return ToH::MAX_DISKS;
}

std::size_t rodIdxOf(const ToHPosition *pos, std::size_t diskIdx) {
    std::size_t posVal = pos->getPos();
    std::size_t shift = tenToThe(diskIdx);
    return (posVal / shift) % 10;
}

/**
 * @brief Returns true if MOVE is valid at position POS.
 * A valid Move must satisfy the following two conditions:
 * 1. the disk to move is the smallest on its current rod, and
 * 2. the destination rod must not be holding a disk that is
 * smaller than the disk we are about to move.
 *
 * @param move Move to check for validity.
 * @return True if MOVE is valid, false otherwise.
 */
bool isValidMove(const ToHPosition *pos, const ToHMove *move) {
    std::size_t diskIdx = move->getDiskIdx();
    std::size_t destRod = move->getRodIdx();
    std::size_t currRod = rodIdxOf(pos, diskIdx);
    return smallestDiskOnRod(pos, currRod) == diskIdx &&
            smallestDiskOnRod(pos, destRod) > diskIdx;
}
}

std::vector<Move *> ToH::getMoves(const Position *pos_) const {
    std::vector<Move *> validMoves;
    const ToHPosition *pos = static_cast<const ToHPosition *>(pos_);
    for (std::size_t i = 0; i < this->rods; ++i) {
        std::size_t topDiskIdx = smallestDiskOnRod(pos, i);
        if (topDiskIdx == this->MAX_DISKS) {
            /* Skip current rod if it is empty. */
            continue;
        }
        for (std::size_t j = 0; j < this->rods; ++j) {
            ToHMove *move = new ToHMove(topDiskIdx, j);
            if (isValidMove(pos, move)) {
                validMoves.push_back(move);
            } else {
                delete move;
            }
        }
    }
    return validMoves;
}

Position *ToH::doMove(const Position *pos_, const Move *move_) const {
    const ToHPosition *pos = static_cast<const ToHPosition *>(pos_);
    const ToHMove *move = static_cast<const ToHMove *>(move_);
    if (isValidMove(pos, move)) {
        std::size_t posVal = pos->getPos();
        std::size_t shift = tenToThe(move->getDiskIdx());
        std::size_t oldDigit = (posVal / shift) % 10;
        return new ToHPosition(posVal + (move->getRodIdx() - oldDigit) * shift);
    } else {
        return nullptr;
    }
}

Puzzle *ToH::getCopy() const {
    return new ToH(this->disks, this->rods);
}


