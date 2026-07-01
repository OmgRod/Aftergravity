#include "NewSaveLayer.h"

bool NewSaveLayer::init() {
    if (!Layer::init()) return false;

    return true;
}

Scene* NewSaveLayer::scene() {
    auto scene = Scene::create();
    auto layer = NewSaveLayer::create();
    scene->addChild(layer);
    return scene;
}

void NewSaveLayer::onBack(Object* sender) {
    Director::getInstance()->popSceneWithTransition(Director::TransitionType::Fade, 0.5f);
}

void NewSaveLayer::keyBackClicked() {
    onBack(nullptr);
}