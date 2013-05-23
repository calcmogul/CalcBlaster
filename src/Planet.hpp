//=============================================================================
//File Name: Planet.hpp
//Description: Handles creating planets of different colors in world
//Author: Tyler Veness
//=============================================================================

#ifndef PLANET_HPP
#define PLANET_HPP

#include "Box2DBase.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Shader.hpp>

class ShipBase;

class Planet : public Box2DBase {
public:
    virtual ~Planet();

    // Call this at end of program to free all planets and avoid memory leaks
    static void cleanup();

    static void drawAll( const ShipBase& ship , sf::RenderTarget& target , sf::RenderStates states = sf::RenderStates::Default );
    static void syncObjects( const sf::Window& referTo );

    static void add( const sf::Vector2f& position , const float32& radius , const sf::Color& color );
    static void applyUnivGravity();

    sf::CircleShape shape;
    sf::Shader shader;

private:
    Planet( const sf::Vector2f& position , const float32& radius , const sf::Color& color );

    static std::vector<Planet*> m_planets;

    sf::RenderStates m_shaderState;
};

#endif // PLANET_HPP
