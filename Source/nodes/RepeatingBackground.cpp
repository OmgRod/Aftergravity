#include "RepeatingBackground.h"

#include "axmol.h"
#include "renderer/backend/Types.h"

using namespace ax;

namespace cue {

static Texture2D* loadPrivateTexture(const char* texturePath) {
    auto fullPath = FileUtils::getInstance()->fullPathForFilename(texturePath);
    if (fullPath.empty()) {
        AXLOGE("[cue::RepeatingBackground] texture not found: '{}'", texturePath);
        return nullptr;
    }

    auto cacheKey = fmt::format("cue.repeating-bg.{}", texturePath);

    auto* cache = Director::getInstance()->getTextureCache();

    if (auto* existing = cache->getTextureForKey(cacheKey))
        return existing;

    auto* img = new Image();
    if (!img->initWithImageFile(fullPath)) {
        img->release();
        AXLOGE("[cue::RepeatingBackground] failed to load image: '{}'", fullPath);
        return nullptr;
    }

    auto* tex = cache->addImage(img, cacheKey);
    img->release();

    return tex;
}

bool RepeatingBackground::init(const char* texture, float scale, RepeatMode mode, Size visibleSize) {
    auto* tex = loadPrivateTexture(texture);
    if (!tex || !this->initWithTexture(tex))
        return false;

    m_mode  = mode;
    m_speed = 1.0f;

    auto winSize = Director::getInstance()->getWinSize();
    m_visibleSize = visibleSize.isZero() ? winSize : visibleSize;

    Rect bgrect = this->getTextureRect();
    m_textureSize = this->getContentSize();

    const bool repeatX = (mode == RepeatMode::X || mode == RepeatMode::Both);
    const bool repeatY = (mode == RepeatMode::Y || mode == RepeatMode::Both);

    if (repeatX) {
        float minW = m_visibleSize.width * 3.f / scale;
        bgrect.size.width = 0;
        while (bgrect.size.width < minW)
            bgrect.size.width += m_textureSize.width;
    }

    if (repeatY) {
        float minH = m_visibleSize.height * 2.f / scale;
        bgrect.size.height = 0;
        while (bgrect.size.height < minH)
            bgrect.size.height += m_textureSize.height;
    }

    Texture2D::TexParams params;
    params.minFilter = backend::SamplerFilter::LINEAR;
    params.magFilter = backend::SamplerFilter::LINEAR;
    params.sAddressMode = repeatX
        ? backend::SamplerAddressMode::REPEAT
        : backend::SamplerAddressMode::CLAMP_TO_EDGE;
    params.tAddressMode = repeatY
        ? backend::SamplerAddressMode::REPEAT
        : backend::SamplerAddressMode::CLAMP_TO_EDGE;

    this->getTexture()->setTexParameters(params);
    this->setTextureRect(bgrect);
    this->setAnchorPoint(Vec2::ZERO);
    this->setScale(scale);
    this->setGlobalZOrder(-1);
    this->scheduleUpdate();

    return true;
}

void RepeatingBackground::update(float dt) {
    const bool repeatX = (m_mode == RepeatMode::X || m_mode == RepeatMode::Both);
    const bool repeatY = (m_mode == RepeatMode::Y || m_mode == RepeatMode::Both);

    const float s = this->getScale();

    float moveByX = repeatX ? (dt * m_speed * m_textureSize.width  / 12.f) : 0.f;
    float moveByY = repeatY ? (dt * m_speed * m_textureSize.height / 15.f) : 0.f;

    float loopX = 0.f;
    while (loopX < m_visibleSize.width)  loopX += m_textureSize.width  * s;

    float loopY = 0.f;
    while (loopY < m_visibleSize.height) loopY += m_textureSize.height * s;

    Vec2 newPos = this->getPosition() - Vec2{moveByX, moveByY};

    if (std::abs(newPos.x) > loopX)
        newPos.x += m_textureSize.width  * s;

    if (std::abs(newPos.y) > loopY)
        newPos.y += m_textureSize.height * s;

    this->setPosition(newPos);
}

RepeatingBackground* RepeatingBackground::create(const char* texture, float scale, RepeatMode mode, Size visibleSize) {
    auto* ret = new RepeatingBackground();
    if (ret->init(texture, scale, mode, visibleSize)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

} // namespace cue
