#include <axmol.h>

using namespace ax;

class CreditsLayer : public Layer {
    bool init();

    void onBack(Object* sender);
    void keyBackClicked();

public:
    CREATE_FUNC(CreditsLayer);
    static Scene* scene();
};