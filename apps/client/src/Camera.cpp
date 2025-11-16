#include "Camera.h"
#include "common/pmg_physics.h"

Camera::Camera() {
    this->fov = 59;
    this->nearClip = 1.0f;
    this->farClip = 10000.0f;
    this->position = Vector3{ 2000.0f, 1500.0f,-800.0f };
    this->rotation = Vector3{ -60, 0.0f, 0.0f };
}

Vector2 Camera::UnprojectWorldPoint(Vector3 vec3WorldPoint, int windowWidth, int windowHeight) {
    mat_t persp = PMathMatPerspectiveRH(ToRadians(59), (float)windowWidth / (float)windowHeight, nearClip, farClip);

    mat_t rot = PMathMatTranspose(PMathMatRotation(rotation.y, rotation.x, rotation.z));
    mat_t view = rot * PMathMatTranslation(-position.x, -position.y, -position.z);

    Vector4 point = Vector4(vec3WorldPoint.x, vec3WorldPoint.y, vec3WorldPoint.z, 1);
    point = view * point; // now in camera space
    point = persp * point; // now in NDC space
    point = Vector4(point.x / point.w, point.y / point.w, point.z / point.w, 1); // perspective divide
    double x = (1.0f + point.x) * 0.5 * windowWidth;
    double y = (1.0f - point.y) * 0.5 * windowHeight;

    return Vector2(x, y);
}

Ray Camera::CameraRay(Vector2 vec2Screen, int windowWidth, int windowHeight) {
    float ndcX = (2.0f * vec2Screen.x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * vec2Screen.y) / windowHeight; // flip if screen y goes down
    float ndcZ = 0.0f;

    Vector4 rayClip = { ndcX, ndcY, -1.0f, 1.0f };

    //mat_t persp = PMathMatPerspectiveRH((float)m_sceneWidth / (float)m_sceneHeight, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);
    mat_t persp = PMathMatPerspectiveRHInverse(ToRadians(fov), (float)windowWidth / (float)windowHeight, nearClip, farClip);
    
    Vector4 rayEye = persp * rayClip;
    
    rayEye = { rayEye.x / rayEye.w, rayEye.y / rayEye.w, rayEye.z / rayEye.w, 0.0 };
    
    mat_t view = PMathMatRotation(rotation.y, rotation.x, rotation.z) * PMathMatTranslation(position.x, position.y, position.z);
    Vector4 rayWorld4 = view * rayEye; // or cameraWorld * ray_eye
    Vector3 rayWorld = Vector3(rayWorld4.x, rayWorld4.y, rayWorld4.z).Normalize();

    return Ray(position, Vector3(rayWorld.x, rayWorld.y, rayWorld.z));
}