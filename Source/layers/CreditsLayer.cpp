#include "CreditsLayer.h"
#include "../managers/LanguageManager.h"

bool CreditsLayer::init() {
    if (!Layer::init()) return false;

    auto& lm = LanguageManager::get();

    auto winSize = Director::getInstance()->getWinSize();

    auto title = Label::createWithTTF(lm.getString("credits.title"), "fonts/pixel.ttf", 48);
    title->setPosition({ winSize.width / 2, winSize.height * 0.9f });
    this->addChild(title);

    return true;
}

Scene* CreditsLayer::scene() {
    auto scene = Scene::create();
    auto layer = CreditsLayer::create();
    scene->addChild(layer);
    return scene;
}

void CreditsLayer::onBack(Object* sender) {
    Director::getInstance()->popSceneWithTransition(Director::TransitionType::Fade, 0.5f);
}

void CreditsLayer::keyBackClicked() {
    onBack(nullptr);
}
