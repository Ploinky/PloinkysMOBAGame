#include <stdio.h>

#include "common/navigation.h"

int main(int argc, char* argv[]) {
    NavMesh navMesh = NavMesh();
    navMesh.LoadFromData({
        "v 0.0 0.0 0.0", "v 1000.0 0.0 0.0", "v 2000.0 0.0 0.0",
        "v 0.0 0.0 -1000.0", "v 1000.0 0.0 -1000.0", "v 2000.0 0.0 -1000.0",
        "v 1000.0 0.0 -2000.0", "v 2000.0 0.0 -2000.0",
        "f 0 1 4", "f 0 4 3",
        "f 1 2 5", "f 1 5 4",
        "f 4 5 7", "f 4 7 6"
    });

    {
        NavigationMap navMap = NavigationMap();
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
    }

    {
        Line aLine = Line({100, 0}, {200, 0});
        Circle aCircle = Circle({100, 0}, 50);
        Vector2 vec2 = TestCollision(aLine, aCircle);

        if (vec2.x != 150 || vec2.y != 0) {
            printf("FAILED: did not leave enclosing circle correctly");
            return 1;
        }
    }
    
    {
        NavigationMap navMap = NavigationMap();
        navMap.m_pMesh = &navMesh;
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
    }

    {
        NavigationMap navMap = NavigationMap();
        navMap.m_pMesh = &navMesh;
        PathResult_t pathResult = navMap.RequestPath({200, -800}, {1800, -1800}, 50);
        PathSegment_t segment = pathResult.vecVec2Path.at(pathResult.vecVec2Path.size() - 1);
        if (pathResult.vecVec2Path.size() != 2 || segment.vec2To.x != 1800 || segment.vec2To.y != -1800) {
            printf("FAILED: not navigating to final destination");
            return 1;
        }
    }

    {
        NavigationMap navMap = NavigationMap();
        navMap.m_pMesh = &navMesh;
        navMap.AddBlocker(500, -510, 50);
        PathResult_t pathResult = navMap.RequestPath({200, -500}, {800, -500}, 50);
        
        bool bFoundArc = false;
        for (int i = 1; i < pathResult.vecVec2Path.size(); i++) {
            PathSegment_t segment = pathResult.vecVec2Path.at(i);
            if (segment.eType != EEdgeType::ARC) {
                continue;
            }
            bFoundArc = true;
            if (segment.vec2To.x <= pathResult.vecVec2Path.at(i - 1).vec2To.x || !segment.arcClockwise) {
                printf("FAILED: arc goes the wrong way");
                return 1;
            }
        }

        if (!bFoundArc) {
            printf("FAILED: path has no arc around blocker");
            return 1;
        }
    }

    {
        NavigationMap navMap = NavigationMap();
        navMap.m_pMesh = &navMesh;
        navMap.AddBlocker(500, -510, 50);
        PathResult_t pathResult = navMap.RequestPath({800, -500}, {100, -500}, 50);
        
        bool bFoundArc = false;
        for (int i = 1; i < pathResult.vecVec2Path.size(); i++) {
            PathSegment_t segment = pathResult.vecVec2Path.at(i);
            if (segment.eType != EEdgeType::ARC) {
                continue;
            }
            bFoundArc = true;
            if (segment.vec2To.x >= pathResult.vecVec2Path.at(i - 1).vec2To.x || segment.arcClockwise) {
                printf("FAILED: arc goes the wrong way (clockwise)");
                return 1;
            }
        }

        if (!bFoundArc) {
            printf("FAILED: path has no arc around blocker (clockwise)");
            return 1;
        }
    }

    {
        NavigationMap navMap = NavigationMap();
        PathSegment_t segment;
        segment.eType = EEdgeType::ARC;
        segment.arcCenter = {500, -500};
        segment.arcRadius = 100;
        segment.arcClockwise = true;
        segment.vec2To = {400, -500};

        Vector2 vec2Pos = {600, -500};
        int steps = 0;

        while((vec2Pos - segment.vec2To).Length() > 0.01f && steps < 100) {
            vec2Pos = navMap.Step(vec2Pos, {segment}, 5).vec2Pos;
            steps++;
            float radius = (vec2Pos - segment.arcCenter).Length();
            if (radius < 99 || radius > 101 || vec2Pos.y > -499) {
                printf("FAILED: left the arc walking clockwise");
                return 1;
            }
        }
        
        if (steps >= 100) {
            printf("FAILED: did not reach end of arc walking clockwise");
            return 1;
        }
    }

    {
        NavigationMap navMap = NavigationMap();
        PathSegment_t segment;
        segment.eType = EEdgeType::ARC;
        segment.arcCenter = {500, -500};
        segment.arcRadius = 100;
        segment.arcClockwise = false;
        segment.vec2To = {600, -500};

        Vector2 vec2Pos = {400, -500};
        int steps = 0;

        while((vec2Pos - segment.vec2To).Length() > 0.01f && steps < 100) {
            vec2Pos = navMap.Step(vec2Pos, {segment}, 5).vec2Pos;
            steps++;
            float radius = (vec2Pos - segment.arcCenter).Length();
            if (radius < 99 || radius > 101 || vec2Pos.y > -499) {
                printf("FAILED: left the arc walking counter-clockwise");
                return 1;
            }
        }
        
        if (steps >= 100) {
            printf("FAILED: did not reach end of arc walking counter-clockwise");
            return 1;
        }
    }

    {
        NavigationMap navMap = NavigationMap();
        navMap.m_pMesh = &navMesh;
        navMap.AddBlocker(1200, -750, 50);
        navMap.AddBlocker(1375, -750, 50);
        navMap.AddBlocker(1268.466553f, -598.851685f, 50);
        navMap.AddBlocker(1285.672485f, -698.003174f, 50);
        PathResult_t pathResult = navMap.RequestPath({1183.092041f, -651.439758f}, {1200, -750}, 50);

        if (pathResult.eResultType != EPathResultType::UNREACHABLE) {
            printf("FAILED: unreachable destination not reported");
            return 1;
        }
    }

    printf("PASSED: all tests passed");

    return 0;
}