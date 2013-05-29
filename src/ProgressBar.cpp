//=============================================================================
//File Name: ProgressBar.cpp
//Description: Holds definitions for ProgressBar class
//Author: Tyler Veness
//=============================================================================

#include "ProgressBar.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

ProgressBar::ProgressBar( const sf::Vector2f& size , std::string message , const sf::Color& fullFillColor , const sf::Color& emptyFillColor , const sf::Color& outlineColor , float percentFull ) : sf::RectangleShape( size ) , barFill( sf::Vector2f( size.x - 2.f , size.y - 2.f ) ) {
    setFillColor( emptyFillColor );
    setOutlineThickness( 1.f );
    setOutlineColor( outlineColor );

    barFill.setFillColor( fullFillColor );
    barFill.setPosition( sf::Vector2f( RectangleShape::getPosition().x + 1.f , RectangleShape::getPosition().y + 1.f ) );

    sfText.setFont( UIFont::getInstance()->arial() );
    sfText.setCharacterSize( 12 );
    sfText.setString( message );
    sfText.setColor( sf::Color( 255 , 255 , 255 ) );
    sfText.setPosition( RectangleShape::getPosition().x , RectangleShape::getPosition().y + RectangleShape::getSize().y + 2.f );

    percent = percentFull;

#if 0
    // Create the shader
    if ( !shader.loadFromFile( "Resources/barGradient.frag" , sf::Shader::Fragment ) ) {
        exit( 1 );
    }
    shader.setParameter( "height" , barFill.getSize().y );
    shader.setParameter( "color" , fullFillColor );

    shaderState.shader = &shader;
#endif
}

void ProgressBar::draw( sf::RenderTarget& target , sf::RenderStates states ) const {
    target.draw( static_cast<sf::RectangleShape>(*this) );
    target.draw( barFill );
    //target.draw( barFill , shaderState );
    target.draw( sfText );
}

void ProgressBar::setPercent( float percentFull ) {
    percent = percentFull;
    barFill.setSize( sf::Vector2f( ( RectangleShape::getSize().x - 2.f ) * percentFull / 100.f , barFill.getSize().y ) );
}

float ProgressBar::getPercent() {
    return percent;
}

void ProgressBar::setPosition( const sf::Vector2f& position ) {
    RectangleShape::setPosition( position );
    barFill.setPosition( position.x + 1 , position.y + 1 );
    sfText.setPosition( RectangleShape::getPosition().x , RectangleShape::getPosition().y + RectangleShape::getSize().y + 2.f );
}

void ProgressBar::setPosition( float x , float y ) {
    RectangleShape::setPosition( x , y );
    barFill.setPosition( x + 1 , y + 1 );
    sfText.setPosition( RectangleShape::getPosition().x , RectangleShape::getPosition().y + RectangleShape::getSize().y + 2.f );
}

void ProgressBar::setSize( const sf::Vector2f& size ) {
    RectangleShape::setSize( size );
    barFill.setSize( sf::Vector2f( ( size.x - 2.f ) * percent , size.y - 2.f ) );
    shader.setParameter( "height" , barFill.getSize().y );

    sfText.setPosition( RectangleShape::getPosition().x , RectangleShape::getPosition().y + RectangleShape::getSize().y + 2.f );
}

void ProgressBar::setSize( float width , float height ) {
    RectangleShape::setSize( sf::Vector2f( width , height ) );
    barFill.setSize( sf::Vector2f( ( width - 2.f ) * percent , height - 2.f ) );
    shader.setParameter( "height" , barFill.getSize().y );

    sfText.setPosition( RectangleShape::getPosition().x , RectangleShape::getPosition().y + RectangleShape::getSize().y + 2.f );
}

void ProgressBar::setString( const std::string& message ) {
    sfText.setString( message );
}

const sf::String& ProgressBar::getString() {
    return sfText.getString();
}

void ProgressBar::setBarFillColor( const sf::Color& fill ) {
    barFill.setFillColor( fill );
}

const sf::Color& ProgressBar::getBarFillColor() {
    return barFill.getFillColor();
}
