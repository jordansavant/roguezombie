#include "SpriteLoader.hpp"
#include "SFML/Graphics.hpp"
#include "../Game/Game.hpp"
#include "../Game/GameComponent.hpp"
#include "../Math/Math.hpp"
#include "../Structures/Exception.hpp"
#include "Sprite.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

bit::SpriteLoader::SpriteLoader(bit::Game* _game)
    : bit::GameComponent(_game)
{
}

bit::SpriteLoader::~SpriteLoader()
{
    for (std::map<std::string, Sprite*>::reverse_iterator iter = sprites.rbegin(); iter != sprites.rend(); ++iter)
    {
        delete iter->second;
    }
}

void bit::SpriteLoader::loadSprites(std::string loadDescriptorFile)
{
    // If this file has already been loaded
    if(std::find(loadedDescriptorFiles.begin(), loadedDescriptorFiles.end(), loadDescriptorFile) != loadedDescriptorFiles.end())
    {
        return;
    }

    loadedDescriptorFiles.push_back(loadDescriptorFile);
    std::ifstream loadDescriptor(loadDescriptorFile);

    if(!loadDescriptor.good())
    {
        throw bit::Exception(loadDescriptorFile + " sprite descriptor not found.");
    }

    std::string line;
    int i=0;
    while (loadDescriptor.good())
    {
        std::string value;

        i++;
        std::getline(loadDescriptor, line, '\n');
        if(i == 1) // header
            continue;
        if(line == "") // blank line
            continue;

        std::stringstream linestream(line);
        std::string currentSprite;
        int j=0;
        while (linestream.good())
        {
            j++;
            std::getline(linestream, value, ',');

            switch(j)
            {
                // Sprite Name
                case 1:
                {
                    if(sprites.find(value) != sprites.end())
                    {
                        throw std::string("Duplicate sprite name");
                    }

                    currentSprite = value;
                    sprites.insert(std::pair<std::string, Sprite*>(value, new Sprite()));

                    break;
                }

                // X in Texture
                case 2:
                {
                    sprites.at(currentSprite)->left = std::atoi(value.c_str());
                    break;
                }

                // Y in Texture
                case 3:
                {
                    sprites.at(currentSprite)->top = std::atoi(value.c_str());
                    break;
                }

                // Width
                case 4:
                {
                    sprites.at(currentSprite)->width = std::atoi(value.c_str());
                    break;
                }

                // Height
                case 5:
                {
                    sprites.at(currentSprite)->height = std::atoi(value.c_str());
                    break;
                }

                default: break;
            }
        }
    }
}

bit::Sprite* bit::SpriteLoader::getSprite(std::string spriteName)
{
    return sprites.at(spriteName);
}