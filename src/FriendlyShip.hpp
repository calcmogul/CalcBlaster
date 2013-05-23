//=============================================================================
//File Name: FriendlyShip.hpp
//Description: Handles actions and rendering of friendly ships (including
//             player)
//Author: Tyler Veness
//=============================================================================

#include "ShipBase.hpp"

class FriendlyShip : public ShipBase {
public:
    FriendlyShip( const sf::Vector2f& position , float fullHealth );
    virtual ~FriendlyShip();

    void controlShip( void* userData );

private:
    static sf::Texture m_shipTexture;
    static bool m_isLoaded;
};
