#include "SettingsLayer.h"

bool SettingsLayer::init() {
    if (!Layer::init()) return false;

    return true;
}

Scene* SettingsLayer::scene() {
    auto scene = Scene::create();
    auto layer = SettingsLayer::create();
    scene->addChild(layer);
    return scene;
}

void SettingsLayer::onBack(Object* sender) {
    Director::getInstance()->popSceneWithTransition(Director::TransitionType::Fade, 0.5f);
}

void SettingsLayer::keyBackClicked() {
    onBack(nullptr);
}