#include "MenuLayer.h"
#include "../nodes/RepeatingBackground.h"
#include <numbers>
#include <ctime>
#include "audio/AudioEngine.h"
#include "../managers/LanguageManager.h"
#include "NewSaveLayer.h"
#include "SelectSaveLayer.h"
#include "SettingsLayer.h"
#include "CreditsLayer.h"

using namespace ax;

bool MenuLayer::init() {
    if (!Layer::init()) return false;

    auto winSize = Director::getInstance()->getWinSize();

    // auto bg = cue::RepeatingBackground::create("");

    const std::string musicPath = "music/menuLoop.ogg";
    const auto currentAudioId = AudioEngine::getAudioIdForChannel(AudioEngine::MUSIC_CHANNEL);
    const auto currentFilePath = AudioEngine::getFilePathForChannel(AudioEngine::MUSIC_CHANNEL);
    const bool alreadyPlayingSameMusic = currentAudioId != AudioEngine::INVALID_AUDIO_ID &&
                                         AudioEngine::getState(currentAudioId) == AudioEngine::AudioState::PLAYING &&
                                         currentFilePath == musicPath;

    if (!alreadyPlayingSameMusic)
    {
        AudioEngine::playMusic2dOnChannel(AudioEngine::MUSIC_CHANNEL, musicPath, true, 0.6f);
    }

    m_logo = Sprite::create("menuLogo.png");
    m_logoBaseY = winSize.height * 0.8f;
    m_logo->setPosition({
        winSize.width / 2,
        m_logoBaseY
    });
    m_logo->setOpacity(0);
    m_logo->runAction(FadeIn::create(1.0f));
    this->addChild(m_logo);

    auto container = Node::create();

    auto& lm = LanguageManager::get();
    
    auto menu = Menu::createWithArray({
        MenuItemLabel::create(Label::createWithTTF(lm.getString("menu.newgame"), "fonts/pixel.ttf", 48), [](Object* sender) {
            Director::getInstance()->pushScene(TransitionFade::create(0.5f, NewSaveLayer::scene()));
        }),
        MenuItemLabel::create(Label::createWithTTF(lm.getString("menu.continue"), "fonts/pixel.ttf", 48), [](Object* sender) {
            Director::getInstance()->pushScene(TransitionFade::create(0.5f, SelectSaveLayer::scene()));
        }),
        MenuItemLabel::create(Label::createWithTTF(lm.getString("menu.settings"), "fonts/pixel.ttf", 48), [](Object* sender) {
            Director::getInstance()->pushScene(TransitionFade::create(0.5f, SettingsLayer::scene()));
        }),
        MenuItemLabel::create(Label::createWithTTF(lm.getString("menu.credits"), "fonts/pixel.ttf", 48), [](Object* sender) {
            Director::getInstance()->pushScene(TransitionFade::create(0.5f, CreditsLayer::scene()));
        }),
        MenuItemLabel::create(Label::createWithTTF(lm.getString("menu.exit"), "fonts/pixel.ttf", 48), [](Object* sender) {
            Director::getInstance()->end();
        })
    });
    menu->setPositionY(winSize.height * 0.35f);
    menu->alignItemsVertically();
    container->addChild(menu);

    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);
    const std::string year = std::to_string(localTime.tm_year + 1900);

    auto copyright = Label::createWithTTF(lm.getString("menu.copyright", year), "fonts/pixel.ttf", 24);
    copyright->setAnchorPoint(Vec2::ZERO);
    copyright->setPosition({ 10.f, 10.f });
    container->addChild(copyright);

    container->setCascadeOpacityEnabled(true);
    container->setOpacity(0);
    container->runAction(Sequence::create(
        DelayTime::create(0.5f),
        FadeIn::create(1.f),
        nullptr
    ));
    this->addChild(container);

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
