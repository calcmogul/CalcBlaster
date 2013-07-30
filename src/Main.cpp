//=============================================================================
//File Name: Main.cpp
//Description: Runs main game
//Author: Tyler Veness
//=============================================================================

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Sleep.hpp>
#include <sstream>
#include <map>
#include <fstream>
#include <list>
#include <cmath>
#include <iostream> // TODO Remove me

#include "UIFont.hpp"
#include "Sounds.hpp"
#include "FriendlyShip.hpp"
#include "EnemyFormula.hpp"
#include "Bullet.hpp"
#include "Utils.hpp"

// Constants used during title screen animation
const float DISP_TIME = 2.f;
const float FADE_TIME = 2.f;
const unsigned int TOTAL_STEPS = 4;

int main() {
    // TODO Fix bullet spawning behind ship to make window resizeable
    sf::RenderWindow mainWin( sf::VideoMode( 800 , 600 )  , "Calc Blaster" , sf::Style::Close );
    mainWin.setMouseCursorVisible( false );
    mainWin.setVerticalSyncEnabled( true );

    sf::Image appIcon;
    if ( !appIcon.loadFromFile( "Resources/GalagaShip.png" ) ) {
        exit( 1 );
    }
    mainWin.setIcon( appIcon.getSize().x , appIcon.getSize().y , appIcon.getPixelsPtr() );

    sf::Image backgroundImage;
    if ( !backgroundImage.loadFromFile( "Resources/PaperBackground.png" ) ) {
        exit( 1 );
    }
    sf::Vector2f backSize;
    backSize.x = backgroundImage.getSize().x;
    backSize.y = backgroundImage.getSize().y;

    sf::Texture backgroundTexture;
    backgroundTexture.create( nextPowerTwo( backSize.x ) , nextPowerTwo( backSize.y ) );
    backgroundTexture.update( backgroundImage );

    backgroundTexture.setRepeated( true ); // Tiling background

    sf::Sprite backgroundSprite( backgroundTexture );
    backgroundSprite.setTextureRect( sf::IntRect( -backSize.x , -backSize.y , mainWin.getSize().x + 2 * backSize.x , mainWin.getSize().y + 2 * backSize.y ) );

    /* ===== Create title screen ===== */
    // Tracks progress during title screen animation
    static unsigned int titlePart = 0;

    // Title
    sf::Image titleImg;
    if ( !titleImg.loadFromFile( "Resources/CalcBlasterTitle.png" ) ) {
        exit( 1 );
    }

    sf::Texture titleTexture;
    titleTexture.loadFromImage( titleImg );

    sf::Sprite titleSpr( titleTexture );
    titleSpr.setPosition( sf::Vector2f( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f ) );

    // Created by
    sf::Image createdImg;
    if ( !createdImg.loadFromFile( "Resources/CreatedBy.png" ) ) {
        exit( 1 );
    }

    sf::Texture createdTexture;
    createdTexture.loadFromImage( createdImg );

    sf::Sprite createdSpr( createdTexture );
    createdSpr.setPosition( sf::Vector2f( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f ) );

    /* ===== */
    sf::Color pxlColor;

    for ( unsigned int y = 0 ; y < titleImg.getSize().y ; y++ ) {
        for ( unsigned int x = 0 ; x < titleImg.getSize().x ; x++ ) {
            pxlColor = titleImg.getPixel( x , y );
            pxlColor.a = 0;
            titleImg.setPixel( x , y , pxlColor );
        }
    }

    titleTexture.update( titleImg );

    for ( unsigned int y = 0 ; y < createdImg.getSize().y ; y++ ) {
        for ( unsigned int x = 0 ; x < createdImg.getSize().x ; x++ ) {
            pxlColor = createdImg.getPixel( x , y );
            pxlColor.a = 0;
            createdImg.setPixel( x , y , pxlColor );
        }
    }

    createdTexture.update( createdImg );
    /* ===== */

    // Times
    sf::Clock titleTime;
    sf::Clock diffTime;
    /* =============================== */

    /* ===== High Score Graphics ===== */
    // These hold the high score graphic
    sf::Sprite hiScoreSprite;
    sf::RenderTexture hiScoreTexture;
    hiScoreTexture.create( 1024 , 1024 );

    // Holds version of graphic before user's score is displayed
    sf::RenderTexture preScoreTexture;
    preScoreTexture.create( 1024 , 1024 );
    /* =============================== */


    /* ===== Hold misc. data for high score table ===== */
    // Flag determining if high score graphic was created yet
    bool isHiScoreCreated = false;

    // Variables that hold
    std::pair<const std::string , std::string>* player = NULL;
    float playerRow = 0;
    /* ================================================ */

    sf::RectangleShape HUDBackground( sf::Vector2f( mainWin.getSize().x , 45.f ) );
    HUDBackground.setFillColor( sf::Color( 90 , 90 , 90 , 170 ) );

    FriendlyShip myShip( sf::Vector2f( 0.f , 200.f ) , 500.f );
    myShip.syncObject( mainWin );

    /* ===== Create health bar sprite ===== */
    sf::Texture healthTexture;
    healthTexture.loadFromImage( appIcon );
    healthTexture.setRepeated( true );

    sf::Sprite healthSprite( healthTexture );
    healthSprite.setTextureRect( sf::IntRect( 0 , 0 , healthTexture.getSize().x * myShip.getHealth() / 100.f , healthTexture.getSize().y ) );
    healthSprite.setPosition( 0.f , 0.f );
    /* ==================================== */

    // Create text that holds score
    sf::Text scoreText( "Score: 0" , UIFont::getInstance()->technical() , 24 );
    scoreText.setPosition( 0.f , 0.f );
    unsigned long long int scoreCount = 0;

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

    // TODO Object displays in wrong spot
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
    //backBody.body->CreateFixture( &backFixture ); // FIXME Fixture isn't in right spot

    b2MassData backMass;
    backBody.body->GetMassData( &backMass );
    backMass.mass = 1000000.f;
    backBody.body->SetMassData( &backMass );

    backShape.setSize( boxBR - boxTL );
    backShape.setOrigin( backShape.getSize() / 2.f );

    // Make edge body move at 10 m/s up
    backBody.body->SetLinearVelocity( b2Vec2( 0.f , 2.f ) );
    /* ================================== */

    // Adjust background at beginning of simulation
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

    /* ===== Create pause graphic ===== */
    sf::RenderTexture pauseTexture;
    pauseTexture.create( 512 , 512 ); // 400, 300

    sf::RectangleShape pauseRect( sf::Vector2f( 400 , 300 ) );
    pauseRect.setFillColor( sf::Color( 90 , 90 , 90 , 170 ) );

    sf::Text pauseText( "PAUSED" , UIFont::getInstance()->technical() , 50 );
    pauseText.setPosition( sf::Vector2f( (pauseRect.getSize().x - pauseText.findCharacterPos( 7 ).x) / 2.f , (pauseRect.getSize().y - pauseText.getCharacterSize()) / 2.f ) );
    pauseText.setStyle( sf::Text::Bold );
    pauseText.setColor( sf::Color( 255 , 255 , 255 ) );

    pauseTexture.draw( pauseRect );
    pauseTexture.draw( pauseText );
    pauseTexture.display();

    sf::Sprite pauseSprite( pauseTexture.getTexture() );
    pauseSprite.setTextureRect( sf::IntRect( 0 , 0 , 400 , 300 ) );
    /* ================================ */

    // Used to limit framerate of simulation
    sf::Clock frameTime;

    /* Set the step values for the simulation.
     * Time step is 1/30 of a second (30Hz)
     */
    float32 timeStep = 1.0f / 30.0f;
    int32 velocityIterations = 64; //Min: 8
    int32 positionIterations = 24; //Min: 3

    mainWin.setView( sf::View( sf::FloatRect( backBody.drawShape->getPosition().x - mainWin.getSize().x / 2 , backBody.drawShape->getPosition().y - mainWin.getSize().y / 2 , mainWin.getSize().x , mainWin.getSize().y ) ) );

    sf::Clock gameTime;

    sf::Event event;
    sf::Clock shootClock;

    sf::Clock enemySpawnClock;

    bool isPaused = false;

    Sounds::getInstance()->background().play();
    while ( mainWin.isOpen() ) {
        if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Escape ) ) {
            mainWin.close();
        }

        /* If game isn't paused, the frame rate is at or below 60Hz, and the
         * player is still alive
         */
        if ( titlePart > TOTAL_STEPS && !isPaused && frameTime.getElapsedTime().asSeconds() > 1.f / 60.f && myShip.getHealth() > 0 ) {
            // Reset frame rate timer
            frameTime.restart();

            // Instruct the world to perform a single step of simulation.
            // It is generally best to keep the time step and iterations fixed.
            Box2DBase::world.Step( timeStep , velocityIterations , positionIterations );

            myShip.syncObject( mainWin );
            backBody.syncObject( mainWin );
            EnemyFormula::syncObjects( mainWin );
            Bullet::syncObjects( mainWin );

            Bullet::checkCollisions( myShip , mainWin );
            EnemyFormula::checkCollisions( myShip , mainWin );

            myShip.controlShip( NULL );
            EnemyFormula::controlEnemies( NULL );

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
                    Sounds::getInstance()->shoot().play();
                    Bullet::add( myShip , mainWin , sf::Color( 255 , 0 , 0 ) , Bullet::infinity );

                    /* Bullets start out at (0,0), not at the Box2D body, so we
                     * need to sync the two first.
                     */
                    Bullet::syncObjects( mainWin );

                    shootClock.restart();
                }

                else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::K ) ) {
                    Sounds::getInstance()->shoot().play();
                    Bullet::add( myShip , mainWin , sf::Color( 0 , 0 , 255 ) , Bullet::constant );

                    /* Bullets start out at (0,0), not at the Box2D body, so we
                     * need to sync the two first.
                     */
                    Bullet::syncObjects( mainWin );

                    shootClock.restart();
                }

                else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::L ) ) {
                    Sounds::getInstance()->shoot().play();
                    Bullet::add( myShip , mainWin , sf::Color( 0 , 0 , 0 ) , Bullet::zero );

                    /* Bullets start out at (0,0), not at the Box2D body, so we
                     * need to sync the two first.
                     */
                    Bullet::syncObjects( mainWin );

                    shootClock.restart();
                }
            }

            /* Time between spawns starts at 4 minutes and is cut in half every
             * second continuously (there is no step)
             */
            if ( enemySpawnClock.getElapsedTime().asSeconds() > 240 / ( 2.f * gameTime.getElapsedTime().asSeconds() ) ) {
                // FIXME Use more exact method of limiting bounds of rand()
                EnemyFormula::add( sf::Vector2f( 90 + rand() % (mainWin.getSize().x - 180) + mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f ) , b2Vec2( 0.f , 0.f ) );

                /* Enemies start out at (0,0), not at the Box2D body, so we
                 * need to sync the two first.
                 */
                EnemyFormula::syncObjects( mainWin );

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
        // Create high score graphic if it hasn't been already
        else if ( myShip.getHealth() == 0 && !isHiScoreCreated ) {
            // Holds position of row at which to draw strings
            float drawRow = 20.f;

            // Create background
            sf::RectangleShape hiScoreRect( sf::Vector2f( 600 , 450 ) );
            hiScoreRect.setFillColor( sf::Color( 90 , 90 , 90 , 170 ) );
            preScoreTexture.draw( hiScoreRect );

            // Write header
            sf::Text hiScoreText( "High Scores" , UIFont::getInstance()->technical() , 50 );
            hiScoreText.setPosition( sf::Vector2f( (hiScoreRect.getSize().x - hiScoreText.findCharacterPos( 12 ).x) / 2.f , drawRow ) );
            hiScoreText.setStyle( sf::Text::Bold );
            hiScoreText.setColor( sf::Color( 255 , 255 , 255 ) );
            preScoreTexture.draw( hiScoreText );

            drawRow = drawRow + hiScoreText.getCharacterSize() + 15.f;

            /* ===== Parse high scores file and generate table ===== */
            hiScoreText.setCharacterSize( 30 );
            std::ifstream hiScoreFile( "HighScore.txt" );

            std::multimap<std::string , std::string> entries;
            std::string tempName;
            std::string tempScore;

            if ( hiScoreFile.is_open() ) {
                while ( !hiScoreFile.eof() ) {
                    std::getline( hiScoreFile , tempName );
                    std::getline( hiScoreFile , tempScore );

                    entries.insert( std::pair<std::string,std::string>( tempScore , tempName ) );
                }

                hiScoreFile.close();
            }

            // Process the player's current score
            std::stringstream ss;
            ss << "PLAYER";
            tempName = ss.str();

            ss.clear();
            ss.str( "" );
            ss << scoreCount;
            tempScore = ss.str();

            entries.insert( std::pair<std::string,std::string>( tempScore , tempName ) );

            // Remove map entries from end until there are five in the map
            while ( entries.size() > 5 ) {
                std::map<std::string , std::string>::iterator i = entries.end();
                i--;

                entries.erase( i );
            }

            // Attempt to find the player's score
            for ( std::map<std::string , std::string>::iterator i = entries.begin() ; i != entries.end() ; i++ ) {
                if ( i->second == std::string("PLAYER") ) {
                    // Store address of object pointed to by iterator
                    player = &*i;

                    break;
                }
            }

            // Draw all entries in order
            for ( std::map<std::string , std::string>::iterator i = entries.begin() ; i != entries.end() ; i++ ) {
                // Skip drawing player because they have to enter their name later
                //if ( player != &*i ) {
                    hiScoreText.setString( i->second );
                    hiScoreText.setString( "" );
                    hiScoreText.setPosition( sf::Vector2f( 60.f , drawRow ) );
                    preScoreTexture.draw( hiScoreText );

                    hiScoreText.setString( i->first );
                    hiScoreText.setPosition( sf::Vector2f( 600 - hiScoreText.findCharacterPos( hiScoreText.getString().getSize() + 1 ).x , drawRow ) );
                    preScoreTexture.draw( hiScoreText );
                //}
#if 0
                else {
                    // TODO Allow name entry

                    // Store the player's drawing row
                    playerRow = drawRow;
                }
#endif

                drawRow = drawRow + hiScoreText.getCharacterSize() + 10.f;
            }
            /* ===================================================== */

            // Store scores to file
            std::ofstream scoreFile( "HighScore.txt" , std::fstream::trunc );
            if ( scoreFile.is_open() ) {
                std::list<std::pair<const std::string , std::string>> elems;
                for ( std::map<std::string , std::string>::iterator i = entries.begin() ; i != entries.end() ; i++ ) {
                    elems.push_back( *i );
                    elems.sort(); // TODO Create custom sorting function
                    scoreFile << i->second << "\n";
                    scoreFile << i->first << "\n";
                }

                scoreFile.close();
            }

            preScoreTexture.display();

            hiScoreSprite.setTexture( preScoreTexture.getTexture() );
            hiScoreSprite.setTextureRect( sf::IntRect( 0 , 0 , 600 , 450 ) );

            isHiScoreCreated = true;
        }

        HUDBackground.setPosition( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y + mainWin.getSize().y / 2.f - HUDBackground.getSize().y );
        healthSprite.setPosition( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f + 6.f , mainWin.getView().getCenter().y + mainWin.getSize().y / 2.f + 6.f - HUDBackground.getSize().y );

        // Update score in text if needed
        if ( myShip.getScore() != scoreCount ) {
            scoreCount = myShip.getScore();

            std::stringstream ss;
            ss << "Score: " << scoreCount;
            scoreText.setString( ss.str() );
        }

        scoreText.setPosition( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f + mainWin.getSize().x - (scoreText.findCharacterPos( scoreText.getString().getSize() + 1 ).x - scoreText.getPosition().x) - 20.f , mainWin.getView().getCenter().y + mainWin.getSize().y / 2.f - HUDBackground.getSize().y / 2.f - scoreText.getCharacterSize() / 2.f );

        mainWin.clear( sf::Color( 10 , 10 , 10 ) );

        mainWin.draw( backgroundSprite );
        //mainWin.draw( *(backBody.drawShape) ); // FIXME window edge collision
        EnemyFormula::drawAll( myShip , mainWin );
        mainWin.draw( myShip );
        Bullet::drawAll( mainWin );

        mainWin.draw( HUDBackground );
        mainWin.draw( healthSprite );
        mainWin.draw( scoreText );

        if ( titlePart <= TOTAL_STEPS ) {
            float totalSecs = titleTime.getElapsedTime().asSeconds();
            float diffSecs = diffTime.getElapsedTime().asSeconds();

            titleSpr.setPosition( sf::Vector2f( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f ) );
            createdSpr.setPosition( sf::Vector2f( mainWin.getView().getCenter().x - mainWin.getSize().x / 2.f , mainWin.getView().getCenter().y - mainWin.getSize().y / 2.f ) );

            // Finish fade title in
            if ( titlePart == 0 && totalSecs > FADE_TIME ) {
                titlePart++;
                diffTime.restart();
            }

            // Finish title display
            else if ( titlePart == 1 && totalSecs > FADE_TIME + DISP_TIME ) {
                titlePart++;
                diffTime.restart();
            }

            // Finish fade title out and fade createdBy in
            else if ( titlePart == 2 && totalSecs > FADE_TIME + DISP_TIME + FADE_TIME ) {
                titlePart++;
                diffTime.restart();
            }

            // Finish createdBy display
            else if ( titlePart == 3 && totalSecs > FADE_TIME + DISP_TIME + FADE_TIME + DISP_TIME ) {
                titlePart++;
                diffTime.restart();
            }

            // Finish fade createdBy out
            else if ( titlePart == 4 && totalSecs > FADE_TIME + DISP_TIME + FADE_TIME + DISP_TIME + FADE_TIME ) {
                titlePart++;
                diffTime.restart();
            }

            // Fade title in
            if ( titlePart == 0 ) {
                sf::Color pxlColor;

                for ( unsigned int y = 0 ; y < titleImg.getSize().y ; y++ ) {
                    for ( unsigned int x = 0 ; x < titleImg.getSize().x ; x++ ) {
                        pxlColor = titleImg.getPixel( x , y );
                        pxlColor.a = 255 - 255 * (FADE_TIME - diffSecs) / FADE_TIME;
                        titleImg.setPixel( x , y , pxlColor );
                    }
                }

                titleTexture.update( titleImg );
            }

            // Fade title out and fade createdBy in
            if ( titlePart == 2 ) {
                sf::Color pxlColor;

                for ( unsigned int y = 0 ; y < titleImg.getSize().y ; y++ ) {
                    for ( unsigned int x = 0 ; x < titleImg.getSize().x ; x++ ) {
                        pxlColor = titleImg.getPixel( x , y );
                        pxlColor.a = 255 - 255 * (diffSecs - FADE_TIME) / FADE_TIME;
                        titleImg.setPixel( x , y , pxlColor );
                    }
                }

                titleTexture.update( titleImg );

                for ( unsigned int y = 0 ; y < createdImg.getSize().y ; y++ ) {
                    for ( unsigned int x = 0 ; x < createdImg.getSize().x ; x++ ) {
                        pxlColor = createdImg.getPixel( x , y );
                        pxlColor.a = 255 - 255 * (FADE_TIME - diffSecs) / FADE_TIME;
                        createdImg.setPixel( x , y , pxlColor );
                    }
                }

                createdTexture.update( createdImg );
            }

            // Fade createdBy out
            if ( titlePart == 4 ) {
                sf::Color pxlColor;

                for ( unsigned int y = 0 ; y < createdImg.getSize().y ; y++ ) {
                    for ( unsigned int x = 0 ; x < createdImg.getSize().x ; x++ ) {
                        pxlColor = createdImg.getPixel( x , y );
                        pxlColor.a = 255 - 255 * (diffSecs - FADE_TIME) / FADE_TIME;
                        createdImg.setPixel( x , y , pxlColor );
                    }
                }

                createdTexture.update( createdImg );
            }

            mainWin.draw( titleSpr );
            mainWin.draw( createdSpr );
        }

        // If player is dead, show the high scores
        if ( myShip.getHealth() == 0 ) {
            // Display high scores table and save the latest one to a file
            hiScoreSprite.setPosition( sf::Vector2f( mainWin.getView().getCenter().x - hiScoreSprite.getTextureRect().width / 2.f , mainWin.getView().getCenter().y - hiScoreSprite.getTextureRect().height / 2.f ) );
            mainWin.draw( hiScoreSprite );

            if ( isHiScoreCreated ) {

            }
            // TODO Name entry
        }

        // Draw pause graphic over everything if paused
        if ( isPaused ) {
            pauseSprite.setPosition( sf::Vector2f( mainWin.getView().getCenter().x - pauseSprite.getTextureRect().width / 2.f , mainWin.getView().getCenter().y - pauseSprite.getTextureRect().height / 2.f ) );
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

                    if ( isPaused ) {
                        Sounds::getInstance()->background().pause();
                    }
                    else {
                        Sounds::getInstance()->background().play();
                    }

                    // Show mouse cursor when paused
                    mainWin.setMouseCursorVisible( isPaused );
                }
            }

            else if ( event.type == sf::Event::LostFocus ) {
                isPaused = true;

                // Stop background music while pausing
                Sounds::getInstance()->background().pause();

                // Show mouse cursor when paused
                mainWin.setMouseCursorVisible( isPaused );
            }

            else if ( event.type == sf::Event::GainedFocus ) {
                isPaused = false;

                // Resume background music when not paused
                Sounds::getInstance()->background().play();

                // Show mouse cursor when paused
                mainWin.setMouseCursorVisible( isPaused );
            }
        }
    }

    EnemyFormula::cleanup();
    Bullet::cleanup();

    return 0;
}
