#include "mmz.h"
#include "solver.h"
#include "toh.h"
#include <chrono>
#include <iostream>

using namespace std;

const char *fileName = "./res/mmz/ra_13.maze";

void solve(const MMz &mmz, bool verbose);
void timeSolve();
void debugPlay();

int main()
{
    MMz mmz(fileName);
    solve(mmz, true);
    return 0;
}

void solve(const MMz &mmz, bool verbose) {
    Solver solver(&mmz);
    int rmt = solver.solve();
    if (verbose) {
        cout << "rmt of root: " << rmt << endl;
        solver.printShortestPath(cout);
    }
}

void timeSolve() {
    MMz mmz(fileName);
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        solve(mmz, false);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    // floating-point duration: no duration_cast needed
    std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

    // integral duration: requires duration_cast
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // converting integral duration to integral duration of shorter divisible time unit:
    // no duration_cast needed
    std::chrono::duration<long, std::micro> int_usec = int_ms;

    std::cout << "solve() took " << fp_ms.count() << " ms, "
              << "or " << int_ms.count() << " whole milliseconds "
              << "(which is " << int_usec.count() << " whole microseconds)" << std::endl;
}

void debugPlay() {
    MMz mmz(fileName);
    MMzPosition *pos = static_cast<MMzPosition *>(mmz.getInitialPosition());
    MMzPosition *nextPos;
    MMzMove *move;
    while (!mmz.isPrimitivePosition(pos)) {
        cout << mmz.asString(pos) << endl;
        char m;
        cout << "enter move: ";
        cin >> m;
        switch (m) {
        case 'w':
            move = new MMzMove(MMzMove::UP);
            break;
        case 'a':
            move = new MMzMove(MMzMove::LEFT);
            break;
        case 's':
            move = new MMzMove(MMzMove::DOWN);
            break;
        case 'd':
            move = new MMzMove(MMzMove::RIGHT);
            break;
        default:
            move = new MMzMove(MMzMove::WAIT);
            break;
        }
        nextPos = static_cast<MMzPosition *>(mmz.doMove(pos, move));
        delete move;
        delete pos;
        pos = nextPos;
    }
    delete pos;
}
