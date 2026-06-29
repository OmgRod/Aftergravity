#pragma once

#include "axmol.h"
#include "base/Controller.h"

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class ControllerType {
    Unknown,
    Xbox,
    PlayStation,
};

enum class InputEvent {
    Pressed,
    Released,
};

class InputManager {
public:
    using ActionCallback = std::function<void(InputEvent)>;
    using AxisCallback   = std::function<void(float value)>;

    static InputManager* getInstance();
    void init();
    void shutdown();
    void bindKey   (std::string_view action, ax::EventKeyboard::KeyCode key);
    void bindButton(std::string_view action, ax::Controller::Key        button);
    void unbindAll (std::string_view action);
    void             pushLayer   (std::string_view layerName);
    void             popLayer    (std::string_view layerName);
    std::string_view currentLayer() const;
    bool             hasLayer    (std::string_view layerName) const;
    void on(std::string_view layerName,
            std::string_view action,
            ActionCallback   callback);
    static constexpr float AXIS_DEADZONE = 0.1f;

    void onAxis(std::string_view layerName,
                ax::Controller::Key axis,
                AxisCallback        callback);

    std::function<void(ControllerType)> onControllerTypeChanged;
    ControllerType getControllerType()    const { return _controllerType; }
    bool           isControllerConnected() const { return _controllerConnected; }
    std::string_view getControllerName() const { return _controllerName; }

private:
    InputManager() = default;

    void dispatchAction(std::string_view action, InputEvent event);
    void dispatchAxis  (ax::Controller::Key axis, float value);

    static ControllerType detectType(std::string_view deviceName);

    void onControllerConnected   (ax::Controller* ctrl);
    void onControllerDisconnected(ax::Controller* ctrl);

    std::unordered_map<std::string, std::vector<ax::EventKeyboard::KeyCode>> _keyBindings;
    std::unordered_map<std::string, std::vector<ax::Controller::Key>>        _buttonBindings;

    std::unordered_map<int, std::string> _keyToAction;
    std::unordered_map<int, std::string> _buttonToAction;

    std::vector<std::string> _layerStack;
    std::unordered_map<std::string,
        std::unordered_map<std::string, ActionCallback>> _actionCallbacks;

    std::unordered_map<std::string,
        std::unordered_map<int, AxisCallback>> _axisCallbacks;

    ControllerType _controllerType      = ControllerType::Unknown;
    bool           _controllerConnected = false;
    std::string    _controllerName;

    ax::Controller* _activeController = nullptr;

    bool _initialised = false;

    static InputManager* s_instance;
};
