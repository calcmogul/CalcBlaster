//=============================================================================
//File Name: Main.cpp
//Description: Runs main game
//Author: Tyler Veness
//=============================================================================

#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Sleep.hpp>
#include <cmath>

#include "FriendlyShip.hpp"
#include "EnemyShip.hpp"
#include "Planet.hpp"
#include "ProgressBar.hpp"
#include "Bullet.hpp"

/* FIXME: Hitting Alt or F10 pauses game; hitting any key that has an ASCII
 *        equivalent resumes it. The window loses focus during this event and
 *        regains it while resuming.
 */

int main() {
    sf::RenderWindow mainWin( sf::VideoMode( 800 , 600 )  , "Calc Blaster" , sf::Style::Resize | sf::Style::Close );
    mainWin.setMouseCursorVisible( false );
    mainWin.setVerticalSyncEnabled( true );

    sf::Image appIcon;
    if ( !appIcon.loadFromFile( "Resources/GalagaShip.png" ) ) {
        exit( 1 );
    }
    mainWin.setIcon( appIcon.getSize().x , appIcon.getSize().y , appIcon.getPixelsPtr() );

    sf::Texture backgroundTexture;

    if ( !backgroundTexture.loadFromFile( "Resources/SpaceBackground.png" ) ) {
        exit( 1 );
    }

    backgroundTexture.setRepeated( true ); // Tiling background

    sf::Sprite backgroundSprite( backgroundTexture );
    backgroundSprite.setTextureRect( sf::IntRect( -86 , -86 , mainWin.getSize().x + 2 * 86 , mainWin.getSize().y + 2 * 86 ) );
    backgroundSprite.setPosition( 0.f , 0.f );

    sf::RectangleShape HUDBackground( sf::Vector2f( mainWin.getSize().x , 45.f ) );
    HUDBackground.setFillColor( sf::Color( 90 , 90 , 90 , 170 ) );

    FriendlyShip myShip( sf::Vector2f( 0.f , 300.f ) , 100.f );
    ProgressBar healthBar( sf::Vector2f( 100.f , 19.f ) , "Health" , sf::Color( 120 , 0 , 0 ) , sf::Color( 40 , 40 , 40 ) , sf::Color( 50 , 50 , 50 ) );

    EnemyShip::add( sf::Vector2f( -100.f , 200.f ) , 50.f );
    EnemyShip::add( sf::Vector2f( 100.f , 200.f ) , 50.f );
    EnemyShip::add( sf::Vector2f( -100.f , 500.f ) , 50.f );
    EnemyShip::add( sf::Vector2f( 100.f , 500.f ) , 50.f );

    // Used to make background move past at 10 m/s
    sf::RectangleShape backShape( sf::Vector2f( 0.f , 0.f ) );
    Box2DBase backBody( &backShape , sf::Vector2f( 0.f , 300.f ) , b2_kinematicBody );
    backBody.body->SetLinearVelocity( b2Vec2( 0.f , 10.f ) );

    // Prepare for simulation. Typically we use a time step of 1/60 of a
    // second (60Hz) and 10 iterations. This provides a high quality simulation
    // in most game scenarios.
    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 1; //6
    int32 positionIterations = 1; //2

    mainWin.setView( sf::View( sf::FloatRect( backBody.drawShape->getPosition().x - mainWin.getSize().x / 2 , backBody.drawShape->getPosition().y - mainWin.getSize().y / 2 , mainWin.getSize().x , mainWin.getSize().y ) ) );

    sf::Event event;
    sf::Clock shootClock;

    bool isPaused = false;

    //backgroundMusic.play();
    while ( mainWin.isOpen() ) {
        while ( mainWin.pollEvent( event ) ) {
            if ( event.type == sf::Event::Closed ) {
                mainWin.close();
            }
            else if ( event.type == sf::Event::Resized ) {
                backgroundSprite.setTextureRect( sf::IntRect( -86.f , -86.f , mainWin.getSize().x + 2.f * 86.f , mainWin.getSize().y + 2.f * 86.f ) );

                HUDBackground.setScale( mainWin.getSize().x / 50.f , HUDBackground.getScale().y );
            }

            else if ( event.type == sf::Event::KeyReleased ) {
                if ( event.key.code == sf::Keyboard::Return ) {
                    isPaused = !isPaused;
                }
            }
        }

        if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Escape ) ) {
            mainWin.close();
        }

        if ( !isPaused ) {
            // Instruct the world to perform a single step of simulation.
            // It is generally best to keep the time step and iterations fixed.
            Box2DBase::world.Step( timeStep , velocityIterations , positionIterations );

            myShip.syncObject( mainWin );
            backBody.syncObject( mainWin );
            Planet::syncObjects( mainWin );
            EnemyShip::syncObjects( mainWin );
            Bullet::syncObjects( mainWin );

            Planet::applyUnivGravity();
            Bullet::checkCollisions( myShip , mainWin );

            myShip.controlShip( NULL );
            EnemyShip::controlShips( &myShip );

            /* ===== Keep main ship within boundaries of window ==== */
            sf::Vector2f myPos = myShip.drawShape->getPosition();
            sf::View myView = mainWin.getView();

            // Limit left edge
            if ( myPos.x < myView.getCenter().x - myView.getSize().x / 2.f ) {
                myPos.x = myView.getCenter().x - myView.getSize().x / 2.f;
            }

            // Limit right edge
            if ( myPos.x > myView.getCenter().x + myView.getSize().x / 2.f ) {
                myPos.x = myView.getCenter().x + myView.getSize().x / 2.f;
            }

            // Limit top edge
            if ( myPos.y < myView.getCenter().y - myView.getSize().y / 2.f ) {
                myPos.y = myView.getCenter().y - myView.getSize().y / 2.f;
            }

            // Limit bottom edge
            if ( myPos.y > myView.getCenter().y + myView.getSize().y / 2.f ) {
                myPos.y = myView.getCenter().y + myView.getSize().y / 2.f;
            }

            // Apply limits
            myShip.body->SetTransform( SFMLToBox( myPos.x , myPos.y , mainWin.getSize().y ) , myShip.body->GetAngle() );
            /* ===================================================== */

            if ( sf::Keyboard::isKeyPressed( sf::Keyboard::J ) && shootClock.getElapsedTime().asMilliseconds() > 150 ) {
                Bullet::add( myShip , mainWin , sf::Color( 255 , 255 , 0 ) );
                for ( unsigned int index = 0 ; index < 4 ; index++ ) {
                    EnemyShip::addBullet( index , mainWin );
                }
                shootClock.restart();
            }

            /* ===== Handle background texture shifting with ship ===== */
            // Move background left
            if ( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x < mainWin.getSize().x / 2 + 86.f ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x - 86.f * ( std::ceil( std::fabs( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x - mainWin.getSize().x / 2 - 86.f ) / 86.f ) ) , backgroundSprite.getPosition().y );
            }

            // Move background right
            if ( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x > mainWin.getSize().x / 2 + 86.f ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x + 86.f * ( std::ceil( std::fabs( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x - mainWin.getSize().x / 2 - 86.f ) / 86.f ) ) , backgroundSprite.getPosition().y );
            }

            // Move background up
            if ( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y < mainWin.getSize().y / 2 + 86.f ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x , backgroundSprite.getPosition().y - 86.f * ( std::ceil( std::fabs( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y - mainWin.getSize().y / 2 - 86.f ) / 86.f ) ) );
            }

            // Move background down
            if ( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y > mainWin.getSize().y / 2 + 86.f ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x , backgroundSprite.getPosition().y + 86.f * ( std::ceil( std::fabs( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y - mainWin.getSize().y / 2 - 86.f ) / 86.f ) ) );
            }
            /* ======================================================== */

            healthBar.setPercent( myShip.getHealth() );
            //healthBar.shader.setParameter( "currentPos" , healthBar.getPosition().x - myShip.shape.getPosition().x + mainWin.getSize().x / 2.f , myShip.shape.getPosition().y - healthBar.getPosition().y + mainWin.getSize().y / 2.f );
            healthBar.shader.setParameter( "currentPos" , 6.f , mainWin.getSize().y - 6.f );
        }

        HUDBackground.setPosition( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f );
        healthBar.setPosition( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f + 6.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f + 6.f );

        mainWin.clear( sf::Color( 10 , 10 , 10 ) );

        mainWin.draw( backgroundSprite );
        Planet::drawAll( myShip , mainWin );
        EnemyShip::drawAll( myShip , mainWin );
        mainWin.draw( myShip );
        Bullet::drawAll( mainWin );

        mainWin.draw( HUDBackground );
        mainWin.draw( healthBar );

        mainWin.display();

        mainWin.setView( sf::View( sf::FloatRect( backBody.drawShape->getPosition().x - mainWin.getSize().x / 2 , backBody.drawShape->getPosition().y - mainWin.getSize().y / 2 , mainWin.getSize().x , mainWin.getSize().y ) ) );
    }

    Planet::cleanup();
    EnemyShip::cleanup();
    Bullet::cleanup();

    return 0;
}
