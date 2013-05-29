//=============================================================================
//File Name: Main.cpp
//Description: Runs main game
//Author: Tyler Veness
//=============================================================================

#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Sleep.hpp>
#include <cmath>

#include "UIFont.hpp"
#include "FriendlyShip.hpp"
#include "EnemyFormula.hpp"
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
    sf::Vector2f backSize;

    if ( !backgroundTexture.loadFromFile( "Resources/PaperBackground.png" ) ) {
        exit( 1 );
    }

    backSize.x = backgroundTexture.getSize().x;
    backSize.y = backgroundTexture.getSize().y;

    backgroundTexture.setRepeated( true ); // Tiling background

    sf::Sprite backgroundSprite( backgroundTexture );
    backgroundSprite.setTextureRect( sf::IntRect( -backSize.x , -backSize.y , mainWin.getSize().x + 2 * backSize.x , mainWin.getSize().y + 2 * backSize.y ) );
    backgroundSprite.setPosition( 0.f , 0.f );

    sf::RectangleShape HUDBackground( sf::Vector2f( mainWin.getSize().x , 45.f ) );
    HUDBackground.setFillColor( sf::Color( 90 , 90 , 90 , 170 ) );

    FriendlyShip myShip( sf::Vector2f( 0.f , 200.f ) , 300.f );

    /* ===== Create health bar sprite ===== */
    sf::Texture healthTexture;
    healthTexture.loadFromImage( appIcon );
    healthTexture.setRepeated( true );

    sf::Sprite healthSprite( healthTexture );
    healthSprite.setTextureRect( sf::IntRect( 0 , 0 , healthTexture.getSize().x * myShip.getHealth() / 100.f , healthTexture.getSize().y ) );
    healthSprite.setPosition( 0.f , 0.f );
    /* ==================================== */

    sf::Vector2f winSize;
    winSize.x = mainWin.getSize().x;
    winSize.y = mainWin.getSize().y;

    /* ===== Create background body ===== */
    // Offset of Box2D world origin from top-left corner of window
    //sf::Vector2f worldOrgn( 0.f , 0.f );
    sf::Vector2f worldOrgn( winSize.x , winSize.y );

    // Used to make background move past at 10 m/s
    sf::RectangleShape backShape( sf::Vector2f( 0.f , 0.f ) );
    backShape.setFillColor( sf::Color( 0.f , 0.f , 0.f ) );

    Box2DBase backBody( &backShape , sf::Vector2f( 0.f , 0.f ) , b2_dynamicBody );

    b2PolygonShape box;

    // Coordinate of top-left of box
    sf::Vector2f boxTL;
    boxTL.x = -winSize.x / 2.f;
    boxTL.y = winSize.y / 2.f;

    // Coordinate of bottom-right of box
    sf::Vector2f boxBR;
    boxBR.x = winSize.x / 2.f;
    boxBR.y = winSize.y / 2.f + 20.f;

    b2Vec2 boxVerts[4];
    boxVerts[0].Set( SFMLToBox_x( boxTL.x - worldOrgn.x ) , SFMLToBox_y( boxTL.y - worldOrgn.y , winSize.y ) );
    boxVerts[1].Set( SFMLToBox_x( boxBR.x - worldOrgn.x ) , SFMLToBox_y( boxTL.y - worldOrgn.y , winSize.y ) );
    boxVerts[2].Set( SFMLToBox_x( boxBR.x - worldOrgn.x ) , SFMLToBox_y( boxBR.y - worldOrgn.y , winSize.y ) );
    boxVerts[3].Set( SFMLToBox_x( boxTL.x - worldOrgn.x ) , SFMLToBox_y( boxBR.y - worldOrgn.y , winSize.y ) );

    box.Set( boxVerts , 4 );

    // Create fixture before attaching it
    b2FixtureDef backFixture;
    backFixture.shape = &box;
    backFixture.friction = 0.f;
    backFixture.restitution = 0.f;
    backFixture.density = 0.f;

    // Add the shape to the body.
    backBody.body->CreateFixture( &backFixture );

    b2MassData backMass;
    backBody.body->GetMassData( &backMass );
    backMass.mass = 1000000.f;
    backBody.body->SetMassData( &backMass );

    backShape.setSize( boxBR - boxTL );
    backShape.setOrigin( backShape.getSize() / 2.f );

    // Make edge body move at 10 m/s up
    backBody.body->SetLinearVelocity( b2Vec2( 0.f , 10.f ) );
    /* ================================== */

    /* ===== Create pause graphic ===== */
    sf::RenderTexture pauseTexture;
    pauseTexture.create( 400 , 300 );

    sf::RectangleShape pauseRect( sf::Vector2f( 400 , 300 ) );
    pauseRect.setFillColor( sf::Color( 90 , 90 , 90 , 170 ) );

    sf::Text pauseText( "PAUSED" , UIFont::getInstance()->arial() , 50 );
    pauseText.setPosition( sf::Vector2f( (pauseRect.getSize().x - pauseText.findCharacterPos( 7 ).x) / 2.f , (pauseRect.getSize().y - pauseText.getCharacterSize()) / 2.f ) );
    pauseText.setColor( sf::Color( 255 , 255 , 255 ) );

    pauseTexture.draw( pauseRect );
    pauseTexture.draw( pauseText );
    pauseTexture.display();

    sf::Sprite pauseSprite(  pauseTexture.getTexture() );
    /* ================================ */

    // Used to limit framerate of simulation
    sf::Clock simTime;

    // Prepare for simulation. Typically we use a time step of 1/60 of a
    // second (30Hz) and 10 iterations. This provides a high quality simulation
    // in most game scenarios.
    float32 timeStep = 1.0f / 30.0f;
    int32 velocityIterations = 64; //8
    int32 positionIterations = 24; //3

    mainWin.setView( sf::View( sf::FloatRect( backBody.drawShape->getPosition().x - mainWin.getSize().x / 2 , backBody.drawShape->getPosition().y - mainWin.getSize().y / 2 , mainWin.getSize().x , mainWin.getSize().y ) ) );

    sf::Event event;
    sf::Clock shootClock;

    sf::Clock enemySpawnClock;

    bool isPaused = false;

    //backgroundMusic.play();
    while ( mainWin.isOpen() ) {
        if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Escape ) ) {
            mainWin.close();
        }

        // If game isn't paused and the frame rate is at or below 30Hz
        if ( !isPaused && simTime.getElapsedTime().asSeconds() > 1.f / 30.f ) {
            // Instruct the world to perform a single step of simulation.
            // It is generally best to keep the time step and iterations fixed.
            Box2DBase::world.Step( timeStep , velocityIterations , positionIterations );

            myShip.syncObject( mainWin );
            backBody.syncObject( mainWin );
            EnemyFormula::syncObjects( mainWin );
            Bullet::syncObjects( mainWin );

            Bullet::checkCollisions( myShip , mainWin );

            myShip.controlShip( NULL );
            EnemyFormula::controlEnemies( &myShip );

            /* ===== Keep main ship within boundaries of window ==== */
            sf::Vector2f myPos = myShip.drawShape->getPosition();
            sf::View myView = mainWin.getView();

            // Limit left edge
            if ( myPos.x - healthTexture.getSize().x < myView.getCenter().x - myView.getSize().x / 2.f ) {
                myPos.x = myView.getCenter().x - myView.getSize().x / 2.f + healthTexture.getSize().x;
            }

            // Limit right edge
            if ( myPos.x + healthTexture.getSize().x > myView.getCenter().x + myView.getSize().x / 2.f ) {
                myPos.x = myView.getCenter().x + myView.getSize().x / 2.f - healthTexture.getSize().x;
            }

            // Limit top edge
            if ( myPos.y - healthTexture.getSize().x < myView.getCenter().y - myView.getSize().y / 2.f ) {
                myPos.y = myView.getCenter().y - myView.getSize().y / 2.f + healthTexture.getSize().x;
            }

            // Limit bottom edge
            if ( myPos.y + healthTexture.getSize().x > myView.getCenter().y + myView.getSize().y / 2.f ) {
                myPos.y = myView.getCenter().y + myView.getSize().y / 2.f - healthTexture.getSize().x;
            }

            // Apply limits
            myShip.body->SetTransform( SFMLToBox( myPos.x , myPos.y , mainWin.getSize().y ) , myShip.body->GetAngle() );
            /* ===================================================== */

            if ( shootClock.getElapsedTime().asMilliseconds() > 250 ) {
                if ( sf::Keyboard::isKeyPressed( sf::Keyboard::J ) ) {
                    Bullet::add( myShip , mainWin , sf::Color( 255 , 0 , 0 ) , Bullet::infinity );

                    shootClock.restart();
                }

                else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::K ) ) {
                    Bullet::add( myShip , mainWin , sf::Color( 0 , 0 , 255 ) , Bullet::constant );

                    shootClock.restart();
                }

                else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::L ) ) {
                    Bullet::add( myShip , mainWin , sf::Color( 0 , 0 , 0 ) , Bullet::zero );

                    shootClock.restart();
                }
            }

            if ( enemySpawnClock.getElapsedTime().asMilliseconds() > 2000 ) {
                EnemyFormula::add( sf::Vector2f( rand() % mainWin.getSize().x + mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f ) , b2Vec2( 0.f , 7.f ) );

                enemySpawnClock.restart();
            }

            /* ===== Handle background texture shifting with ship ===== */
            // Move background left
            if ( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x < mainWin.getSize().x / 2 + backSize.x ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x - backSize.x * ( std::ceil( std::fabs( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x - mainWin.getSize().x / 2 - backSize.x ) / backSize.x ) ) , backgroundSprite.getPosition().y );
            }

            // Move background right
            if ( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x > mainWin.getSize().x / 2 + backSize.x ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x + backSize.x * ( std::ceil( std::fabs( backBody.drawShape->getPosition().x - backgroundSprite.getPosition().x - mainWin.getSize().x / 2 - backSize.x ) / backSize.x ) ) , backgroundSprite.getPosition().y );
            }

            // Move background up
            if ( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y < mainWin.getSize().y / 2 + backSize.y ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x , backgroundSprite.getPosition().y - backSize.y * ( std::ceil( std::fabs( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y - mainWin.getSize().y / 2 - backSize.y ) / backSize.y ) ) );
            }

            // Move background down
            if ( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y > mainWin.getSize().y / 2 + backSize.y ) {
                backgroundSprite.setPosition( backgroundSprite.getPosition().x , backgroundSprite.getPosition().y + backSize.y * ( std::ceil( std::fabs( backBody.drawShape->getPosition().y - backgroundSprite.getPosition().y - mainWin.getSize().y / 2 - backSize.y ) / backSize.y ) ) );
            }
            /* ======================================================== */

            healthSprite.setTextureRect( sf::IntRect( 0 , 0 , healthTexture.getSize().x * myShip.getHealth() / 100.f , healthTexture.getSize().y ) );
        }

        HUDBackground.setPosition( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y + mainWin.getSize().y / 2.f - HUDBackground.getSize().y );
        healthSprite.setPosition( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f + 6.f , mainWin.getView().getCenter().y + mainWin.getSize().y / 2.f + 6.f - HUDBackground.getSize().y );

        mainWin.clear( sf::Color( 10 , 10 , 10 ) );

        mainWin.draw( backgroundSprite );
        mainWin.draw( *(backBody.drawShape) );
        EnemyFormula::drawAll( myShip , mainWin );
        mainWin.draw( myShip );
        Bullet::drawAll( mainWin );

        mainWin.draw( HUDBackground );
        mainWin.draw( healthSprite );

        // Draw pause graphic over everything if paused
        if ( isPaused ) {
            pauseSprite.setPosition( sf::Vector2f( mainWin.getView().getCenter().x - pauseTexture.getSize().x / 2.f , mainWin.getView().getCenter().y - pauseTexture.getSize().y / 2.f ) );
            mainWin.draw( pauseSprite );
        }

        mainWin.display();

        mainWin.setView( sf::View( sf::FloatRect( backBody.drawShape->getPosition().x - mainWin.getSize().x / 2 , backBody.drawShape->getPosition().y - mainWin.getSize().y / 2 , mainWin.getSize().x , mainWin.getSize().y ) ) );

        while ( mainWin.pollEvent( event ) ) {
            if ( event.type == sf::Event::Closed ) {
                mainWin.close();
            }
            else if ( event.type == sf::Event::Resized ) {
                backgroundSprite.setTextureRect( sf::IntRect( -backSize.x , -backSize.y , mainWin.getSize().x + 2.f * backSize.x , mainWin.getSize().y + 2.f * backSize.y ) );

                HUDBackground.setScale( mainWin.getSize().x / 50.f , HUDBackground.getScale().y );
            }

            else if ( event.type == sf::Event::KeyReleased ) {
                if ( event.key.code == sf::Keyboard::Return ) {
                    isPaused = !isPaused;

                    // Show mouse cursor when paused
                    mainWin.setMouseCursorVisible( isPaused );
                }
            }

            else if ( event.type == sf::Event::LostFocus ) {
                isPaused = true;

                // Show mouse cursor when paused
                mainWin.setMouseCursorVisible( isPaused );
            }

            else if ( event.type == sf::Event::GainedFocus ) {
                isPaused = false;

                // Show mouse cursor when paused
                mainWin.setMouseCursorVisible( isPaused );
            }
        }
    }

    EnemyFormula::cleanup();
    Bullet::cleanup();

    return 0;
}
