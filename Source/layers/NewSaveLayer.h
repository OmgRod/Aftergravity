#include <axmol.h>

using namespace ax;

class NewSaveLayer : public Layer {
    bool init();

    void onBack(Object* sender);
    void keyBackClicked();

public:
    CREATE_FUNC(NewSaveLayer);
    static Scene* scene();
};