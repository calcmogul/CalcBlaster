//=============================================================================
//File Name: Bullet.cpp
//Description: Handles all m_bullets created by entities
//Author: Tyler Veness
//=============================================================================

#include "Bullet.hpp"
#include "ShipBase.hpp"
#include "Sounds.hpp"
#include "EnemyFormula.hpp"

std::vector<Bullet*> Bullet::m_bullets;
sf::Texture Bullet::m_textures[Bullet::size];
sf::Vector2u Bullet::m_sizes[Bullet::size];
bool Bullet::m_isLoaded = false;

Bullet::~Bullet() {
    std::vector<Bullet*>::iterator i;
    for ( i = m_bullets.begin() ; *i != this ; i++ ) {
        if ( i >= m_bullets.end() ) {
            return;
        }
    }

    m_bullets.erase( i );
}

void Bullet::cleanup() {
    for ( unsigned int i = m_bullets.size() ; i > 0 ; i-- ) {
        delete m_bullets[0];
    }
}

void Bullet::drawAll( sf::RenderTarget& target , sf::RenderStates states ) {
    for ( unsigned int i = 0 ; i < m_bullets.size() ; i++ ) {
        target.draw( *m_bullets[i] );
    }
}

void Bullet::syncObjects( const sf::Window& referTo ) {
    for ( unsigned int i = 0 ; i < m_bullets.size() ; i++ ) {
        m_bullets[i]->syncObject( referTo );
        //std::cout << "newPos=" << m_bullets[i]->drawShape->getPosition().x << m_bullets[i]->drawShape->getPosition().y << "\n";
    }
}

void Bullet::add( const ShipBase& ship , const sf::Window& referTo , const sf::Color& color , BulletType type ) {
    m_bullets.push_back( new Bullet( ship , referTo , color , type ) );
}

void Bullet::checkCollisions( ShipBase& ship , const sf::RenderWindow& referTo ) {
    Bullet* bullet;

    unsigned int i = 0;
    while ( i < m_bullets.size() ) {
        bullet = m_bullets[i];

        sf::Vector2f pos( bullet->drawShape->getPosition() );

        // If bullet is off left or right of screen
        if ( pos.x < referTo.getView().getCenter().x - referTo.getSize().x / 2.f ||
                pos.x > referTo.getView().getCenter().x + referTo.getSize().x / 2.f ) {
            delete bullet; // bullet is outside of window view so delete it
            continue; // stop checking body collisions for this bullet because it doesn't exist anymore
        }

        // If bullet is off top or bottom of screen
        if ( pos.y < referTo.getView().getCenter().y - referTo.getSize().y / 2.f ||
                pos.y > referTo.getView().getCenter().y + referTo.getSize().y / 2.f ) {
            delete bullet; // bullet is outside of window view so delete it
            continue; // stop checking body collisions for this bullet because it doesn't exist anymore
        }

        // Check for collisions with other objects
        b2ContactEdge* contact = bullet->body->GetContactList();
        while ( contact != NULL ) {
            // Don't let bullet hit its source body
            if ( bullet->getSourceBody() != contact->other ) {
                /* Act upon 'other' object if it's an EnemyFormula
                 * (Only that class has a non-NULL userData pointer)
                 */
                if ( contact->other->GetUserData() != NULL ) {
                    userData* tempData = static_cast<userData*>( contact->other->GetUserData() );

                    // If type of formula matches type of bullet
                    if ( tempData->formulaType == bullet->getType() ) {
                        // Play kill sound
                        Sounds::getInstance()->enemyKill().play();

                        // Remove formula
                        delete tempData->formulaObj;

                        // Add to score for destroying formula
                        ship.setScore( ship.getScore() + 50 );
                    }

                    // Else damage player
                    else {
                        // Play sound for ship damage
                        Sounds::getInstance()->shipDamage().play();

                        if ( ship.getHealth() >= 50 ) {
                            ship.setHealth( ship.getHealth() - 100 );
                        }
                        else {
                            ship.setHealth( 0 );
                        }
                    }
                }

                delete bullet; // bullet hit something so delete it

                // Make sure loop terminates
                contact = NULL;
                continue; // stop checking body collisions for this bullet because it doesn't exist anymore
            }

            contact = contact->next;
        }

        i++;
    }
}

const b2Body* const Bullet::getSourceBody() const {
    return m_sourceBody;
}

const Bullet::BulletType Bullet::getType() const {
    return m_type;
}

Bullet::Bullet( const ShipBase& ship , const sf::Window& referTo , const sf::Color& color , BulletType type ) :
        Box2DBase( &shape , BoxToSFML( ship.body->GetPosition().x + 1.3f * cos( ship.body->GetAngle() + b2_pi / 2.f ) , ship.body->GetPosition().y + 1.3f * sin( ship.body->GetAngle() + b2_pi / 2.f ) , referTo.getSize().y ) , b2_dynamicBody ) ,
        m_sourceBody( ship.body ) ,
        m_type( type ) {
    if ( !m_isLoaded ) {
        sf::Image tempImg;

        // Load zero bullet
        if ( !tempImg.loadFromFile( "Resources/Bullets/Zero.png" ) ) {
            tempImg.create( 16 , 16 , sf::Color( 255 , 0 , 0 ) );
        }
        tempImg.createMaskFromColor( sf::Color( 255 , 255 , 255 ) , 0 );
        m_textures[zero].create( 16 , 16 );
        m_textures[zero].update( tempImg );
        m_sizes[zero] = tempImg.getSize();

        // Load constant bullet
        if ( !tempImg.loadFromFile( "Resources/Bullets/Constant.png" ) ) {
            tempImg.create( 16 , 16 , sf::Color( 0 , 0 , 0 ) );
        }
        tempImg.createMaskFromColor( sf::Color( 255 , 255 , 255 ) , 0 );
        m_textures[constant].create( 16 , 16 );
        m_textures[constant].update( tempImg );
        m_sizes[constant] = tempImg.getSize();

        // Load infinity bullet
        if ( !tempImg.loadFromFile( "Resources/Bullets/Infinity.png" ) ) {
            tempImg.create( 16 , 16 , sf::Color( 0 , 0 , 255 ) );
        }
        tempImg.createMaskFromColor( sf::Color( 255 , 255 , 255 ) , 0 );
        m_textures[infinity].create( 16 , 16 );
        m_textures[infinity].update( tempImg );
        m_sizes[infinity] = tempImg.getSize();

        m_isLoaded = true;
    }

    // Set correct origin of the image for the given type
    sf::Vector2u tempSizeU = m_textures[type].getSize();
    sf::Vector2f tempSizeF( tempSizeU.x , tempSizeU.y );
    shape.setSize( tempSizeF );
    shape.setOrigin( tempSizeF / 2.f );

    // Set correct texture for the given type
    shape.setTexture( &(m_textures[type]) );
    shape.setTextureRect( sf::IntRect( 0 , 0 , m_sizes[type].x , m_sizes[type].y ) );

    float angle = ship.body->GetAngle();

    // Define the ground box shape.
    b2PolygonShape bulletBox;

    // The extents are the half-widths of the box.
    bulletBox.SetAsBox( tempSizeF.x / 2.f / 30.f , tempSizeF.y / 2.f / 30.f );

    // Add the bullet fixture to the bullet body.
    body->CreateFixture( &bulletBox , 1.f );
    body->SetBullet( true );

    body->SetLinearVelocity( b2Vec2( 10.f * cos( angle + b2_pi / 2.f ) , 10.f + 10.f * sin( angle + b2_pi / 2.f ) ) + ship.body->GetLinearVelocity() );
    body->SetTransform( body->GetPosition() , angle );
}
