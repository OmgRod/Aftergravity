#pragma once

#include "RepeatingBackground.h"
#include "2d/Node.h"

#include <vector>

namespace cue {

class ParallaxBackground : public ax::Node {
public:
    struct LayerConfig {
        const char* texture;
        float scale = 1.0f;
        float speed = 1.0f;
        ax::Vec2 parallaxFactor = {0.5f, 0.f};
        RepeatMode mode = RepeatMode::X;
    };

    static ParallaxBackground* create();

    bool init() override;

    RepeatingBackground* addLayer(const LayerConfig& config);

    void setReferencePosition(ax::Vec2 ref);

    ax::Vec2 getReferencePosition() const { return _ref; }

private:
    struct Layer {
        ax::Node*            holder;
        RepeatingBackground* bg;
        ax::Vec2             factor;
    };

    std::vector<Layer> _layers;
    ax::Vec2           _ref;
};

} // namespace cue
