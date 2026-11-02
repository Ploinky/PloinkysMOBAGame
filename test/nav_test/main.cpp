#include <stdio.h>

#include "common/navigation.h"

int main(int argc, char* argv[]) {
    NavigationMap navMap = NavigationMap();
    NavMesh navMesh = NavMesh();
    navMesh.LoadFromData({
        "v 0.0 0.0 0.0", "v 1000.0 0.0 0.0", "v 2000.0 0.0 0.0",
        "v 0.0 0.0 -1000.0", "v 1000.0 0.0 -1000.0", "v 2000.0 0.0 -1000.0",
        "v 1000.0 0.0 -2000.0", "v 2000.0 0.0 -2000.0",
        "f 0 1 4", "f 0 4 3",
        "f 1 2 5", "f 1 5 4",
        "f 4 5 7", "f 4 7 6"
    });
    navMap.m_pMesh = &navMesh;

    HGridBlocker a = navMap.AddBlocker(10, 10, 20);
    HGridBlocker b = navMap.AddBlocker(50, 50, 20);
    HGridBlocker c = navMap.AddBlocker(100, 100, 20);
    navMap.RemoveBlocker(a);
    HGridBlocker d = navMap.AddBlocker(150, 150, 20);

    if (a == b || a == c || a == d || b == c || b == d || c == d) {
        printf("FAILED: not all blockers got unique handles");
        return 1;
    }

    Line aLine = Line({100, 0}, {200, 0});
    Circle aCircle = Circle({100, 0}, 50);
    Vector2 vec2 = TestCollision(aLine, aCircle);

    if (vec2.x != 150 || vec2.y != 0) {
        printf("FAILED: did not leave enclosing circle correctly");
        return 1;
    }

    Vector2 testBlockerPos = {600, -900};
    HGridBlocker testBlocker = navMap.AddBlocker(testBlockerPos.x, testBlockerPos.y, 50);
    PathResult_t pathResult = navMap.RequestPath({200, -800}, {1800, -1800}, 50);
    bool avoidsTestBlocker = false;
    for (PathSegment_t segment : pathResult.vecVec2Path) {
        if ((segment.vec2To - testBlockerPos).Length() <= 101) {
            avoidsTestBlocker = true;
        }
    }

    if (!avoidsTestBlocker) {
        printf("FAILED: did not avoid intermediate blocker");
        return 1;
    }

    printf("PASSED: all tests passed");

    return 0;
}