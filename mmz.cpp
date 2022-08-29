#include "mmz.h"
#include <cassert>
#include <fstream>
#include <limits>
#include <sstream>

namespace {
const std::uint64_t ONE = 1;
const std::uint64_t MAX_NPCS = 4;
const std::uint64_t PLAYER_IDX = MAX_NPCS;
const std::uint64_t PLAYER_STRENGTH = 4;
const std::uint64_t INVALID_LOC = std::numeric_limits<uint64_t>::max();

/*
 * Structure of an NPC info:
 * +---------+-------+-------+----------------------+
 * | Walking | Color | Alive |       Position       |
 * +---------+-------+-------+----------------------+
 *      12       11     10    9                     0
 *
 * Structure of player info:
 * +-------+----------------------+
 * | Alive |       Position       |
 * +-------+----------------------+
 *    10    9                     0
 *
 * Structure of a position:
 * +------+-------------+--------------+--------------+--------------+--------------+
 * | Gate | Player info |  NPC 3 info  |  NPC 2 info  |  NPC 1 info  |  NPC 0 info  |
 * +------+-------------+--------------+--------------+--------------+--------------+
 *    63   62         52 51          39 38          26 25          13 12           0
 */
const std::size_t CHARACTER_LOC_LENGTH     = 10;
const std::size_t CHARACTER_ALIVE_SHIFT    = CHARACTER_LOC_LENGTH;
const std::size_t CHARACTER_COLOR_SHIFT    = CHARACTER_ALIVE_SHIFT + 1;
const std::size_t CHARACTER_STRENGTH_SHIFT = CHARACTER_COLOR_SHIFT;
const std::size_t CHARACTER_WALKING_SHIFT  = CHARACTER_COLOR_SHIFT + 1;
const std::size_t CHARACTER_INFO_LENGTH    = CHARACTER_WALKING_SHIFT + 1;
const std::size_t PLAYER_INFO_LENGTH       = CHARACTER_ALIVE_SHIFT + 1;

/* Only position and alive masks are valid for player. */
const std::uint64_t CHARACTER_LOC_MASK      = (ONE << CHARACTER_LOC_LENGTH) - 1;
const std::uint64_t CHARACTER_ALIVE_MASK    = ONE << CHARACTER_ALIVE_SHIFT;
const std::uint64_t CHARACTER_COLOR_MASK    = ONE << CHARACTER_COLOR_SHIFT;
const std::uint64_t CHARACTER_WALKING_MASK  = ONE << CHARACTER_WALKING_SHIFT;
const std::uint64_t CHARACTER_STRENGTH_MASK = CHARACTER_COLOR_MASK | CHARACTER_WALKING_MASK;

const std::size_t   GATE_SHIFT = MAX_NPCS * CHARACTER_INFO_LENGTH + PLAYER_INFO_LENGTH;
const std::uint64_t GATE_MASK  = ONE << GATE_SHIFT;

const char EMPTY         = '_';
const char WALL          = 'W';
const char GATE          = 'G';
const char UNLOCKED_GATE = 'U';
const char KEY           = 'K';
const char TRAP          = 'T';
const char EXIT          = 'E';
const char WHITE_SCP     = '0';  // converts to 0b00
const char RED_SCP       = '1';  // converts to 0b01
const char WHITE_MMY     = '2';  // converts to 0b10
const char RED_MMY       = '3';  // converts to 0b11
const char PLAYER        = '4';

/* Helper Functions */
inline void setBit(std::uint64_t &number, std::size_t n, bool x);
inline std::size_t toWorldDim(std::size_t gridDim);
inline std::size_t toGridDim(std::size_t worldDim);
std::size_t toWorldLoc(std::uint64_t gridLoc, std::size_t gridCols);
std::size_t toGridLoc(std::uint64_t worldLoc, std::size_t worldCols);
inline bool isChr(char c);
inline bool isNPC(char c);
inline bool isKey(char c);
inline bool isGate(char c);
inline bool isExit(char c);
inline bool isTrap(char c);

inline void chrSetAlive(std::uint64_t &pos, std::uint64_t chrIdx);
inline void chrSetLoc(std::uint64_t &pos, std::uint64_t loc, std::uint64_t chrIdx);
inline void chrSetStrength(std::uint64_t &pos, std::uint64_t strength, std::uint64_t chrIdx);
bool addChr(std::uint64_t &pos, char chr, std::uint64_t loc);
inline void killChr(std::uint64_t &pos, std::size_t chrIdx);
inline void killPlayer(std::uint64_t &pos);
bool collect(std::uint64_t &pos);
void setGate(std::uint64_t &pos, char gate);
void setGate(std::uint64_t &pos, bool closed);
void toggleGate(std::uint64_t &pos);

uint64_t moveChr(std::uint64_t &pos, std::size_t chrIdx, int direction, std::size_t gridCols);
std::uint64_t movePlayer(std::uint64_t &pos, int direction, std::size_t gridCols);

inline bool gateIsClosed(std::uint64_t pos);
inline std::uint64_t chrLoc(std::uint64_t pos, std::size_t chrIdx);
inline bool chrIsAlive(std::uint64_t pos, std::size_t chrIdx);
inline bool chrIsWalking(std::uint64_t pos, std::size_t chrIdx);
inline bool chrIsRed(std::uint64_t pos, std::size_t chrIdx);
inline std::uint64_t chrStrength(std::uint64_t pos, std::size_t chrIdx);
inline std::uint64_t playerLoc(std::uint64_t pos);
inline bool playerIsAlive(std::uint64_t pos);

void getOffsets(int direction, int &i_ofs, int &j_ofs);
} // Anonymous Namespace

/* class MMzPosition */

inline MMzPosition::MMzPosition(std::uint64_t pos) {
    this->pos = pos;
}

inline MMzPosition::~MMzPosition() {}

inline uint64_t MMzPosition::getPos() const {
    return this->pos;
}

inline std::size_t MMzPosition::hash() const {
    assert(sizeof(std::size_t) == sizeof(std::uint64_t));
    return this->pos;
}

inline bool MMzPosition::operator ==(const Position &other_) const {
    const MMzPosition *other = static_cast<const MMzPosition *>(&other_);
    return this->pos == other->pos;
}

inline Position *MMzPosition::getCopy() const {
    return new MMzPosition(this->pos);
}

/* class MMzMove */

/**
 * @brief Constructs a move given MOVE index.
 * MOVE should be one of the possible moves defined
 * in the MMzMove::PossibleMoves enum. Constructs
 * a default WAIT move if MOVE is out of range.
 * @param move Index of a move.
 */
MMzMove::MMzMove(int move) {
    if (move < WAIT || move > RIGHT) {
        this->direction = WAIT;
    } else {
        this->direction = move;
    }
}

MMzMove::~MMzMove() {}

inline int MMzMove::getDirection() const {
    return this->direction;
}

std::string MMzMove::toString() const {
    switch (this->direction) {
    case WAIT:
        return "WAIT";
    case UP:
        return "UP";
    case LEFT:
        return "LEFT";
    case DOWN:
        return "DOWN";
    case RIGHT:
        return "RIGHT";
    default:
        return "UNKNOWN MOVE";
    }
}

/* class MMz */

inline MMz::MMz() {
    this->initialized = false;
}

MMz::MMz(const std::string &fileName) {
    readFromFile(fileName);
}

inline MMz::~MMz() {}

bool MMz::readFromFile(const std::string &fileName) {
    this->initialized = false;
    std::ifstream fin;
    fin.open(fileName);
    if (!fin.is_open()) {
        return false;
    }
    fin >> this->rows;
    fin >> this->cols;
    this->worldRows = toWorldDim(this->rows);
    this->worldCols = toWorldDim(this->cols);
    world.resize(worldRows * worldCols);
    this->initPos = 0;
    fin.get();
    for (std::size_t i = 0; i < this->worldRows; ++i) {
        for (std::size_t j = 0; j < this->worldCols; ++j) {
            std::size_t loc = i * this->worldCols + j;
            fin.get(world[loc]);
            if (isChr(world[loc])) {
                addChr(this->initPos, world[loc], toGridLoc(loc, this->worldCols));
                world[loc] = EMPTY;
            } else if (isGate(world[loc])) {
                setGate(this->initPos, world[loc]);
                world[loc] = GATE;
            }
        }
        fin.get();
    }
    fin.close();
    this->initialized = true;
    return true;
}

std::string MMz::asString(const MMzPosition *mmzPos) const {
    std::stringstream ss;
    std::size_t walker = 0;
    for (std::size_t i = 0; i < this->worldRows; ++i) {
        for (std::size_t j = 0; j < this->worldCols; ++j) {
            bool replaced = false;
            if (mmzPos && (i % 2) && (j % 2)) {
                std::uint64_t pos = mmzPos->getPos();
                for (std::uint64_t chrIdx = 0; chrIdx < PLAYER_IDX + 1; ++chrIdx) {
                    if (chrIsAlive(pos, chrIdx) && chrLoc(pos, chrIdx) == toGridLoc(walker, this->worldCols)) {
                        if (chrIdx == PLAYER_IDX) {
                            ss.put(PLAYER);
                        }
                        else {
                            ss.put('0' + chrStrength(pos, chrIdx));
                        }
                        replaced = true;
                        break;
                    }
                }
            }
            if (!replaced) {
                ss.put(world[walker]);
            }
            walker++;
        }
        ss.put('\n');
    }
    ss << std::endl;
    return ss.str();
}

inline Position *MMz::getInitialPosition() const {
    return new MMzPosition(this->initPos);
}

bool MMz::isPrimitivePosition(const Position *pos_) const {
    const MMzPosition *mmzPos = static_cast<const MMzPosition *>(pos_);
    std::uint64_t pos = mmzPos->getPos();
    std::uint64_t ploc = playerLoc(pos);
    char unit = this->world[toWorldLoc(ploc, this->cols)];
    return playerIsAlive(pos) && isExit(unit);
}

std::vector<Move *> MMz::getMoves(const Position *pos_) const {
    const MMzPosition *pos = static_cast<const MMzPosition *>(pos_);
    std::vector<Move *> validMoves;
    MMzMove *move;
    for (int i = 0; i < MMzMove::NUM_POSSIBLE_MOVES; ++i) {
        move = new MMzMove(i);
        if (isValidMove(pos, move)) {
            validMoves.push_back(move);
        } else {
            delete move;
        }
    }
    return validMoves;
}

Position *MMz::doMove(const Position *pos_, const Move *move_) const {
    const MMzPosition *mmzPos = static_cast<const MMzPosition *>(pos_);
    const MMzMove *move = static_cast<const MMzMove *>(move_);
    if (!isValidMove(mmzPos, move)) {
        /* Not a valid move. */
        return nullptr;
    }
    std::uint64_t pos = mmzPos->getPos();
    std::uint64_t ploc = playerLoc(pos);
    std::uint64_t newPloc = movePlayer(pos, move->getDirection(), this->cols);
    /* If player steps on a trap, kill player. */
    if (isTrap(this->world[toWorldLoc(newPloc, this->cols)])) {
        killPlayer(pos);
        return new MMzPosition(pos);
    } else if (newPloc != ploc && isKey(this->world[toWorldLoc(newPloc, this->cols)])) {
        toggleGate(pos);
    }
    /* Handle NPCs. */
    bool gateToggled;
    for (int i = 0; i < 3; ++i) {
        if (moveNPCs(pos, i != 0, gateToggled)) {
            return new MMzPosition(pos);
        } else if (gateToggled) {
            toggleGate(pos);
        }
    }
    return new MMzPosition(pos);
}

Puzzle *MMz::getCopy() const {
    return new MMz(*this);
}

std::uint64_t MMz::getDestLoc(std::uint64_t pos, std::size_t chrIdx, int direction) const {
    int i_ofs, j_ofs;
    getOffsets(direction, i_ofs, j_ofs);
    std::uint64_t loc = chrLoc(pos, chrIdx);
    std::uint64_t worldLoc = toWorldLoc(loc, this->cols);
    std::uint64_t wallWorldLoc = worldLoc + i_ofs * this->worldCols + j_ofs;
    /* Destination is not reachable if wall or closed gate is blocking the way. */
    if (this->world[wallWorldLoc] == WALL) {
        return INVALID_LOC;
    } else if (this->world[wallWorldLoc] == GATE && gateIsClosed(pos)) {
        return INVALID_LOC;
    }
    return wallWorldLoc + i_ofs * this->worldCols + j_ofs;
}

/**
 * @brief Returns true if MOVE is valid at POS.
 * A player move is valid if and only if there is
 * no wall blocking the path and there is no NPC
 * at destination.
 */
bool MMz::isValidMove(const MMzPosition *mmzPos, const MMzMove *move) const {
    std::uint64_t pos = mmzPos->getPos();
    int dir = move->getDirection();
    if (!playerIsAlive(pos)) {
        /* No moves are available if player is dead. */
        return false;
    }
    std::uint64_t destWorldLoc = getDestLoc(pos, PLAYER_IDX, dir);
    if (destWorldLoc == INVALID_LOC) {
        return false;
    }
    return !isNPC(this->world[destWorldLoc]);
}

std::uint64_t MMz::moveNPC(std::uint64_t &pos, std::uint64_t chrIdx, bool &gateToggled) const {
    std::uint64_t ploc = playerLoc(pos);
    std::uint64_t nloc = chrLoc(pos, chrIdx);
    std::uint64_t newNloc = nloc;
    std::uint64_t p_i = ploc / this->cols;
    std::uint64_t p_j = ploc % this->cols;
    std::uint64_t n_i = nloc / this->cols;
    std::uint64_t n_j = nloc % this->cols;
    std::uint64_t destWorldLoc;
    /* At most one of the following 8 branches will get executed.
     * No branch is entered if and only if no move is valid. */
    if (chrIsRed(pos, chrIdx)) {
        /* Prioritizes vertical moves. */
        if (n_i > p_i && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::UP)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        } else if (n_i < p_i && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::DOWN)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        } else if (n_j > p_j && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::LEFT)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        } else if (n_j < p_j && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::RIGHT)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        }
    } else {
        /* Prioritizes horizontal moves. */
        if (n_j > p_j && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::LEFT)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        } else if (n_j < p_j && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::RIGHT)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        } else if (n_i > p_i && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::UP)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        } else if (n_i < p_i && (destWorldLoc = getDestLoc(pos, chrIdx, MMzMove::DOWN)) != INVALID_LOC) {
            newNloc = toGridLoc(destWorldLoc, this->worldCols);
        }
    }
    chrSetLoc(pos, newNloc, chrIdx);
    if (newNloc != nloc && isKey(this->world[toWorldLoc(newNloc, this->cols)])) {
        gateToggled = true;
    }
    return 0;
}

/**
 * @brief Move NPCs and return true if player is killed afterwards.
 */
bool MMz::moveNPCs(std::uint64_t &pos, bool walking, bool &gateToggled) const {
    gateToggled = false;
    for (std::uint64_t i = 0; i < MAX_NPCS; ++i) {
        if (chrIsAlive(pos, i) && chrIsWalking(pos, i) == walking) {
            moveNPC(pos, i, gateToggled);
        }
    }
    return collect(pos);
}

namespace {
/* Helper functions */
inline void setBit(std::uint64_t &number, std::size_t n, bool x) {
    /* Clear the Nth bit, then set it to X. */
    number = (number & ~(ONE << n)) | (static_cast<std::uint64_t>(x) << n);
}

inline std::size_t toWorldDim(std::size_t gridDim) {
    return (gridDim << 1) + 1;
}

inline std::size_t toGridDim(std::size_t worldDim) {
    return worldDim >> 1;
}

std::size_t toWorldLoc(std::uint64_t gridLoc, std::size_t gridCols) {
    std::size_t i = gridLoc / gridCols;
    std::size_t j = gridLoc % gridCols;
    return toWorldDim(i) * toWorldDim(gridCols) + toWorldDim(j);
}

std::size_t toGridLoc(std::uint64_t worldLoc, std::size_t worldCols) {
    std::size_t world_i = worldLoc / worldCols;
    std::size_t world_j = worldLoc % worldCols;
    return toGridDim(world_i) * toGridDim(worldCols) + toGridDim(world_j);
}

inline bool isChr(char c) {
    return c == WHITE_SCP || c == RED_SCP ||  \
            c == WHITE_MMY || c == RED_MMY || \
            c == PLAYER;
}

inline bool isNPC(char c) {
    return isChr(c) && c != PLAYER;
}

inline bool isKey(char c) {
    return c == KEY;
}

inline bool isGate(char c) {
    return c == GATE || c == UNLOCKED_GATE;
}

inline bool isExit(char c) {
    return c == EXIT;
}

inline bool isTrap(char c) {
    return c == TRAP;
}

inline void chrSetAlive(std::uint64_t &pos, std::uint64_t chrIdx) {
    pos |= CHARACTER_ALIVE_MASK << (chrIdx * CHARACTER_INFO_LENGTH);
}

inline void chrSetLoc(std::uint64_t &pos, std::uint64_t loc, std::uint64_t chrIdx) {
    std::uint64_t shift = chrIdx * CHARACTER_INFO_LENGTH;
    pos &= ~(CHARACTER_LOC_MASK << shift);
    pos |= loc << shift;
}

inline void chrSetStrength(std::uint64_t &pos, std::uint64_t strength, std::uint64_t chrIdx) {
    std::uint64_t shift = chrIdx * CHARACTER_INFO_LENGTH;
    pos &= ~(CHARACTER_STRENGTH_MASK << shift);
    pos |= strength << (shift + CHARACTER_STRENGTH_SHIFT);
}

bool addChr(std::uint64_t& pos, char chr, std::uint64_t loc) {
    std::uint64_t strength = chr - '0';
    if (strength == PLAYER_STRENGTH) {
        /* Player */
        if (playerIsAlive(pos)) {
            /* Error: player already exists. */
            return false;
        }
        chrSetAlive(pos, PLAYER_IDX);
        chrSetLoc(pos, loc, PLAYER_IDX);
        return true;
    } else {
        /* NPC */
        for (std::size_t i = 0; i < MAX_NPCS; ++i) {
            if (!chrIsAlive(pos, i)) {
                chrSetAlive(pos, i);
                chrSetLoc(pos, loc, i);
                chrSetStrength(pos, strength, i);
                return true;
            }
        }
        /* Max number of NPCs exceeded. */
        return false;
    }
}

inline void killChr(std::uint64_t &pos, std::size_t chrIdx) {
    pos &= ~(CHARACTER_ALIVE_MASK << (chrIdx * CHARACTER_INFO_LENGTH));
}

inline void killPlayer(std::uint64_t &pos) {
    killChr(pos, PLAYER_IDX);
}

/**
 * @brief Let NPCs kill each other and kill player if possible.
 * Returns true if player is killed, false otherwise.
 */
bool collect(std::uint64_t &pos) {
    std::uint64_t nloc1, nloc2;
    std::uint64_t ploc = playerLoc(pos);
    for (std::uint64_t i = 0; i < MAX_NPCS; ++i) {
        if (!chrIsAlive(pos, i)) {
            continue;
        }
        /* Kill player if possible. */
        nloc1 = chrLoc(pos, i);
        if (nloc1 == ploc) {
            killPlayer(pos);
        }
        for (std::uint64_t j = i + 1; j < MAX_NPCS; ++j) {
            nloc2 = chrLoc(pos, j);
            if (nloc1 == nloc2) {
                if (chrStrength(pos, i) < chrStrength(pos, j)) {
                    /* Kill i. */
                    killChr(pos, i);
                    /* NPC i already dead. */
                    break;
                } else {
                    /* Kill j. */
                    killChr(pos, j);
                }
            }
        }
    }
    return !playerIsAlive(pos);
}

void setGate(std::uint64_t& pos, char gate) {
    if (gate == GATE) {
        setGate(pos, true);
    } else if (gate == UNLOCKED_GATE) {
        setGate(pos, false);
    } else {
        /* This is not suppose to be reached. */
        assert(false);
    }
}

void setGate(std::uint64_t& pos, bool closed) {
    setBit(pos, GATE_SHIFT, closed);
}

void toggleGate(std::uint64_t &pos) {
    pos ^= ONE << GATE_SHIFT;
}

std::uint64_t moveChr(std::uint64_t &pos, std::size_t chrIdx, int direction, std::size_t gridCols) {
    int i_ofs, j_ofs;
    getOffsets(direction, i_ofs, j_ofs);
    std::uint64_t destLoc = chrLoc(pos, chrIdx) + i_ofs * gridCols + j_ofs;
    chrSetLoc(pos, destLoc, chrIdx);
    return destLoc;
}

std::uint64_t movePlayer(std::uint64_t &pos, int direction, std::size_t gridCols) {
    return moveChr(pos, PLAYER_IDX, direction, gridCols);
}

inline bool gateIsClosed(std::uint64_t pos) {
    return pos & GATE_MASK;
}

inline std::uint64_t chrLoc(std::uint64_t pos, std::size_t chrIdx) {
    return (pos >> (chrIdx * CHARACTER_INFO_LENGTH)) & CHARACTER_LOC_MASK;
}

inline bool chrIsAlive(std::uint64_t pos, std::size_t chrIdx) {
    return pos & (CHARACTER_ALIVE_MASK << (chrIdx * CHARACTER_INFO_LENGTH));
}

inline bool chrIsWalking(std::uint64_t pos, std::size_t chrIdx) {
    return pos & (CHARACTER_WALKING_MASK << (chrIdx * CHARACTER_INFO_LENGTH));
}

inline bool chrIsRed(std::uint64_t pos, std::size_t chrIdx) {
    return pos & (CHARACTER_COLOR_MASK << (chrIdx * CHARACTER_INFO_LENGTH));
}

inline std::uint64_t chrStrength(std::uint64_t pos, std::size_t chrIdx) {
    return ((pos >> (chrIdx * CHARACTER_INFO_LENGTH)) & CHARACTER_STRENGTH_MASK) >> CHARACTER_STRENGTH_SHIFT;
}

inline std::uint64_t playerLoc(std::uint64_t pos) {
    return chrLoc(pos, PLAYER_IDX);
}

inline bool playerIsAlive(std::uint64_t pos) {
    return chrIsAlive(pos, PLAYER_IDX);
}

void getOffsets(int direction, int &i_ofs, int &j_ofs) {
    j_ofs = 0;
    i_ofs = 0;
    switch (direction) {
    case MMzMove::WAIT:
        break;
    case MMzMove::UP:
        i_ofs = -1;
        break;
    case MMzMove::LEFT:
        j_ofs = -1;
        break;
    case MMzMove::DOWN:
        i_ofs = 1;
        break;
    case MMzMove::RIGHT:
        j_ofs = 1;
        break;
    default:
        /* Not reached. */
        assert(false);
    }
}

} // Anonymous namespace


std::size_t MMz::hashSize() const {
    return 0;
}
