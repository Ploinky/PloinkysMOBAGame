#include <stdio.h>

#include "common/navigation.h"


int main(int argc, char* argv[]) {
    NavigationMap navMap = NavigationMap();
    /*
    NavMesh navMesh = NavMesh();
    navMesh.LoadFromData({
        "v 0.0 0.0 0.0",
        "v 1000.0 0.0 -1000.0",
        "v 1000.0 0.0 0.0",
        "f 0 1 2"
    });
    */
    HGridBlocker a = navMap.AddBlocker(10, 10, 20);
    HGridBlocker b = navMap.AddBlocker(50, 50, 20);
    HGridBlocker c = navMap.AddBlocker(100, 100, 20);
    navMap.RemoveBlocker(a);
    HGridBlocker d = navMap.AddBlocker(150, 150, 20);

    if (a == b || a == c || a == d || b == c || b == d || c == d) {
        printf("FAILED: not all blockers got unique handles");
        return 1;
    }

    printf("PASSED: all tests passed");

    return 0;
}