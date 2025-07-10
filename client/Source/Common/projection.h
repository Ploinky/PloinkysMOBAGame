#pragma once

#include "ray.h"
#include "vector2.h"
#include "matrix.h"

Ray ScreenToRay(Vector2 screen_coords, Vector3 camera_position, Vector3 camera_rotation, float aspect_ratio, float fov, float near_clip, float far_clip, float scene_width, float scene_height);
Vector2 WorldToScreen(Vector3 world_coords, mat_t model_mat, mat_t projection_matrix, mat_t view_matrix);