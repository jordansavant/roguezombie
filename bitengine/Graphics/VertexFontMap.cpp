#include "VertexFontMap.hpp"
#include "Sprite.hpp"

bit::VertexFontMap::VertexFontMap()
{
}

void bit::VertexFontMap::load(sf::Font* _font)
{
    // Characters
    alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    symbol = " \n!@#$%^&*()_+-=[]\{}|;':\",./<>~`";
    full = alpha + symbol;

    // Load font
    font = _font;
    text.setFont(*font);
    text.setCharacterSize(42);
    text.setString(full);

    // Copy texture
    texture = font->getTexture(42);
    texture.setSmooth(true);

    // Load the vertext map
    vertexMap.load(&texture, sf::PrimitiveType::Quads);
}

void bit::VertexFontMap::applyCharacter(Vertex3* vertex, std::string &character)
{
    sf::Glyph g = font->getGlyph(66, 42, false);
    bit::Sprite s(g.textureRect.left, g.textureRect.top, g.textureRect.width, g.textureRect.height);
    s.applyToQuad(vertex);
}