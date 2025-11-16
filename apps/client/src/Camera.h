#pragma once

#include <common/pmg_physics.h>

class Camera {
public:
    Vector3 position;
    Vector3 rotation;
    float fov;
    float nearClip;
    float farClip;

    Camera();

    /**
     * Unprojects a world point to a screen space coordinate.
     * @param vec3WorldPoint The point in world space
     * @return A `Vector2` where x is the screen-space x coordinate and y is the screen-space y coordinate
     */
    Vector2 UnprojectWorldPoint(Vector3 vec3WorldPoint, int windowWidth, int windowHeight);

    /**
     * Projects a screen-space coordinate into a ray from the camera through the game world
     * @param vec2Screen The screen-space point
     * @return A `Ray` from the camera through the screen-space point into the 3D game world
     */
    Ray CameraRay(Vector2 vec2Screen, int windowWidth, int windowHeight);
};
