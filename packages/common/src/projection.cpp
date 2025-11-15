#include "projection.h"
#include "matrix.h" // TODO: remove after creating custom matrix class
#include "vector4.h"

Ray ScreenToRay(Vector2 screen_coords, Vector3 camera_position, Vector3 camera_rotation, float aspect_ratio, float fov, float near_clip, float far_clip, float scene_width, float scene_height) {
    //mat_t persp = PMathMatPerspectiveRH((float)m_sceneWidth / (float)m_sceneHeight, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);
    mat_t persp = PMathMatPerspectiveRH(fov, aspect_ratio, near_clip, far_clip);
    mat_t view = PMathMatRotation(camera_rotation.z, camera_rotation.y, camera_rotation.x) *
        PMathMatTranslation(camera_position.x, camera_position.y, camera_position.z);
    view = PMathMatTranspose(PMathMatInverse(view));


    Vector3 relScreen = {
        screen_coords.x * 2.0f / scene_width - 1.0f,
        1.0f - (screen_coords.y * 2.0f) / (float)scene_height,
        -1.0f
    };

    Vector4 rayClip = {
        relScreen.x,
        relScreen.y,
        -1.0f,
        1.0f
    };

    mat_t perspInverse = PMathMatInverse(persp);

    Vector4 rayEye = perspInverse * rayClip;

    rayEye = { rayEye.x, rayEye.y, -1.0, 0.0 };

    Vector4 rw4 = (view * rayEye);
    Vector3 rayWorld = { rw4.x, rw4.y, rw4.z };

    rayWorld = rayWorld.Normalize();

    return Ray(camera_position, Vector3(rayWorld.x, rayWorld.y, rayWorld.z));
}

	
Vector4 operator*(const Vector4& vec, const mat_t& M) {
    return {
        vec.x * M.m[0][0] + vec.y * M.m[0][1] + vec.z * M.m[0][2] + vec.w * M.m[0][3], // X
        vec.x * M.m[1][0] + vec.y * M.m[1][1] + vec.z * M.m[1][2] + vec.w * M.m[1][3], // Y
        vec.x * M.m[2][0] + vec.y * M.m[2][1] + vec.z * M.m[2][2] + vec.w * M.m[2][3], // Z
        vec.x * M.m[3][0] + vec.y * M.m[3][1] + vec.z * M.m[3][2] + vec.w * M.m[3][3]  // W
    };
}
Vector2 WorldToScreen(Vector3 world_coords, mat_t model_mat, mat_t projection_matrix, mat_t view_matrix) {
    Vector4 objectPos4 = Vector4(world_coords.x, world_coords.y, world_coords.z, 1.0f);
    
    objectPos4 = objectPos4 * model_mat;
    objectPos4 = objectPos4 * view_matrix;
    objectPos4 = objectPos4 * projection_matrix;

    float clipSpaceX = objectPos4.x / objectPos4.w;
    float clipSpaceY = objectPos4.y/ objectPos4.w;

    return { clipSpaceX, clipSpaceY };
}