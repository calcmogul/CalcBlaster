//=============================================================================
//File Name: Bullet.hpp
//Description: Handles all bullets created by entities
//Author: Tyler Veness
//=============================================================================

#ifndef BULLET_HPP
#define BULLET_HPP

#include "Box2DBase.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

class ShipBase;

class Bullet : public Box2DBase {
public:
    enum BulletType {
        zero = 0,
        constant,
        infinity,
        size
    };

    virtual ~Bullet();

    // Call this at end of program to free all bullets and avoid memory leaks
    static void cleanup();

    static void drawAll( sf::RenderTarget& target , sf::RenderStates states = sf::RenderStates::Default );
    static void syncObjects( const sf::Window& referTo );

    static void add( const ShipBase& ship , const sf::Window& referTo , const sf::Color& color , BulletType type );

    // This function may change the referenced ship's health
    static void checkCollisions( ShipBase& ship , const sf::RenderWindow& referTo ); // checks if bullet collided with other object or left the screen to destroy it

    const b2Body* getSourceBody() const;

    BulletType getType() const;

protected:
    static std::vector<Bullet*> m_bullets;

private:
    Bullet( const ShipBase& ship , const sf::Window& referTo , const sf::Color& color , BulletType type );

    static sf::Texture m_textures[Bullet::size];
    static sf::Vector2u m_sizes[Bullet::size];
    static bool m_isLoaded;

    sf::RectangleShape shape;

    // FIXME If source body dies before bullet despawns, undefined behavior results
    const b2Body* const m_sourceBody;

    BulletType m_type;
};

#endif // BULLET_HPP
