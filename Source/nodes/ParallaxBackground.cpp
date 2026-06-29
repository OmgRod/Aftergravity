#include "ParallaxBackground.h"

#include "axmol.h"

using namespace ax;

namespace cue {

ParallaxBackground* ParallaxBackground::create() {
    auto* ret = new ParallaxBackground();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ParallaxBackground::init() {
    if (!Node::init()) return false;

    this->setGlobalZOrder(-1);

    return true;
}

RepeatingBackground* ParallaxBackground::addLayer(const LayerConfig& config) {
    auto* holder = Node::create();
    holder->setAnchorPoint(Vec2::ZERO);
    holder->setPosition(Vec2::ZERO);
    this->addChild(holder);

    auto* bg = RepeatingBackground::create(
        config.texture,
        config.scale,
        config.mode);

    if (!bg) {
        holder->removeFromParent();
        AXLOGE("[cue::ParallaxBackground] failed to create layer for '{}'", config.texture);
        return nullptr;
    }

    bg->setSpeed(config.speed);

    bg->setGlobalZOrder(0);

    holder->addChild(bg);

    int z = static_cast<int>(_layers.size());
    holder->setLocalZOrder(z);

    _layers.push_back({ holder, bg, config.parallaxFactor });

    return bg;
}

void ParallaxBackground::setReferencePosition(Vec2 ref) {
    _ref = ref;

    for (auto& layer : _layers) {
        Vec2 offset{
            -ref.x * layer.factor.x,
            -ref.y * layer.factor.y
        };
        layer.holder->setPosition(offset);
    }
}

} // namespace cue
