#include "ternary.h"

TernaryPosition::TernaryPosition(std::size_t pos) {
    this->pos = pos;
}

TernaryPosition::~TernaryPosition() {}

std::size_t TernaryPosition::hash() const {
    return this->pos;
}

bool TernaryPosition::operator ==(const Position &other) const {
    const TernaryPosition *otherPtr = static_cast<const TernaryPosition *>(&other);
    return this->pos == otherPtr->pos;
}

Position *TernaryPosition::getCopy() const {
    return new TernaryPosition(this->pos);
}

TernaryMove::TernaryMove(bool isRot) {
    this->isRot = isRot;
}

bool TernaryMove::isRotate() const {
    return this->isRot;
}

TernaryMove::~TernaryMove() {}

std::string TernaryMove::toString() const {
    if (this->isRot) {
        return "ROTATE";
    }
    return "SPIN";
}

Ternary::Ternary() {}

Ternary::~Ternary(){}

Position *Ternary::getInitialPosition() const {
    return new TernaryPosition;
}

bool Ternary::isPrimitivePosition(const Position *pos) const {
    return pos->hash() == INIT_POS;
}

std::vector<Move *> Ternary::getMoves(const Position *pos) const {
    (void)pos; // unused POS parameter.
    return std::vector<Move *>({new TernaryMove(true), new TernaryMove(false)});
}

Position *Ternary::doMove(const Position *pos_, const Move *move_) const {
    const TernaryPosition *pos = static_cast<const TernaryPosition *>(pos_);
    const TernaryMove *move = static_cast<const TernaryMove *>(move_);
    std::size_t val = pos->hash();
    if (move->isRotate()) {
        val <<= 4;
        val |= (val >> 16);
        val &= ~(0b1111 << 16);
    } else {
        /* Spin */
        for (int i = 0; i < 3; ++i) {
            std::size_t num = (val & (0b11 << (i << 2))) >> (i << 2);
            num = (num + 1) % 3;
            val &= ~(0b11 << (i << 2));
            val |= num << (i << 2);
        }
    }
    return new TernaryPosition(val);
}

Puzzle *Ternary::getCopy() const {
    return new Ternary;
}













