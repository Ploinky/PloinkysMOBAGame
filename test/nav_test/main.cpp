#include <stdio.h>

#include "common/navigation.h"

int main(int argc, char* argv[]) {
    NavMesh navMesh = NavMesh();
    navMesh.LoadFromData({
        "v 0.0 0.0 0.0",
        "v 1000.0 0.0 -1000.0",
        "v 1000.0 0.0 0.0",
        "f 0 1 2"
    });
    return 0;
}