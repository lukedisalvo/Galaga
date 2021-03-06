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

    int x;
    int y;

    bool isTapped;
    bool isTappedRight;
    bool isTappedLeft;
    bool isTappedUp;
    bool isTappedDown;

    bool isTiltedRightMax;
    bool isTiltedLeftMax;
    bool isTiltedUpMax;
    bool isTiltedDownMax;

    bool isTiltedRightMed;
    bool isTiltedLeftMed;
    bool isTiltedUpMed;
    bool isTiltedDownMed;

    bool isTiltedRightLow;
    bool isTiltedLeftLow;
    bool isTiltedUpLow;
    bool isTiltedDownLow;





};
typedef struct _Joystick Joystick;

Joystick Joystick_construct();

void refreshJoyStick(Joystick *);

bool isTappedUp(Joystick joystick);
bool isTappedDown(Joystick joystick);
bool isTappedLeft(Joystick joystick);
bool isTappedRight(Joystick joystick);




#endif /* JOYSTICK_H_ */

