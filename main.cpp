#include <iostream>
#include "solver.h"
#include "toh.h"
#include "mmz.h"

using namespace std;

int main()
{
    MMz mmz("./res/mmz/ra_13.maze");
//    MMzPosition *pos = static_cast<MMzPosition *>(mmz.getInitialPosition());
//    MMzPosition *nextPos;
//    MMzMove *move;
//    while (!mmz.isPrimitivePosition(pos)) {
//        cout << mmz.asString(pos) << endl;
//        char m;
//        cout << "enter move: ";
//        cin >> m;
//        switch (m) {
//        case 'w':
//            move = new MMzMove(MMzMove::UP);
//            break;
//        case 'a':
//            move = new MMzMove(MMzMove::LEFT);
//            break;
//        case 's':
//            move = new MMzMove(MMzMove::DOWN);
//            break;
//        case 'd':
//            move = new MMzMove(MMzMove::RIGHT);
//            break;
//        default:
//            move = new MMzMove(MMzMove::WAIT);
//            break;
//        }
//        nextPos = static_cast<MMzPosition *>(mmz.doMove(pos, move));
//        delete move;
//        delete pos;
//        pos = nextPos;
//    }
//    delete pos;
    Solver solver(&mmz);
    int rmt = solver.solve();
    cout << "rmt of root: " << rmt << endl;
    solver.printShortestPath(cout);
    return 0;
}
