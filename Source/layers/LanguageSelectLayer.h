#include <axmol.h>
#include "../managers/LanguageManager.h"

using namespace ax;

class LanguageSelectLayer : public Layer {
    bool init();
public:
    CREATE_FUNC(LanguageSelectLayer);
    static Scene* scene();
};
