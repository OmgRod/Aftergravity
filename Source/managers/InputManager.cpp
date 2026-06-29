#include "InputManager.h"
#include "axmol.h"

#include <algorithm>
#include <cctype>

using namespace ax;

InputManager* InputManager::s_instance = nullptr;

InputManager* InputManager::getInstance() {
    if (!s_instance)
        s_instance = new InputManager();
    return s_instance;
}

static bool containsCI(std::string_view haystack, std::string_view needle) {
    if (needle.empty()) return true;
    if (haystack.size() < needle.size()) return false;

    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(),   needle.end(),
        [](unsigned char a, unsigned char b) {
            return std::tolower(a) == std::tolower(b);
        });
    return it != haystack.end();
}

ControllerType InputManager::detectType(std::string_view name) {
    if (containsCI(name, "Xbox")        ||
        containsCI(name, "xinput")      ||
        containsCI(name, "Microsoft"))
        return ControllerType::Xbox;

    if (containsCI(name, "PlayStation") ||
        containsCI(name, "DualShock")   ||
        containsCI(name, "DualSense")   ||
        containsCI(name, "DUALSHOCK")   ||
        containsCI(name, "DUALSENSE")   ||
        containsCI(name, "Sony"))
        return ControllerType::PlayStation;

    return ControllerType::Unknown;
}

void InputManager::init() {
    if (_initialised) return;
    _initialised = true;

    auto* dispatcher = Director::getInstance()->getEventDispatcher();

    auto* kbListener = EventListenerKeyboard::create();

    kbListener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event*) {
        auto it = _keyToAction.find(static_cast<int>(key));
        if (it != _keyToAction.end())
            dispatchAction(it->second, InputEvent::Pressed);
    };

    kbListener->onKeyReleased = [this](EventKeyboard::KeyCode key, Event*) {
        auto it = _keyToAction.find(static_cast<int>(key));
        if (it != _keyToAction.end())
            dispatchAction(it->second, InputEvent::Released);
    };

    dispatcher->addEventListenerWithFixedPriority(kbListener, 1);

    auto* ctrlListener = EventListenerController::create();

    ctrlListener->onConnected = [this](Controller* ctrl, Event*) {
        onControllerConnected(ctrl);
    };

    ctrlListener->onDisconnected = [this](Controller* ctrl, Event*) {
        onControllerDisconnected(ctrl);
    };

    ctrlListener->onKeyDown = [this](Controller* ctrl, int keyCode, Event*) {
        if (_activeController != ctrl) {
            _activeController = ctrl;
            onControllerConnected(ctrl);
        }

        auto it = _buttonToAction.find(keyCode);
        if (it != _buttonToAction.end())
            dispatchAction(it->second, InputEvent::Pressed);
    };

    ctrlListener->onKeyUp = [this](Controller* ctrl, int keyCode, Event*) {
        if (_activeController != ctrl) {
            _activeController = ctrl;
            onControllerConnected(ctrl);
        }

        auto it = _buttonToAction.find(keyCode);
        if (it != _buttonToAction.end())
            dispatchAction(it->second, InputEvent::Released);
    };

    ctrlListener->onAxisEvent = [this](Controller* ctrl, int axisCode, Event*) {
        if (_activeController != ctrl) {
            _activeController = ctrl;
            onControllerConnected(ctrl);
        }

        const auto& status = ctrl->getKeyStatus(axisCode);
        float value = status.value;

        if (std::abs(value) < AXIS_DEADZONE)
            value = 0.0f;

        dispatchAxis(static_cast<ax::Controller::Key>(axisCode), value);
    };

    dispatcher->addEventListenerWithFixedPriority(ctrlListener, 1);

    Controller::startDiscoveryController();

    // --- Base UI Bindings ---
    bindKey("ui_up", EventKeyboard::KeyCode::KEY_UP_ARROW);
    bindKey("ui_up", EventKeyboard::KeyCode::KEY_W);
    bindButton("ui_up", Controller::Key::BUTTON_DPAD_UP);

    bindKey("ui_down", EventKeyboard::KeyCode::KEY_DOWN_ARROW);
    bindKey("ui_down", EventKeyboard::KeyCode::KEY_S);
    bindButton("ui_down", Controller::Key::BUTTON_DPAD_DOWN);

    bindKey("ui_submit", EventKeyboard::KeyCode::KEY_ENTER);
    bindKey("ui_submit", EventKeyboard::KeyCode::KEY_SPACE);
    bindButton("ui_submit", Controller::Key::BUTTON_A);
}

void InputManager::shutdown() {
    Controller::stopDiscoveryController();
    _initialised = false;
}

void InputManager::bindKey(std::string_view action, EventKeyboard::KeyCode key) {
    std::string a{action};
    _keyBindings[a].push_back(key);
    _keyToAction[static_cast<int>(key)] = a;
}

void InputManager::bindButton(std::string_view action, Controller::Key button) {
    std::string a{action};
    _buttonBindings[a].push_back(button);
    _buttonToAction[static_cast<int>(button)] = a;
}

void InputManager::unbindAll(std::string_view action) {
    std::string a{action};

    auto kit = _keyBindings.find(a);
    if (kit != _keyBindings.end()) {
        for (auto key : kit->second)
            _keyToAction.erase(static_cast<int>(key));
        _keyBindings.erase(kit);
    }

    auto bit = _buttonBindings.find(a);
    if (bit != _buttonBindings.end()) {
        for (auto btn : bit->second)
            _buttonToAction.erase(static_cast<int>(btn));
        _buttonBindings.erase(bit);
    }
}

void InputManager::pushLayer(std::string_view layerName) {
    std::string name{layerName};
    for (const auto& l : _layerStack)
        if (l == name) return;
    _layerStack.push_back(std::move(name));
}

void InputManager::popLayer(std::string_view layerName) {
    std::string name{layerName};
    auto it = std::find(_layerStack.begin(), _layerStack.end(), name);
    if (it != _layerStack.end())
        _layerStack.erase(it);
}

std::string_view InputManager::currentLayer() const {
    if (_layerStack.empty()) return {};
    return _layerStack.back();
}

bool InputManager::hasLayer(std::string_view layerName) const {
    std::string name{layerName};
    return std::find(_layerStack.begin(), _layerStack.end(), name) != _layerStack.end();
}

void InputManager::on(std::string_view layerName,
                      std::string_view action,
                      ActionCallback   callback) {
    _actionCallbacks[std::string{layerName}][std::string{action}] = std::move(callback);
}

void InputManager::onAxis(std::string_view    layerName,
                          ax::Controller::Key axis,
                          AxisCallback        callback) {
    _axisCallbacks[std::string{layerName}][static_cast<int>(axis)] = std::move(callback);
}

void InputManager::dispatchAction(std::string_view action, InputEvent event) {
    if (_layerStack.empty()) return;

    const std::string& topLayer = _layerStack.back();

    auto layerIt = _actionCallbacks.find(topLayer);
    if (layerIt == _actionCallbacks.end()) return;

    auto cbIt = layerIt->second.find(std::string{action});
    if (cbIt == layerIt->second.end()) return;

    if (cbIt->second)
        cbIt->second(event);
}

void InputManager::dispatchAxis(ax::Controller::Key axis, float value) {
    if (_layerStack.empty()) return;

    const std::string& topLayer = _layerStack.back();

    auto layerIt = _axisCallbacks.find(topLayer);
    if (layerIt == _axisCallbacks.end()) return;

    auto cbIt = layerIt->second.find(static_cast<int>(axis));
    if (cbIt == layerIt->second.end()) return;

    if (cbIt->second)
        cbIt->second(value);
}

void InputManager::onControllerConnected(Controller* ctrl)
{
    _activeController  = ctrl;
    _controllerConnected = true;
    _controllerName    = std::string{ctrl->getDeviceName()};

    ControllerType newType = detectType(_controllerName);
    if (newType != _controllerType) {
        _controllerType = newType;
        if (onControllerTypeChanged)
            onControllerTypeChanged(_controllerType);
    }
}

void InputManager::onControllerDisconnected(Controller* ctrl) {
    if (_activeController == ctrl) {
        _activeController = nullptr;

        const auto& all = Controller::getAllController();
        for (auto* c : all) {
            if (c != ctrl && c->isConnected()) {
                onControllerConnected(c);
                return;
            }
        }

        _controllerConnected = false;
        _controllerName.clear();

        ControllerType newType = ControllerType::Unknown;
        if (newType != _controllerType) {
            _controllerType = newType;
            if (onControllerTypeChanged)
                onControllerTypeChanged(_controllerType);
        }
    }
}
