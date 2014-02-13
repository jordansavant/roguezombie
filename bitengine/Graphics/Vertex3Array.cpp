#include "Vertex3Array.h"
#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include "Vertex3.h"

bit::Vertex3Array::Vertex3Array()
    : m_vertices(), m_primitiveType(sf::Points)
{
}

bit::Vertex3Array::Vertex3Array(sf::PrimitiveType type, unsigned int vertexCount)
    : m_vertices(vertexCount), m_primitiveType(type)
{
}

unsigned int bit::Vertex3Array::getVertexCount() const
{
    return static_cast<unsigned int>(m_vertices.size());
}

bit::Vertex3& bit::Vertex3Array::operator[](unsigned int index)
{
    return m_vertices[index];
}

const bit::Vertex3& bit::Vertex3Array::operator[](unsigned int index) const
{
    return m_vertices[index];
}

void bit::Vertex3Array::clear()
{
    m_vertices.clear();
}

unsigned int bit::Vertex3Array::size()
{
    return m_vertices.size();
}

void bit::Vertex3Array::resize(unsigned int vertexCount)
{
    m_vertices.resize(vertexCount);
}

void bit::Vertex3Array::resize(unsigned int vertexCount, Vertex3 &val)
{
    m_vertices.resize(vertexCount, val);
}

void bit::Vertex3Array::append(const Vertex3& vertex)
{
    m_vertices.push_back(vertex);
}

void bit::Vertex3Array::setPrimitiveType(sf::PrimitiveType type)
{
    m_primitiveType = type;
}

sf::PrimitiveType bit::Vertex3Array::getPrimitiveType() const
{
    return m_primitiveType;
}

sf::FloatRect bit::Vertex3Array::getBounds() const
{
    if (!m_vertices.empty())
    {
        float left   = m_vertices[0].position.x;
        float top    = m_vertices[0].position.y;
        float right  = m_vertices[0].position.x;
        float bottom = m_vertices[0].position.y;

        for (std::size_t i = 1; i < m_vertices.size(); ++i)
        {
            sf::Vector3f position = m_vertices[i].position;

            // Update left and right
            if (position.x < left)
                left = position.x;
            else if (position.x > right)
                right = position.x;

            // Update top and bottom
            if (position.y < top)
                top = position.y;
            else if (position.y > bottom)
                bottom = position.y;
        }

        return sf::FloatRect(left, top, right - left, bottom - top);
    }
    else
    {
        // Array is empty
        return sf::FloatRect();
    }
}

// Re-implementation of sf::RenderTarget::draw() to allow Depth Testing
void bit::Vertex3Array::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // If no vertices, do not render
    if(m_vertices.empty())
        return;

    // Get vertices data
    const Vertex3* vertices = &m_vertices[0];
    unsigned int vertexCount = static_cast<unsigned int>(m_vertices.size());

    // Apply the transform
    glLoadMatrixf(states.transform.getMatrix());

    // Apply the view
    applyCurrentView(target);

    // Apply the blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Apply the texture
    sf::Texture::bind(states.texture, sf::Texture::Pixels);

    // Apply the shader
    if (states.shader)
        sf::Shader::bind(states.shader);

    // Setup the pointers to the vertices' components
    if (vertices)
    {
        const char* data = reinterpret_cast<const char*>(vertices);
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex3), data + 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3), data + 12);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3), data + 16);
    }

    // Find the OpenGL primitive type
    static const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
    GLenum mode = modes[m_primitiveType];

    // Draw the primitives
    glDrawArrays(mode, 0, vertexCount);

    // Unbind the shader, if any
    if (states.shader)
        sf::Shader::bind(NULL);
}

void bit::Vertex3Array::applyCurrentView(sf::RenderTarget& target) const
{
    // Set the viewport
    sf::IntRect viewport = getViewport(target, target.getView());
    int top = target.getSize().y - (viewport.top + viewport.height);
    glViewport(viewport.left, top, viewport.width, viewport.height);

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(target.getView().getTransform().getMatrix());

    // Go back to model-view mode
    glMatrixMode(GL_MODELVIEW);
}

sf::IntRect bit::Vertex3Array::getViewport(sf::RenderTarget& target, const sf::View& view) const
{
    float width  = static_cast<float>(target.getSize().x);
    float height = static_cast<float>(target.getSize().y);
    const sf::FloatRect& viewport = view.getViewport();

    return sf::IntRect(
        static_cast<int>(0.5f + width  * viewport.left),
        static_cast<int>(0.5f + height * viewport.top),
        static_cast<int>(width  * viewport.width),
        static_cast<int>(height * viewport.height));
}
