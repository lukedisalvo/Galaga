/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <HAL/HAL.h>
#include <HAL/Graphics.h>
enum _START_SCREEN
{
    PLAY_GAME, HOW_TO_PLAY, HIGH_SCORES, NUM_MENU_CHOICES
};
typedef enum _START_SCREEN startScreen;

struct _Application
{

    // Put your application members and FSM state variables here!
    // =========================================================================
    startScreen MenuChoice;
    bool firstCall;

    //The Graphics Structure
    GFX gfx;
    SWTimer Timer;

    int state;
    int Health;
    int Diff;
    int Score;
    int Shield;
    int back;
    int x;
    int y;



};
typedef struct _Application Application;


// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal);

// Called whenever the UART module needs to be updated
void Application_updateCommunications(Application* app, HAL* hal);

// Interprets an incoming character and echoes back to terminal what kind of
// character was received (number, letter, or other)
void Application_interpretIncomingChar(Application* app, HAL* hal);


// Generic circular increment function
uint32_t CircularIncrement(uint32_t value, uint32_t maximum);

uint32_t CircularDecrement(uint32_t value, uint32_t maximum);

void Application_Mainmenu(Application* app, HAL* hal);

void Application_Game(Application* app, HAL* hal);

void Application_returnHome(Application* app, HAL* hal);

void Application_changeScreen(Application* app, HAL* hal);


#endif /* APPLICATION_H_ */
