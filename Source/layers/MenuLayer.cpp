#include "MenuLayer.h"
#include "../nodes/RepeatingBackground.h"
#include <numbers>
#include "audio/AudioEngine.h"

bool MenuLayer::init() {
    if (!Layer::init()) return false;

    auto winSize = Director::getInstance()->getWinSize();

    // auto bg = cue::RepeatingBackground::create("");

    m_audioID = AudioEngine::play2d("music/menuLoop.ogg", true);

    m_logo = Sprite::create("menuLogo.png");
    m_logoBaseY = winSize.height * 0.8f;
    m_logo->setPosition({
        winSize.width / 2,
        m_logoBaseY
    });
    m_logo->setOpacity(0);
    m_logo->runAction(FadeIn::create(1.0f));
    this->addChild(m_logo);

    // auto contents = 

    scheduleUpdate();

    return true;
}

Scene* MenuLayer::scene() {
    auto scene = Scene::create();
    auto layer = MenuLayer::create();
    scene->addChild(layer);
    return scene;
}

void MenuLayer::update(float dt) {
    m_time += dt;

    constexpr float amplitude = 12.0f;
    constexpr float speed = 0.5f;

    float offset = std::sin(m_time * speed * std::numbers::pi_v<float> * 2.0f) * amplitude;

    m_logo->setPositionY(m_logoBaseY + offset);
}

MenuLayer::~MenuLayer() {
    AudioEngine::stop(m_audioID);
}
