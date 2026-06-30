#include "LanguageSelectLayer.h"
#include "MenuLayer.h"

bool LanguageSelectLayer::init() {
    if (!Layer::init()) return false;

    auto menu = Menu::create();
    auto& mgr = LanguageManager::get();

    auto winSize = Director::getInstance()->getWinSize();

    auto title = Label::createWithTTF(
        mgr.getString("langselect.title"),
        "fonts/pixel.ttf",
        48
    );

    title->setPosition({ winSize.width / 2, winSize.height * 0.9f });
    this->addChild(title);

    int languagesCount = static_cast<int>(aftergravity::Language::Count);

    auto index = std::make_shared<int>(0);

    auto cycleAction = ax::RepeatForever::create(
        ax::Sequence::create(
            ax::DelayTime::create(2.0f),

            ax::CallFunc::create([title, index, &mgr, languagesCount]() {

                *index = (*index + 1) % languagesCount;
                auto lang = static_cast<aftergravity::Language>(*index);

                title->runAction(ax::Sequence::create(
                    ax::FadeOut::create(0.25f),

                    ax::CallFunc::create([title, lang, &mgr]() {
                        title->setString(
                            mgr.getString("langselect.title", lang)
                        );
                    }),

                    ax::FadeIn::create(0.25f),
                    nullptr
                ));
            }),

            nullptr
        )
    );

    title->runAction(cycleAction);

    for (int i = 0; i < languagesCount; i++) {
        auto lang = static_cast<aftergravity::Language>(i);
        auto info = mgr.getLanguageInfo(lang);

        auto button = MenuItemLabel::create(
            Label::createWithTTF(
                info.displayName,
                "fonts/pixel.ttf",
                36
            ),
            [lang](ax::Object* sender) {
                auto& mgr = LanguageManager::get();
                mgr.setLanguage(lang);
                mgr.saveLanguage();

                Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MenuLayer::scene()));
            }
        );

        button->setTag(i);
        menu->addChild(button);
    }

    menu->alignItemsVertically();
    this->addChild(menu);

    return true;
}

Scene* LanguageSelectLayer::scene() {
    auto scene = Scene::create();
    auto layer = LanguageSelectLayer::create();
    scene->addChild(layer);
    return scene;
}