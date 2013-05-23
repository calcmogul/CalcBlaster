//=============================================================================
//File Name: Bullet.hpp
//Description: Handles all bullets created by entities
//Author: Tyler Veness
//=============================================================================

#ifndef BULLET_HPP
#define BULLET_HPP

#include "Box2DBase.hpp"
#include <SFML/Graphics/RectangleShape.hpp>

class ShipBase;

class Bullet : public Box2DBase {
public:
    virtual ~Bullet();

    // Call this at end of program to free all bullets and avoid memory leaks
    static void cleanup();

    static void drawAll( sf::RenderTarget& target , sf::RenderStates states = sf::RenderStates::Default );
    static void syncObjects( const sf::Window& referTo );

    static void add( const ShipBase& ship , const sf::Window& referTo , const sf::Color& color );
    static void checkCollisions( const ShipBase& ship , const sf::Window& referTo ); // checks if bullet collided with other object or left the screen to destroy it

    sf::RectangleShape shape;

protected:
    static std::vector<Bullet*> bullets;

private:
    Bullet( const ShipBase& ship , const sf::Window& referTo , const sf::Color& color );
};

#endif // BULLET_HPP
