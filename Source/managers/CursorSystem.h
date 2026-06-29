#pragma once

#include "2d/Node.h"
#include "2d/Sprite.h"

class CursorSystem : public ax::Node {
public:
    static CursorSystem* getInstance();

    void initCursor(const std::string& frameName);
    void setCursor(const std::string& frameName);

    bool init() override;
    void update(float dt) override;


private:

    ax::Sprite* _cursor = nullptr;
    std::string _frameName;

    ax::Vec2 _mousePos;

    static CursorSystem* s_instance;
};