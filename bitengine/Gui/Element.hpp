#pragma once
#ifndef BIT_ELEMENT_H
#define BIT_ELEMENT_H

#include "SFML/Graphics.hpp"
#include <queue>
#include <list>
#include <functional>
#include "../Game/Game.hpp"
#include "Effect.hpp"

namespace bit
{
    class Draggable;
    class Hoverable;
    class InputManager;

    class Element : public sf::FloatRect
    {
    public:

        // Expose only these members of FloatRect to the public
        using sf::FloatRect::contains;
        using sf::FloatRect::intersects;

        enum AnchorType
        {
            TopLeft,
            Top,
            TopRight,
            Right,
            BottomRight,
            Bottom,
            BottomLeft,
            Left,
            Center
        };

        enum ScaleStyle
        {
            None,
            Smooth,
            PowerOfTwo,
        };

        Element();

        Element(float relativeX, float relativeY, float width, float height, AnchorType anchorType);

        Element(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

        virtual ~Element();

        // Positioning
        Element* parentElement;
        sf::Vector2f relativePosition;
        AnchorType anchorType;
        ScaleStyle scaleStyle;
        float targetWidth, targetHeight;
        float opacity;
        float elementScale;
        sf::RectangleShape debugRect; // debug

        // Input Activation
        bool isInfocus;
        bool canHaveFocus;
        std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput;
        std::function<void(Element*)> onActivate;

        // Blarb
        std::function<void(Element*, sf::RenderWindow*, sf::Time*)> onAfterUpdate;

        // Effects
        std::deque<Effect*> effectQueue;
        std::list<Effect*> concurrentEffects;

        // Helper properties for Containers
        bool hasPositioned;
        bool removeFromParent;
        bool transitFromParent;
        bit::Element* transferToParent;
        std::function<void(Element*)> onTransmit;

        // Manipulation
        Draggable* draggable;
        Hoverable* hoverable;

        static bool debugMode;

        virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual Element* queueEffect(Effect* effect);

        virtual Element* immediateEffect(Effect* effect);

        virtual Element* clearEffects();

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateInput(sf::RenderWindow &window, sf::Time &gameTime);

        virtual bool listenForInput(sf::RenderWindow &window, sf::Time &gameTime);

        float calculateViewRatio();

        void updateEffects(sf::RenderWindow &window, sf::Time &gameTime);

        sf::Vector2f calculateAnchor(sf::RenderWindow &window);

        void makeDraggable(InputManager*, std::function<void(Draggable*, bit::Element*)> onDragStart = NULL, std::function<bool(Draggable*, bit::Element*)> onDragStop = NULL, std::function<bool(Draggable*, bit::Element*)> checkDrag = NULL, bool centerOnMouse = false);

        void makeHoverable(InputManager*, std::function<void(Hoverable*, bit::Element*)> onHoverEnter = NULL, std::function<void(Hoverable*, bit::Element*)> onHoverLeave = NULL);
    };
}
#endif