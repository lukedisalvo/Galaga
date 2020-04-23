/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* HAL and Application includes */
#include <Application.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>
#include <HAL/Joystick.h>
extern tImage GrimReaper8BPP_UNCOMP;

/**
 * The main entry point of your project. The main function should immediately
 * stop the Watchdog timer, call the Application constructor, and then
 * repeatedly call the main super-loop function. The Application constructor
 * should be responsible for initializing all hardware components as well as all
 * other finite state machines you choose to use in this project.
 *
 * THIS FUNCTION IS ALREADY COMPLETE. Unless you want to temporarily experiment
 * with some behavior of a code snippet you may have, we DO NOT RECOMMEND
 * modifying this function in any way.
 */

int main(void)
{
    // Stop Watchdog Timer - THIS SHOULD ALWAYS BE THE FIRST LINE OF YOUR MAIN
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();

    // Main super-loop! In a polling architecture, this function should call
    // your main FSM function over and over.
    while (true)
    {
        Application_loop(&app, &hal);
        HAL_refresh(&hal);

    }

}

/**
 * A helper function which increments a value with a maximum. If incrementing
 * the number causes the value to hit its maximum, the number wraps around
 * to 0.
 */
uint32_t CircularIncrement(uint32_t value, uint32_t maximum)
{
    return (value + 1) % maximum;
}

uint32_t CircularDecrement(uint32_t value, uint32_t maximum)
{
    return (value + (maximum - 1)) % maximum;
}

/**
 * The main constructor for your application. This function should initialize
 * each of the FSMs which implement the application logic of your project.
 *
 * @return a completely initialized Application object
 */

Application Application_construct()
{
    Application app;

    // Initialize local application state variables here!
    app.MenuChoice = PLAY_GAME;
    app.firstCall = true;
    app.Timer = SWTimer_construct(1500);
    app.gfx = GFX_construct(GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_WHITE);
    SWTimer_start(&app.Timer);
    app.state = 0;

    app.Health = 3;
    app.Diff = 0;
    app.Score = 0;
    app.Shield = 0;
    app.back = 0;

    return app;
}

/**
 * The main super-loop function of the application. We place this inside of a
 * single infinite loop in main. In this way, we can model a polling system of
 * FSMs. Every cycle of this loop function, we poll each of the FSMs one time,
 * followed by refreshing all inputs to the system through a convenient
 * [HAL_refresh()] call.
 *
 * @param app:  A pointer to the main Application object.
 * @param hal:  A pointer to the main HAL object
 */
void Application_loop(Application* app, HAL* hal)
{
    //Non-Blocking test. Pressing LauchpadS1 turns launchpad LED1 on and releasing it turns it off
    LED_turnOff(&hal->launchpadLED1);
    if (Button_isPressed(&hal->launchpadS1))
    {
        LED_turnOn(&hal->launchpadLED1);
    }
    Graphics_setForegroundColor(&app->gfx.context, 0);
    //splash screen
    if(app->state == 0)
    {
        Graphics_drawImage(&app->gfx.context, &GrimReaper8BPP_UNCOMP, 0, 0);
        if(SWTimer_expired(&app->Timer))
        {
            Graphics_clearDisplay(&app->gfx.context);
            app->state++;
        }
    }
    //main menu
    else if(app->state == 1)
    {
        Graphics_drawString(&app->gfx.context, "Main Menu", -1, 0, 0, false);
        Graphics_drawString(&app->gfx.context, "---------------------------", -1, 0, 10, false);
        Graphics_drawString(&app->gfx.context, "Play Game", -1, 10, 20, false);
        Graphics_drawString(&app->gfx.context, "How to play", -1, 10, 30, false);
        Graphics_drawString(&app->gfx.context, "High Scores", -1, 10, 40, false);
        Application_Mainmenu(app, hal);
        Application_changeScreen(app, hal);


    }
    //play game
    else if(app->state == 2)
    {
        Graphics_drawString(&app->gfx.context, "Avatar vs Dangers", -1, 0, 3, false);
        Graphics_drawString(&app->gfx.context, "---------------------------", -1, 0, 10, false);
        Application_Game(app, hal);
    }
    //how to play
    else if(app->state == 3)
    {
        Graphics_drawString(&app->gfx.context, "How to Play", -1, 0, 0, false);
        Graphics_drawString(&app->gfx.context, "---------------------------", -1, 0, 10, false);
        Graphics_drawString(&app->gfx.context, "Ya do stuff", -1, 20, 20, false);
        if (Button_isTapped(&hal->boosterpackJS))
        {
            Application_returnHome(app, hal);
        }


    }
    //high scores
    else if(app->state == 4)
    {
        Graphics_drawString(&app->gfx.context, "High Scores", -1, 0, 0, false);
        Graphics_drawString(&app->gfx.context, "---------------------------", -1, 0, 10, false);
        Graphics_drawString(&app->gfx.context, "Score 1: 0 ", -1, 20, 20, false);
        Graphics_drawString(&app->gfx.context, "Score 2: 0", -1, 20, 30, false);
        Graphics_drawString(&app->gfx.context, "Score 3: 0", -1, 20, 40, false);
        Graphics_drawString(&app->gfx.context, "Score 4: 0", -1, 20, 50, false);
        Graphics_drawString(&app->gfx.context, "Score 5: 0", -1, 20, 60, false);
        Graphics_drawString(&app->gfx.context, "To main menu: JS ", -1, 20, 70, false);
        if (Button_isTapped(&hal->boosterpackJS))
        {
            Application_returnHome(app, hal);
        }


    }


}

void Application_Mainmenu(Application* app, HAL* hal)
{
    uint32_t newMenu;
    if (hal->joystick.isTappedDown)
    {
        newMenu = CircularIncrement((uint32_t) app->MenuChoice, NUM_MENU_CHOICES);
        app->MenuChoice = (startScreen) newMenu;
    }
    if (hal->joystick.isTappedUp)
    {
        newMenu = CircularDecrement((uint32_t) app->MenuChoice, NUM_MENU_CHOICES);
        app->MenuChoice = (startScreen) newMenu;
    }
    switch(app->MenuChoice)
    {
    case PLAY_GAME:
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 20, false);
        Graphics_setForegroundColor(&app->gfx.context, 16777215);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 30, false);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 40, false);

        break;

    case HOW_TO_PLAY:
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 30, false);
        Graphics_setForegroundColor(&app->gfx.context, 16777215);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 20, false);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 40, false);

        break;

    case HIGH_SCORES:
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 40, false);
        Graphics_setForegroundColor(&app->gfx.context, 16777215);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 20, false);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 30, false);
        break;

    }

}

void Application_Game(Application* app, HAL* hal)
{
    char health[10];
    sprintf(health, "H:%d", app->Health);
    Graphics_drawString(&app->gfx.context, (int8_t*) health, -1, 0, 120, false);

    char diff[10];
    sprintf(diff, "D:%d", app->Diff);
    Graphics_drawString(&app->gfx.context, (int8_t*) diff, -1, 25, 120, false);

    char score[10];
    sprintf(score, "S:%03d", app->Score);
    Graphics_drawString(&app->gfx.context, (int8_t*) score, -1, 50, 120, false);

    char shield[10];
    sprintf(shield, "P:%02d", app->Shield);
    Graphics_drawString(&app->gfx.context, (int8_t*) shield, -1, 90, 120, false);

    Graphics_setForegroundColor(&app->gfx.context, 0);
    Graphics_fillCircle(&app->gfx.context, 63, 63, 5);

}

void Application_returnHome(Application* app, HAL* hal)
{
    Graphics_clearDisplay(&app->gfx.context);
    app->state = 1;
}

void Application_changeScreen(Application* app, HAL* hal)
{
    if(Button_isTapped(&hal->boosterpackJS))
    {
        switch(app->MenuChoice)
        {
        case PLAY_GAME:
            Graphics_clearDisplay(&app->gfx.context);
            app->state = 2;
            break;


        case HOW_TO_PLAY:
            Graphics_clearDisplay(&app->gfx.context);
            app->state = 3;
            break;

        case HIGH_SCORES:
            Graphics_clearDisplay(&app->gfx.context);
            app->state = 4;
            break;
        }

    }

}


