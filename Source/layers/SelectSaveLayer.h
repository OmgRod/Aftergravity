#include <axmol.h>

using namespace ax;

class SelectSaveLayer : public Layer {
    bool init();

    void onBack(Object* sender);
    void keyBackClicked();

public:
    CREATE_FUNC(SelectSaveLayer);
    static Scene* scene();
};