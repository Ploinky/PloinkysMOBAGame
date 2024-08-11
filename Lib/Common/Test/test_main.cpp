#include <Common/pmg_physics.h>
#include <Common/PMG_Common.h>
#include <cassert>
#include <gtest/gtest.h>

TEST(PhysicsTest, TestCalculateAngle) {
	EXPECT_EQ(PMG::Physics::CalculateAngle({0,0}, {1, 0}), 0);
	EXPECT_EQ(PMG::Physics::CalculateAngle({0,0}, {0, -1}), 90);
	EXPECT_EQ(PMG::Physics::CalculateAngle({0,0}, {-1, 0}), -180);
	EXPECT_EQ(PMG::Physics::CalculateAngle({0,0}, {0, 1}), -90);
}


TEST(NavigationTest, TestGetPathSimple) {
	PMG::NavMesh mesh;
	mesh.LoadFromData({
		"v 0 0 0",
		"v 0 0 -100",
		"v 100 0 -100",
		"v 100 0 0",
		"f 0 1 2",
		"f 0 2 3",
	});
	PMG::NavigationCellGrid cellGrid = PMG::NavigationCellGrid(&mesh);
	
	EXPECT_EQ(mesh.mesh.size(), 2);
	EXPECT_EQ(cellGrid.CellCountX, 2);
	EXPECT_EQ(cellGrid.CellCountY, 2);
	
	std::vector<PMG::Physics::Vector2> path = cellGrid.GetPath({0, 0}, {99, -99});

	EXPECT_EQ(path.size(), 1);
	EXPECT_EQ(path[0].x, 99);
	EXPECT_EQ(path[0].y, -99);
}

TEST(NavigationTest, TestGetPathAdvanced) {
	PMG::NavMesh mesh;
	mesh.LoadFromData({
		"v 0 0 0",
		"v 50 0 0",
		"v 100 0 0",
		"v 150 0 0",
		"v 50 0 -50",
		"v 100 0 -50",
		"v 50 0 -100",
		"v 100 0 -100",
		"v 0 0 -150",
		"v 50 0 -150",
		"v 100 0 -150",
		"v 150 0 -150",
		"f 0 8 9",
		"f 0 9 1",
		"f 1 4 5",
		"f 1 5 2",
		"f 6 9 10",
		"f 6 10 7",
		"f 2 10 11",
		"f 2 11 3",
	});
	PMG::NavigationCellGrid cellGrid = PMG::NavigationCellGrid(&mesh);
	
	EXPECT_EQ(mesh.mesh.size(), 8);
	EXPECT_EQ(cellGrid.CellCountX, 3);
	EXPECT_EQ(cellGrid.CellCountY, 3);
	EXPECT_EQ(cellGrid.Cells[4]->IsWalkable, false);
	
	std::vector<PMG::Physics::Vector2> path = cellGrid.GetPath({0, 0}, {149, -149});

	EXPECT_EQ(path.size(), 2);
	EXPECT_EQ(path[0].x, 25);
	EXPECT_EQ(path[0].y, -125);
	EXPECT_EQ(path[1].x, 149);
	EXPECT_EQ(path[1].y, -149);
	
	path = cellGrid.GetPath({149, -149}, {0, 0});

	EXPECT_EQ(path.size(), 2);
	EXPECT_EQ(path[0].x, 25);
	EXPECT_EQ(path[0].y, -125);
	EXPECT_EQ(path[1].x, 0);
	EXPECT_EQ(path[1].y, 0);
}

TEST(NavigationTest, TestGetPathAdvanceder) {
	PMG::NavMesh mesh;
	mesh.LoadFromData({
		"v 0 0 0",
		"v 50 0 0",
		"v 100 0 0",
		"v 150 0 0",
		"v 50 0 -50",
		"v 100 0 -50",
		"v 50 0 -100",
		"v 100 0 -100",
		"v 0 0 -150",
		"v 50 0 -150",
		"v 100 0 -150",
		"v 150 0 -150",
		"f 0 8 9",
		"f 0 9 1",
		"f 1 4 5",
		"f 1 5 2",
		"f 4 6 7",
		"f 4 7 5",
		"f 6 9 10",
		"f 6 10 7",
		"f 2 10 11",
		"f 2 11 3",
	});
	PMG::NavigationCellGrid cellGrid = PMG::NavigationCellGrid(&mesh);
	
	std::vector<PMG::Physics::Vector2> path = cellGrid.GetPath({0, 0}, {149, -149});

	EXPECT_EQ(path.size(), 1);
	EXPECT_EQ(path[0].x, 149);
	EXPECT_EQ(path[0].y, -149);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}