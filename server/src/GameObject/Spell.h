#pragma once

class ISpell {
public:
    virtual void OnCastStart() = 0;
    virtual void OnCast() = 0;

    float fCastPoint = 1;
};