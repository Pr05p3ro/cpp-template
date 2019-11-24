#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO

#include <stb_image.h>

#include "ResourceManager.h"

namespace game {

ResourceManager::ResourceManager(android_app *androidApp) :
    assetManager(androidApp->activity->assetManager) {

}

DecodedImage *ResourceManager::getImage(const std::string path) {
    DecodedImage *cachedImage = findImage(path);

    if (cachedImage) {
        return cachedImage;
    }

    return loadImage(path);
}

DecodedImage *ResourceManager::loadImage(std::string path) {
    AAsset *asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);

    if (asset) {
        auto size = AAsset_getLength(asset);
        auto data = new unsigned char[size];

        int32_t readSize = AAsset_read(asset, data, (size_t) size);

        if (readSize != size) {
            delete[] data;

            return nullptr;
        }

        auto img = std::make_unique<DecodedImage>();

        stbi_set_flip_vertically_on_load(1);

        img->path = path;
        img->data = stbi_load_from_memory(data, (int) size, &img->width, &img->height, &img->channels, 4);

        delete[] data;

        if (img->data == nullptr) {
            return nullptr;
        }

        DecodedImage* imgPtr = img.get();

        images.insert(std::make_pair(path, std::move(img)));

        return imgPtr;
    }

    return nullptr;
}


DecodedImage *ResourceManager::findImage(const std::string path) {
    auto it = images.find(path);

    if (it != images.end()) {
        return (*it).second.get();
    }

    return nullptr;
}

} // namespace game
