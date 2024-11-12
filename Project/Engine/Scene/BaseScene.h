#pragma once

#include <memory>

class BaseScene
{
public:

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

protected:


};