#include "CursorSystem.h"
#include "axmol.h"
#include "platform/RenderViewImpl.h"
#include <GLFW/glfw3.h>

using namespace ax;

CursorSystem* CursorSystem::s_instance = nullptr;

static GLFWwindow* getGLFWWindow() {
    auto view = static_cast<RenderViewImpl*>(Director::getInstance()->getRenderView());
    return view ? view->getWindow() : nullptr;
}

static void hideSysCursor() {
    if (auto* win = getGLFWWindow())
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

CursorSystem* CursorSystem::getInstance() {
    if (!s_instance) {
        s_instance = new CursorSystem();
        s_instance->init();
        s_instance->retain();
    }
    return s_instance;
}

bool CursorSystem::init() {
    if (!Node::init()) return false;

    ax::Director::getInstance()->getRenderView()->setCursorVisible(false);

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [this](EventMouse* e) {
        hideSysCursor();
        _mousePos = e->getLocation();
        if (_cursor) {
            _cursor->setPosition(_mousePos);
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithFixedPriority(mouseListener, 1);

    ax::Director::getInstance()->setNotificationNode(this);

    this->scheduleUpdate();

    return true;
}

void CursorSystem::syncMousePosition() {
    if (auto* win = getGLFWWindow()) {
        double x = 0.0;
        double y = 0.0;
        glfwGetCursorPos(win, &x, &y);

        auto director = Director::getInstance();
        auto glView = director->getGLView();
        auto frameSize = glView->getFrameSize();
        auto visibleSize = director->getVisibleSize();
        auto origin = director->getVisibleOrigin();

        float scaleX = visibleSize.width / frameSize.width;
        float scaleY = visibleSize.height / frameSize.height;

        float cx = static_cast<float>(x) * scaleX + origin.x;

        float cy = visibleSize.height - static_cast<float>(y) * scaleY + origin.y;

        _mousePos = Vec2(cx, cy);

        if (_cursor) {
            _cursor->setPosition(_mousePos);
        }
    }
}

void CursorSystem::update(float dt) {
    hideSysCursor();
    syncMousePosition();
}

void CursorSystem::initCursor(const std::string& frameName) {
    _frameName = frameName;

    _cursor = Sprite::createWithSpriteFrameName(frameName);

    if (_cursor) {
        _cursor->getTexture()->setAliasTexParameters();
        _cursor->setScale(2.0f);
        _cursor->setAnchorPoint(Vec2(0.0f, 1.0f));
        _cursor->setGlobalZOrder(9999999);
        addChild(_cursor);
    }
}

void CursorSystem::setCursor(const std::string& frameName) {
    if (!_cursor || frameName == _frameName) return;

    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    if (!frame) return;

    _frameName = frameName;
    _cursor->setSpriteFrame(frame);
    _cursor->getTexture()->setAliasTexParameters();
}


