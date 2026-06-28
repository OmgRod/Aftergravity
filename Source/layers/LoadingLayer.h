#include <axmol.h>

using namespace ax;

class LoadingLayer : public Layer {
    bool init();
    void loadAssets();

    Label* m_loadingLabel;
public:
    CREATE_FUNC(LoadingLayer);
    static Scene* scene();
};
