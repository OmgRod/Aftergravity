#include "LoadingLayer.h"

bool LoadingLayer::init() {
    if (!Layer::init()) return false;

    auto winSize = Director::getInstance()->getWinSize();

    if (auto logo = Sprite::create("menuLogo.png")) {
        float targetWidth = winSize.width * 0.5f;
        float scale = targetWidth / logo->getContentSize().width;
        logo->setScale(scale);

        logo->setPosition(winSize / 2);
        logo->setOpacity(0);
        logo->runAction(Sequence::create(
            FadeIn::create(1.0f),
            nullptr
        ));

        this->addChild(logo);
    }
        
    if (m_loadingLabel = Label::createWithTTF("Loading assets...", "fonts/8bitoperator_jve.ttf", 32, Vec2::ZERO, TextHAlignment::CENTER, TextVAlignment::TOP)) {
        m_loadingLabel->setPosition({ winSize.width / 2, winSize.height * 0.1f });
        this->addChild(m_loadingLabel);
    }
    
    this->runAction(Sequence::create(
        DelayTime::create(1.f),
        CallFunc::create(AX_CALLBACK_0(LoadingLayer::loadAssets, this)),
        nullptr
    ));

    return true;
}

Scene* LoadingLayer::scene() {
    auto scene = Scene::create();
    auto layer = LoadingLayer::create();
    scene->addChild(layer);
    return scene;
}

void LoadingLayer::loadAssets() {
    auto sfc = SpriteFrameCache::getInstance();
    // Spritesheets

    m_loadingLabel->setString("Loading spritesheets...");

    sfc->addSpriteFramesWithFile("sheets/cursorSheet.plist");
    sfc->addSpriteFramesWithFile("sheets/inputsSheet.plist");
    sfc->addSpriteFramesWithFile("sheets/minimapSheetA.plist");
    sfc->addSpriteFramesWithFile("sheets/minimapSheetB.plist");
    sfc->addSpriteFramesWithFile("sheets/minimapSheetC.plist");
    sfc->addSpriteFramesWithFile("sheets/minimapSheetD.plist");
    sfc->addSpriteFramesWithFile("sheets/minimapSheetE.plist");
    sfc->addSpriteFramesWithFile("sheets/minimapSheetF.plist");

    m_loadingLabel->setString("Done!");
}