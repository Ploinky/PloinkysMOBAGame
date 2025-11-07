#include "test-framework.h"

#include "common/PMG_Common.h"

TEST_CASE(TestPerspectiveRH) {
    mat m = PMathMatPerspectiveRH(ToRadians(90), 1920.0f/1680.0f, 0.1f, 1000.0f);

    REQUIRE(CompareFloat(m.m[0][0], 0.875));
    REQUIRE(CompareFloat(m.m[1][1], 1));
    REQUIRE(CompareFloat(m.m[2][2], -1.0001));
    REQUIRE(CompareFloat(m.m[2][3], -1));
    REQUIRE(CompareFloat(m.m[3][2], -0.10001));
}


TEST_CASE(TestWorldToScreen) {
    Vector3 world_coords = Vector3(1.0f, 2.0f, -5.0f);
    mat_t model_mat = mat::Identity();
    mat_t view_matrix;
    view_matrix.m[0][0] = 1.0f;
    view_matrix.m[1][1] = 1.0f;
    view_matrix.m[2][2] = 1.0f;
    view_matrix.m[3][3] = 1.0f;
    view_matrix.m[2][3] = 2.0f;

    mat_t projection_matrix = mat::Identity();
    projection_matrix.m[0][0] = 0.875f;
    projection_matrix.m[1][1] = 1.0f;
    projection_matrix.m[2][2] = -1.0001f;
    projection_matrix.m[2][3] = -1.000f;
    projection_matrix.m[3][2] = -.10001f;

    Vector2 ndc = WorldToScreen(world_coords, model_mat, projection_matrix, view_matrix);

    REQUIRE(CompareFloat(ndc.x, 0.673f));
    REQUIRE(CompareFloat(ndc.y, 1.538f));
}