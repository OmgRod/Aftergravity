/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"
#include "axmol.h"
#include "platform/RenderViewImpl.h"
#include "layers/LoadingLayer.h"
#include "Inspector/Inspector.h"
#include "managers/CursorSystem.h"
#include "managers/InputManager.h"
#include "ImGui/imgui.h"
#include "ImGui/ImGuiPresenter.h"
#include "managers/InputManager.h"

#define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#    include "audio/AudioEngine.h"
#endif

using namespace ax;

static ax::Size designResolutionSize = ax::Size(1280, 720);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

// if you want a different context, modify the value of gfxContextAttrs
// it will affect all platforms
void AppDelegate::initGfxContextAttrs()
{
    // set graphics context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GfxContextAttrs gfxContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    // since axmol-2.2 vsync was enabled in engine by default
    // gfxContextAttrs.vsync = false;

    RenderView::setGfxContextAttrs(gfxContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
    auto renderView   = director->getRenderView();

    if (!renderView) {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
        auto viewImpl = RenderViewImpl::createWithRect(
            "Aftergravity", ax::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        renderView = viewImpl;
#else
        renderView = RenderViewImpl::create("Aftergravity");
#endif
        director->setRenderView(renderView);
    }

    renderView->setCursorVisible(false);

    // turn on display FPS
    director->setStatsDisplay(false);

#ifndef GITHUB_ACTIONS
    auto inspector = ax::extension::Inspector::getInstance();
    ax::extension::ImGuiPresenter::getInstance(); // Ensure ImGui context is created

    // Prevent ImGui from overriding our custom cursor
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    renderView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                    ResolutionPolicy::SHOW_ALL);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
    if (auto rvi = dynamic_cast<ax::RenderViewImpl*>(renderView)) {
        rvi->setFullscreen();
    }
#endif

    // run
    director->runWithScene(LoadingLayer::scene());

    // Initialise the unified input system
    InputManager::getInstance()->init();

#ifndef GITHUB_ACTIONS
    static bool s_inspectorOpen = false;
    static bool s_altPressed = false;

    auto listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_ALT || keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ALT) {
            s_altPressed = true;
        }

        if (keyCode == EventKeyboard::KeyCode::KEY_ENTER && s_altPressed) {
            auto view = dynamic_cast<ax::RenderViewImpl*>(Director::getInstance()->getRenderView());
            if (view) {
                if (view->isFullscreen()) {
                    view->setWindowed(designResolutionSize.width, designResolutionSize.height);
                } else {
                    view->setFullscreen();
                }
            }
        }

        if (keyCode == EventKeyboard::KeyCode::KEY_F11) {
            auto inspector = ax::extension::Inspector::getInstance();

            s_inspectorOpen = !s_inspectorOpen;

            if (s_inspectorOpen) {
                ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
                inspector->openForCurrentScene();
            } else {
                inspector->close();
            }
        }
    };

    listener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_ALT || keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ALT) {
            s_altPressed = false;
        }
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);

    Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        Director::EVENT_AFTER_SET_NEXT_SCENE, [](EventCustom*) {
            if (s_inspectorOpen) {
                ax::extension::Inspector::getInstance()->openForCurrentScene();
            }
        });
#endif

    // Hide system cursor and set up sprite cursor
    Director::getInstance()->getRenderView()->setCursorVisible(false);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sheets/cursorSheet.plist");
    CursorSystem::getInstance()->initCursor("tile_0168.png");

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}

void AppDelegate::applicationWillQuit() {}
