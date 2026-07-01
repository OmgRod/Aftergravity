#include "SelectSaveLayer.h"

bool SelectSaveLayer::init() {
    if (!Layer::init()) return false;

    return true;
}

Scene* SelectSaveLayer::scene() {
    auto scene = Scene::create();
    auto layer = SelectSaveLayer::create();
    scene->addChild(layer);
    return scene;
}

void SelectSaveLayer::onBack(Object* sender) {
    Director::getInstance()->popSceneWithTransition(Director::TransitionType::Fade, 0.5f);
}

void SelectSaveLayer::keyBackClicked() {
    onBack(nullptr);
}