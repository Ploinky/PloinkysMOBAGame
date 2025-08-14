#pragma once

#include "common/PMG_Common.h"

class ICanvas2D {
public:
    virtual ~ICanvas2D() = default;

    virtual void RenderText(int x, int y, int w, int h, float color[3], std::string strText) = 0;
    virtual void DrawRect(int x, int y, int w, int h, float color[3]) = 0;
    virtual void DrawShape(Vector2* points, int pointCount, float color[3]) = 0;
    virtual void FillShape(Vector2* points, int pointCount, float color[3]) = 0;
    virtual void FillRect(int x, int y, int w, int h, float color[3]) = 0;
    virtual void DrawImage(float x, float y, float w, float h, BitmapAsset_t& bmp) = 0;
};