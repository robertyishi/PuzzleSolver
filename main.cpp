#include <iostream>
#include "solver.h"
#include "toh.h"

using namespace std;

int main()
{
    ToH toh(9, 3);
    Solver solver(&toh);
    int rmt = solver.solve();
    cout << "rmt of root: " << rmt << endl;
    solver.printShortestPath(cout);
    return 0;
}
