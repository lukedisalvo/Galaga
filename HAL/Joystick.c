/*
 * Joystick.c
 *
 *  Created on: Oct 18, 2018
 *      Author: Leyla
 */
#include <HAL/Joystick.h>
#include <Application.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define PRESSED 0
#define RELEASED 1
#define VX_MIDDLE 8000
#define VY_MIDDLE 8000
#define VX_MIDDLERIGHT 8500
#define VY_MIDDLEUP 8750
#define VX_MIDDLELEFT 7500
#define VY_MIDDLEDOWN 7500
#define LEFT_THRESHOLD  2500
#define RIGHT_THRESHOLD 13000
#define UPPER_THRESHOLD 13000
#define LOWER_THRESHOLD 2500
#define MIDDLELEFT_THRESHOLD 5250
#define MIDDLERIGHT_THRESHOLD 10500
#define MIDDLEUPPER_THRESHOLD 10500
#define MIDDLELOWER_THRESHOLD 5250

void initADC();
void initJoyStick();
void startADC();

Joystick Joystick_construct()
{
    Joystick joystick;

    initADC();
    initJoyStick();
    startADC();


    joystick.vx_old = VX_MIDDLE;
    joystick.vy_old = VY_MIDDLE;

    //There are two main ways to do this, one is using a FSM

    // The outputs of the FSM.
    joystick.isTappedRight = false;
    joystick.isTappedLeft =  false;
    joystick.isTappedUp = false;
    joystick.isTappedDown = false;

    joystick.isTiltedRightMax = false;
    joystick.isTiltedLeftMax = false;
    joystick.isTiltedUpMax = false;
    joystick.isTiltedDownMax = false;

    joystick.isTiltedRightMed = false;
    joystick.isTiltedLeftMed = false;
    joystick.isTiltedUpMed = false;
    joystick.isTiltedDownMed = false;

    joystick.isTiltedRightLow= false;
    joystick.isTiltedLeftLow = false;
    joystick.isTiltedUpLow = false;
    joystick.isTiltedDownLow = false;


    return joystick;

}

// Initializing the ADC which resides on SoC
void initADC() {
    ADC14_enableModule();

    // This sets the conversion clock to 3MHz
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC,
                     ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1,
                      0
                     );

    // This configures the ADC to store output results
    // in ADC_MEM0 up to ADC_MEM1. Each conversion will
    // thus use two channels.
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);

    // This configures the ADC in manual conversion mode
    // Software will start each conversion.
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
}


void startADC()
{
   // Starts the ADC with the first conversion
   // in repeat-mode, subsequent conversions run automatically
   ADC14_enableConversion();
   ADC14_toggleConversionTrigger();
}


// Interfacing the Joystick with ADC (making the proper connections in software)
void initJoyStick() {

    // This configures ADC_MEM0 to store the result from
    // input channel A15 (Joystick X), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A15,                 // joystick X
                                  ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A15 is multiplexed on GPIO port P6 pin PIN0
    // TODO: which one of GPIO_PRIMARY_MODULE_FUNCTION, or
    //                    GPIO_SECONDARY_MODULE_FUNCTION, or
    //                    GPIO_TERTIARY_MODULE_FUNCTION
    // should be used in place of 0 as the last argument?
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);


    ADC14_configureConversionMemory(ADC_MEM1,
                                   ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                   ADC_INPUT_A9,                 // joystick y
                                   ADC_NONDIFFERENTIAL_INPUTS);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN4,
                                               GPIO_TERTIARY_MODULE_FUNCTION);




}

void getSampleJoyStick(unsigned *X, unsigned *Y)
{
    // ADC runs in continuous mode, we just read the conversion buffers
    *X = ADC14_getResult(ADC_MEM0);

    // TODO: Read the Y channel
    *Y = ADC14_getResult(ADC_MEM1);
}

void refreshJoyStick(Joystick *joystick_p)
{
    joystick_p->vx_old = joystick_p->vx;
    joystick_p->vy_old = joystick_p->vy;

    joystick_p->vx = ADC14_getResult(ADC_MEM0);
    joystick_p->vy = ADC14_getResult(ADC_MEM1);

    //Moving left
    if ((joystick_p->vx < LEFT_THRESHOLD) && (joystick_p->vx_old > LEFT_THRESHOLD))
    {
        joystick_p->isTappedLeft = true;

    }
    else
    {
        joystick_p->isTappedLeft = false;
    }

    //Moving Right
    if ((joystick_p->vx > RIGHT_THRESHOLD) && (joystick_p->vx_old < RIGHT_THRESHOLD))
    {
        joystick_p->isTappedRight = true;
    }
    else
    {
        joystick_p->isTappedRight = false;
    }

    //Moving up
    if ((joystick_p->vy > UPPER_THRESHOLD) && (joystick_p->vy_old < UPPER_THRESHOLD))
    {
        joystick_p->isTappedUp = true;
    }
    else
    {
        joystick_p->isTappedUp = false;
    }

    //Moving down
    if ((joystick_p->vy < LOWER_THRESHOLD) && (joystick_p->vy_old > LOWER_THRESHOLD))
    {
        joystick_p->isTappedDown = true;
    }
    else
    {
        joystick_p->isTappedDown = false;
    }
/////////////////////////////////////////////////////////////////////////////////
    if (joystick_p->vx < LEFT_THRESHOLD)
    {
        joystick_p->isTiltedLeftMax = true;
    }
    else
    {
        joystick_p->isTiltedLeftMax = false;
    }
    if (joystick_p->vx > RIGHT_THRESHOLD)
    {
        joystick_p->isTiltedRightMax = true;
    }
    else
    {
        joystick_p->isTiltedRightMax = false;
    }
    if (joystick_p->vy < LOWER_THRESHOLD)
    {
        joystick_p->isTiltedUpMax = true;
    }
    else
    {
        joystick_p->isTiltedUpMax = false;
    }
    if (joystick_p->vy > UPPER_THRESHOLD)
    {
        joystick_p->isTiltedDownMax = true;
    }
    else
    {
        joystick_p->isTiltedDownMax = false;
    }
///////////////////////////////////////////////////////////////////////////////
    if (joystick_p->vx > LEFT_THRESHOLD && joystick_p->vx < MIDDLELEFT_THRESHOLD)
    {
        joystick_p->isTiltedLeftMed = true;
    }
    else
    {
        joystick_p->isTiltedLeftMed = false;
    }
    if (joystick_p->vx < RIGHT_THRESHOLD && joystick_p->vx > MIDDLERIGHT_THRESHOLD)
    {
        joystick_p->isTiltedRightMed = true;
    }
    else
    {
        joystick_p->isTiltedRightMed = false;
    }
    if (joystick_p->vy < MIDDLELOWER_THRESHOLD && joystick_p->vy > LOWER_THRESHOLD)
    {
        joystick_p->isTiltedUpMed = true;
    }
    else
    {
        joystick_p->isTiltedUpMed = false;
    }
    if (joystick_p->vy < UPPER_THRESHOLD && joystick_p->vy > MIDDLEUPPER_THRESHOLD)
    {
        joystick_p->isTiltedDownMed = true;
    }
    else
    {
        joystick_p->isTiltedDownMed = false;
    }
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (joystick_p->vx > MIDDLELEFT_THRESHOLD && joystick_p->vx < VX_MIDDLELEFT)
    {
        joystick_p->isTiltedLeftLow = true;
    }
    else
    {
        joystick_p->isTiltedLeftLow = false;
    }
    if (joystick_p->vx < MIDDLERIGHT_THRESHOLD && joystick_p->vx > VX_MIDDLERIGHT)
    {
        joystick_p->isTiltedRightLow = true;
    }
    else
    {
        joystick_p->isTiltedRightLow = false;
    }
    if (joystick_p->vy < VY_MIDDLEUP && joystick_p->vy > MIDDLELOWER_THRESHOLD)
    {
        joystick_p->isTiltedUpLow = true;
    }
    else
    {
        joystick_p->isTiltedUpLow = false;
    }
    if (joystick_p->vy > VY_MIDDLEDOWN && joystick_p->vy < MIDDLEUPPER_THRESHOLD)
    {
        joystick_p->isTiltedDownLow = true;
    }
    else
    {
        joystick_p->isTiltedDownLow = false;
    }
}




