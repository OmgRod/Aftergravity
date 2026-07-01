#pragma once

#include <axmol.h>

using namespace ax;

class MenuLayer : public Layer {
    bool init();

    Sprite* m_logo;
    float m_time = 0.0f;
    float m_logoBaseY = 0.0f;

    virtual void update(float dt) override;

public:
    CREATE_FUNC(MenuLayer);
    static Scene* scene();
};
