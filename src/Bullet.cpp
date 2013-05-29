//=============================================================================
//File Name: Bullet.cpp
//Description: Handles all m_bullets created by entities
//Author: Tyler Veness
//=============================================================================

#include "Bullet.hpp"
#include "ShipBase.hpp"
#include "EnemyFormula.hpp"
#include <iostream> // FIXME Remove me

std::vector<Bullet*> Bullet::m_bullets;

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
                delete bullet; // bullet hit something so delete it

                /* Act upon 'other' object if it's an EnemyFormula
                 * (Only that class has a non-NULL userData pointer)
                 */
                if ( contact->other->GetUserData() != NULL ) {
                    userData* tempData = static_cast<userData*>( contact->other->GetUserData() );

                    std::cout << tempData->formulaType << " =?= " << bullet->getType() << "\n";

                    // If type of formula matches type of bullet
                    if ( tempData->formulaType == bullet->getType() ) {
                        // Remove formula
                        delete tempData->formulaObj;

                        // TODO Implement scoring
                    }

                    // Else damage player
                    else {
                        if ( ship.getHealth() >= 50 ) {
                            ship.setHealth( ship.getHealth() - 50 );
                        }
                        else {
                            ship.setHealth( 0 );
                        }
                    }
                }

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
        shape( sf::Vector2f( 2.f , 10.f ) ) ,
        m_sourceBody( ship.body ) ,
        m_type( type ) {
    std::cout << "type=" << m_type << "\n";
    float angle = ship.body->GetAngle();

    // Define the ground box shape.
    b2PolygonShape bulletBox;

    // The extents are the half-widths of the box.
    bulletBox.SetAsBox( 1.f / 30.f , 5.f / 30.f );

    // Add the bullet fixture to the bullet body.
    body->CreateFixture( &bulletBox , 1.f );
    body->SetBullet( true );

    body->SetLinearVelocity( b2Vec2( 10.f * cos( angle + b2_pi / 2.f ) , 10.f + 10.f * sin( angle + b2_pi / 2.f ) ) + ship.body->GetLinearVelocity() );
    body->SetTransform( body->GetPosition() , angle );

    shape.setFillColor( color );
    shape.setOrigin( 5.f , 1.f );
}
