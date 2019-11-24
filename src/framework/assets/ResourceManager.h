#ifndef CPP_TEMPLATE_RESOURCE_H
#define CPP_TEMPLATE_RESOURCE_H

#include <android_native_app_glue.h>
#include <vector>
#include <string>
#include <GLES2/gl2.h>
#include <unordered_map>
#include "../Types.h"

namespace game {

class ResourceManager {
public:
    explicit ResourceManager(android_app *androidApp);

    DecodedImage *getImage(std::string path);

    DecodedImage *loadImage(std::string path);

private:
    DecodedImage *findImage(std::string path);

    AAssetManager *assetManager;

    std::unordered_map<std::string, std::unique_ptr<DecodedImage>> images;
};

} // namespace game

#endif //CPP_TEMPLATE_RESOURCE_H
