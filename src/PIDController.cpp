//=============================================================================
//File Name: PIDController.cpp
//Description: Implements a PID controller for controlling linear input systems
//Author: Tyler Veness
//=============================================================================

#include <SFML/System/Sleep.hpp>
#include "PIDController.hpp"

PIDController::PIDController( float p , float i , float d ) :
        m_sampleThread( threadFunc , this ) {
    m_p = p;
    m_i = i;
    m_d = d;
    m_errLast = 0.f;
    m_err = 0.f;
    m_errSum = 0.f;
    m_isRunning = false;
}

PIDController::PIDController() :
        m_sampleThread( threadFunc , this ) {
    m_p = 0.f;
    m_i = 0.f;
    m_d = 0.f;
    m_errLast = 0.f;
    m_err = 0.f;
    m_errSum = 0.f;
    m_isRunning = false;
}

PIDController::~PIDController() {
    m_isRunning = false;
}

void PIDController::setPID( float p , float i , float d ) {
    m_p = p;
    m_i = i;
    m_d = d;
}

float PIDController::getP() {
    return m_p;
}

float PIDController::getI() {
    return m_i;
}

float PIDController::getD() {
    return m_d;
}

void PIDController::threadFunc( void* object ) {
    PIDController* obj = static_cast<PIDController*>( object );

    while ( obj->m_isRunning ) {
        float dt = obj->m_timer.restart().asSeconds();

        sf::sleep( sf::milliseconds( 20 ) );
    }
}
