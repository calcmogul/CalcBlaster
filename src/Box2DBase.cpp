//=============================================================================
//File Name: Box2DBase.cpp
//Description: Base class for Box2D shapes with SFML
//Author: Tyler Veness
//=============================================================================

#include "Box2DBase.hpp"

float BoxToSFML_x( float x ) {
    return x * 30.f;
}

float BoxToSFML_y( float y , float boxHeight ) {
    return boxHeight - y * 30.f;
}

sf::Vector2f BoxToSFML( float x , float y , float boxHeight ) {
    return sf::Vector2f( BoxToSFML_x( x ) , BoxToSFML_y( y , boxHeight ) );
}

float SFMLToBox_x( float x ) {
    return x / 30.f;
}

float SFMLToBox_y( float y , float boxHeight ) {
    return ( boxHeight - y ) / 30.f;
}

b2Vec2 SFMLToBox( float x , float y , float boxHeight ) {
    return b2Vec2( SFMLToBox_x( x ) , SFMLToBox_y( y , boxHeight ) );
}

b2World Box2DBase::world( b2Vec2( 0.f , 0.f ) );

Box2DBase::Box2DBase( sf::Shape* sfShape , const sf::Vector2f& position , b2BodyType bodyType ) {
    b2BodyDef bodyDef;

    bodyDef.type = bodyType;
    bodyDef.position = b2Vec2( position.x / 30.f , ( 600 - position.y ) / 30.f );
    bodyDef.fixedRotation = true;

    body = world.CreateBody( &bodyDef );

    drawShape = sfShape;
}

Box2DBase::~Box2DBase() {
    world.DestroyBody( body );
}

void Box2DBase::syncObject( const sf::Window& referTo ) {
    drawShape->setPosition( BoxToSFML( body->GetPosition().x , body->GetPosition().y , referTo.getSize().y ) );
    drawShape->setRotation( 360.f - body->GetAngle() * 180.f / b2_pi );
}

void Box2DBase::draw( sf::RenderTarget& target , sf::RenderStates states ) const {
    target.draw( *drawShape , states );
}

void Box2DBase::setTexture( sf::Texture* texture , sf::Vector2u subSize ) {
    drawShape->setTexture( texture );

    sf::IntRect tempRect( 0 , 0 , subSize.x , subSize.y );
    drawShape->setTextureRect( tempRect );
}
