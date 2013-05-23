//=============================================================================
//File Name: Planet.cpp
//Description: Handles creating planets of different colors in world
//Author: Tyler Veness
//=============================================================================

#include "Planet.hpp"
#include "ShipBase.hpp"

std::vector<Planet*> Planet::m_planets;

Planet::~Planet() {
    std::vector<Planet*>::iterator index;
    for ( index = m_planets.begin() ; *index != this ; index++ ) {
        if ( index >= m_planets.end() ) {
            return;
        }
    }

    m_planets.erase( index );
}

void Planet::cleanup() {
    for ( unsigned int index = m_planets.size() ; index > 0 ; index-- ) {
        delete m_planets[0];
    }
}

void Planet::drawAll( const ShipBase& ship , sf::RenderTarget& target , sf::RenderStates states ) {
    for ( unsigned int index = 0 ; index < m_planets.size() ; index++ ) {
        // Realign shading with position of given ship's current position
        m_planets[index]->shader.setParameter( "currentPos" , m_planets[index]->shape.getPosition().x - ship.shape.getPosition().x + target.getSize().x / 2.f , ship.shape.getPosition().y - m_planets[index]->shape.getPosition().y + target.getSize().y / 2.f + 2.f/* offset for shading */ );

        // Redraw planet
        target.draw( *m_planets[index] , m_planets[index]->m_shaderState );
    }
}

void Planet::syncObjects( const sf::Window& referTo ) {
    for ( unsigned int index = 0 ; index < m_planets.size() ; index++ ) {
        m_planets[index]->syncObject( referTo );
    }
}

void Planet::add( const sf::Vector2f& position , const float32& radius , const sf::Color& color ) {
    m_planets.push_back( new Planet( position , radius , color ) );
}

void Planet::applyUnivGravity() {
    // Applies universal gravitation to all combinations of bodies
    for ( b2Body* startBody = Box2DBase::world.GetBodyList() ; startBody != NULL ; startBody = startBody->GetNext() ) {
        for ( b2Body* moveBody = startBody->GetNext() ; moveBody != NULL ; moveBody = moveBody->GetNext() ) {
            if ( moveBody != startBody ) { // shouldn't apply universal gravitation on same body
                b2Vec2 delta = startBody->GetWorldCenter() - moveBody->GetWorldCenter();
                float r = delta.Length();

                float force = Constant::G * moveBody->GetMass() * startBody->GetMass() / ( r * r );

                delta.Normalize();
                startBody->ApplyForceToCenter( -force * delta );
                moveBody->ApplyForceToCenter( force * delta );
            }
        }
    }
}

Planet::Planet( const sf::Vector2f& position , const float32& radius , const sf::Color& color ) : Box2DBase( &shape , position , b2_dynamicBody ) , shape( radius * 30.f ) {
    b2CircleShape earthCircle;

    earthCircle.m_p.Set( 0.f , 0.f );
    earthCircle.m_radius = radius;

    // Add the Earth fixture to the ground body.
    body->CreateFixture( &earthCircle , 1.f );

    body->SetAngularVelocity( 20.f );

    // Create SFML shape
    shape.setFillColor( color );
    shape.setOrigin( shape.getRadius() , shape.getRadius() );
    shape.setPosition( position );

    // Create the shader
    if ( !shader.loadFromFile( "Resources/circleShade.frag" , sf::Shader::Fragment ) ) {
        exit( 1 );
    }
    shader.setParameter( "radius" , shape.getRadius() );
    shader.setParameter( "startFade" , 7.f );
    shader.setParameter( "centerColor" , shape.getFillColor() );

    m_shaderState.shader = &shader;
}
