#pragma once
#ifndef BIT_SPRITELOADER_H
#define BIT_SPRITELOADER_H

#include "../Game/GameComponent.hpp"

namespace bit
{
    class Sprite;

    class SpriteLoader : public GameComponent
    {
    public:
        SpriteLoader();

        ~SpriteLoader();

        std::map<std::string, Sprite*> sprites;
        std::vector<std::string> loadedDescriptorFiles;

        void loadSprites(std::string loadDescriptorFile);

        Sprite* getSprite(std::string spriteName);
    };
}

#endif