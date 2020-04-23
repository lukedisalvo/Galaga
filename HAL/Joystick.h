/*
 * Joystick.h
 *
 *  Created on: Oct 18, 2018
 *      Author: Leyla
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

struct _Joystick
{
    unsigned int vx;
    unsigned int vy;

    unsigned int vx_old;
    unsigned int vy_old;

    //There are two main ways to do this, one is using a FSM

    // The outputs of the FSM.
    int pushState;
    bool isTapped;
    bool isTappedRight;
    bool isTappedLeft;
    bool isTappedUp;
    bool isTappedDown;
    bool isTiltedRight;
    bool isTiltedLeft;
    bool isTiltedUp;
    bool isTiltedDown;
};
typedef struct _Joystick Joystick;

Joystick Joystick_construct();

void refreshJoyStick(Joystick *);

bool isTappedUp(Joystick joystick);
bool isTappedDown(Joystick joystick);
bool isTappedLeft(Joystick joystick);
bool isTappedRight(Joystick joystick);

bool isTiltedLeft(Joystick joystick);
bool isTiltedRight(Joystick joystick);
bool isTiltedUp(Joystick joystick);
bool isTiltedDown(Joystick joystick);

#endif /* JOYSTICK_H_ */

