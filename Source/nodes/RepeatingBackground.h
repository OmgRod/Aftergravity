#pragma once

#include "2d/Sprite.h"
#include "math/Vec2.h"
#include "base/Types.h"

namespace cue {

enum class RepeatMode {
    X, Y, Both
};

class RepeatingBackground : public ax::Sprite {
public:
    /// Create a tiling/scrolling background.
    ///
    /// @param texture      Path to the texture file (relative to Content/).
    /// @param scale        Uniform scale applied to the sprite.
    /// @param mode         Which axes to tile and scroll.
    /// @param visibleSize  Viewport size used for loop detection; defaults to
    ///                     Director::getWinSize() when zero.
    static RepeatingBackground* create(const char* texture,
                                       float       scale       = 1.0f,
                                       RepeatMode  mode        = RepeatMode::X,
                                       ax::Size    visibleSize = {});

    void  setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const      { return m_speed; }

protected:
    float      m_speed      = 1.0f;
    RepeatMode m_mode       = RepeatMode::X;
    ax::Size   m_visibleSize;
    ax::Size   m_textureSize;

    bool init(const char* texture, float scale, RepeatMode mode, ax::Size visibleSize);
    void update(float dt) override;
};

} // namespace cue