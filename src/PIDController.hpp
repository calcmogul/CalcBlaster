//=============================================================================
//File Name: PIDController.hpp
//Description: Implements a PID controller for controlling linear input systems
//Author: Tyler Veness
//=============================================================================

#ifndef PID_CONTROLLER_HPP
#define PID_CONTROLLER_HPP

#include <atomic>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>

class PIDController {
public:
    PIDController( float p , float i , float d );
    PIDController();
    virtual ~PIDController();

    void setPID( float p , float i , float d );

    float getP();
    float getI();
    float getD();

private:
    std::atomic<float> m_p;
    std::atomic<float> m_i;
    std::atomic<float> m_d;

    sf::Clock m_timer;
    float m_errLast;
    float m_err;
    float m_errSum;

    sf::Thread m_sampleThread;
    std::atomic<bool> m_isRunning;

    static void threadFunc( void* object );
};

#endif // PID_CONTROLLER_HPP
