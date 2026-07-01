#include <axmol.h>

using namespace ax;

class SettingsLayer : public Layer {
    bool init();

    void onBack(Object* sender);
    void keyBackClicked();

public:
    CREATE_FUNC(SettingsLayer);
    static Scene* scene();
};