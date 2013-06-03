//=============================================================================
//File Name: EnemyFormula.hpp
//Description: Handles actions/AI and rendering of enemy formulas
//Author: Tyler Veness
//=============================================================================

#ifndef ENEMY_FORMULA_HPP
#define ENEMY_FORMULA_HPP

#include "ShipBase.hpp"
#include "Bullet.hpp"
#include <vector>
#include <map>
#include <string>

class EnemyFormula;

struct userData {
    Bullet::BulletType formulaType;

    EnemyFormula* formulaObj;
};

class EnemyFormula : public ShipBase {
public:
    EnemyFormula( const sf::Vector2f& position , b2Vec2 speed );
    virtual ~EnemyFormula();

    // Call this at end of program to free all planets and avoid memory leaks
    static void cleanup();

    static void drawAll( const ShipBase& ship , sf::RenderTarget& target , sf::RenderStates states = sf::RenderStates::Default );
    static void syncObjects( const sf::Window& referTo );

    static void add( const sf::Vector2f& position , b2Vec2 speed );
    static void addBullet( unsigned int index , const sf::Window& referTo );

    static void controlEnemies( void* userData );

private:
    static bool m_isLoaded;

    static std::vector<EnemyFormula*> m_enemyFormulas;

    // Contains textures for images of all formulas
    static std::vector<sf::Texture*> m_textures;

    // Contains sizes of images which are contained by the textures
    static std::vector<sf::Vector2u> m_sizes;

    static std::map<std::string , std::string> m_limits;

    sf::Texture* m_formulaTexture;

    b2Vec2 m_speed;

    // Bullets use this data to determine what to do with bodies they hit
    userData m_userData;

    void controlShip( void* userData );
};

#endif // ENEMY_FORMULA_HPP
