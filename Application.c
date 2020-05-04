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

#define XMAX 108
#define XMIN 3
#define YMIN 17
#define YMAX 97
#define MOVESPEEDMAX 3
#define MOVESPEEDMED 2
#define MOVESPEEDLOW 1
#define CENTER 8
#define CONFINER 1
#define UPPERLIMIT 21000
#define MIDDLELIMIT 10500
#define SHIELDRANGE 25
#define ENEMYRANGE 10
#define SPECIALMAX 120
#define SPECIALMIN 25
#define SHIELDPACKRANGE 8

extern tImage GrimReaper8BPP_UNCOMP;
extern tImage Spaceshipone8BPP_UNCOMP;
extern tImage SpaceshipReset8BPP_UNCOMP;
extern tImage Shield8BPP_UNCOMP;
extern tImage galagabackground8BPP_UNCOMP;
extern tImage Enemyone8BPP_UNCOMP;
extern tImage Enemytwo8BPP_UNCOMP;
extern tImage Enemythree8BPP_UNCOMP;
extern tImage TitleScreen8BPP_UNCOMP;
extern tImage Reset8BPP_UNCOMP;

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
    app.DiffChoice = DIFF0;
    app.firstCall = true;
    app.gfx = GFX_construct(GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_BLACK);

    app.Timer = SWTimer_construct(1500);
    app.Game_Timer = SWTimer_construct(5000);
    app.Shield_Timer = SWTimer_construct(1500);
    app.Enemy_Spawn = SWTimer_construct(3000);
    app.Special_Attack = SWTimer_construct(250);
    app.Special_Attack2 = SWTimer_construct(250);
    app.attack = SWTimer_construct(250);
    SWTimer_start(&app.Timer);

    app.state = 0;
    app.Health = 3;
    app.Score = 0;
    app.Shield = 0;
    app.back = 0;
    app.x = 55;
    app.y = 55;
    app.shield_Pack = 0;
    app.Enemy = 0;
    app.E_locationx = 0;
    app.E_locationy = 0;
    app.E_locationxold = 0;
    app.E_locationyold = 0;
    app.S_locationx = 0;
    app.S_locationy = 0;
    app.Highscore1 = 0;
    app.Highscore2 = 0;
    app.Highscore3 = 0;
    app.Highscore4 = 0;
    app.Highscore5 = 0;
    app.Shield_Active = false;
    app.Stop_Attack = false;
    app.Enemy_Active = false;
    app.ShieldPack_Active = false;
    app.SpecialAttack_Active = false;
    app.Stop_SpecialAttack = false;
    app.SpecialAttack2_Active = false;
    app.Stop_SpecialAttack2 = false;

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
    Graphics_setForegroundColor(&app->gfx.context, 16777215);
    //splash screen
    if(app->state == 0)
    {
        Graphics_drawImage(&app->gfx.context, &TitleScreen8BPP_UNCOMP, 0, 0);
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
        Graphics_drawImage(&app->gfx.context, &Enemyone8BPP_UNCOMP, 25, 55);
        Graphics_drawImage(&app->gfx.context, &Enemytwo8BPP_UNCOMP, 45, 55);
        Graphics_drawImage(&app->gfx.context, &Enemythree8BPP_UNCOMP, 65, 55);
        Graphics_drawImage(&app->gfx.context, &Enemyone8BPP_UNCOMP, 85, 55);

        Graphics_drawImage(&app->gfx.context, &Enemytwo8BPP_UNCOMP, 25, 72);
        Graphics_drawImage(&app->gfx.context, &Enemythree8BPP_UNCOMP, 45, 72);
        Graphics_drawImage(&app->gfx.context, &Enemyone8BPP_UNCOMP, 64, 72);
        Graphics_drawImage(&app->gfx.context, &Enemytwo8BPP_UNCOMP, 85, 72);
        Graphics_drawImage(&app->gfx.context, &Spaceshipone8BPP_UNCOMP, 55, 110);
        Application_Mainmenu(app, hal);
        Application_changeScreen(app, hal);


    }
    //play game
    else if(app->state == 2)
    {
        if (app->Health > 0)
        {
            Graphics_drawString(&app->gfx.context, "Avatar vs Dangers", -1, 0, 3, false);
            Graphics_drawString(&app->gfx.context, "---------------------------", -1, 0, 10, false);
            Application_Game(app, hal);
        }
        else
        {
            Graphics_clearDisplay(&app->gfx.context);
            app->state = 5;
        }

    }
    //how to play
    else if(app->state == 3)
    {
        Application_HowtoPlay(app, hal);
        if (Button_isTapped(&hal->boosterpackJS))
        {
            Application_returnHome(app, hal);
        }


    }
    //high scores
    else if(app->state == 4)
    {
        Application_HighScores(app, hal);
        if (Button_isTapped(&hal->boosterpackJS))
        {
            Application_returnHome(app, hal);
        }


    }
    //Game over screen
    else if (app->state == 5)
    {
        Application_Death(app, hal);
    }

}

//Application_Mainmenu draws the ">" that points to what state the player can choose.
//It also includes a FSM for when the joystick is tapped up or down the > will move with it.
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
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 30, false);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 40, false);

        break;

    case HOW_TO_PLAY:
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 30, false);
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 20, false);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 40, false);

        break;

    case HIGH_SCORES:
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 40, false);
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 20, false);
        Graphics_drawString(&app->gfx.context, ">", -1, 0, 30, false);
        break;

    }

}

//Application_Game draws the boundaries of the playing field and draws the health, difficulty
//score, and shield points. It also includes all of the supporting functions that make the game work.
void Application_Game(Application* app, HAL* hal)
{
    char health[10];
    sprintf(health, "H:%d", app->Health);
    Graphics_drawString(&app->gfx.context, (int8_t*) health, -1, 0, 120, true);

    char diff[10];
    sprintf(diff, "D:%d", app->Diff);
    Graphics_drawString(&app->gfx.context, (int8_t*) diff, -1, 25, 120, true);

    char score[10];
    sprintf(score, "S:%03d", app->Score);
    Graphics_drawString(&app->gfx.context, (int8_t*) score, -1, 50, 120, true);

    char shield[10];
    sprintf(shield, "P:%02d", app->Shield);
    Graphics_drawString(&app->gfx.context, (int8_t*) shield, -1, 90, 120, true);

    Graphics_drawLineH(&app->gfx.context, 0, 127, 115);


    Application_moveCharacter(app, hal);
    Application_SpawnShield(app, hal);
    Application_SpawnEnemy(app, hal);
    Application_Hit(app, hal);
    Application_ChangeDifficulty(app, hal);

}

//Application_returnHome brings the player to the main screen
void Application_returnHome(Application* app, HAL* hal)
{
    Graphics_clearDisplay(&app->gfx.context);
    app->state = 1;
}

//Applicaiton_changeScreen allows the player to choose which of the three states the game can be in.
//When the joystick is pressed, the player will go to the Play game, How to Play or Highscores state, depending
//on where the ">" is located.
void Application_changeScreen(Application* app, HAL* hal)
{
    if(Button_isTapped(&hal->boosterpackJS))
    {
        switch(app->MenuChoice)
        {
        case PLAY_GAME:
            Graphics_clearDisplay(&app->gfx.context);
            app->state = 2;
            SWTimer_start(&app->Game_Timer);
            SWTimer_start(&app->Enemy_Spawn);
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

//Application_moveCharacter allows for the movement of the character in the game. It also supports
//the movement of both the shield attack and special attack that I implemented.
void Application_moveCharacter(Application* app, HAL* hal)
{
    app->x_old = app->x;
    app->y_old = app->y;

    Graphics_drawImage(&app->gfx.context, &Spaceshipone8BPP_UNCOMP, app->x, app->y);

    if(hal->joystick.isTiltedRightMax)
    {
        if (app->x < XMAX)
        {
            app->x = app->x + MOVESPEEDMAX;
        }
    }
    else if(hal->joystick.isTiltedRightMed)
    {
        if (app->x < XMAX)
        {
            app->x = app->x + MOVESPEEDMED;
        }
    }
    else if(hal->joystick.isTiltedRightLow)
    {
        if (app->x < XMAX)
        {
            app->x = app->x + MOVESPEEDLOW;
        }
    }

    if(hal->joystick.isTiltedLeftMax)
    {
        if (app->x > XMIN)
        {
            app->x = app->x - MOVESPEEDMAX;
        }
    }
    else if(hal->joystick.isTiltedLeftMed)
    {
        if (app->x > XMIN)
        {
            app->x = app->x - MOVESPEEDMED;
        }
    }
    else if(hal->joystick.isTiltedLeftLow)
    {
        if (app->x > XMIN)
        {
            app->x = app->x - MOVESPEEDLOW;
        }
    }
    if(hal->joystick.isTiltedUpMax)
    {
        if (app->y < YMAX)
        {
            app->y = app->y + MOVESPEEDMAX;
        }
    }
    else if(hal->joystick.isTiltedUpMed)
    {
        if (app->y < YMAX)
        {
            app->y = app->y + MOVESPEEDMED;
        }
    }
    else if(hal->joystick.isTiltedUpLow)
    {
        if (app->y < YMAX)
        {
            app->y = app->y + MOVESPEEDLOW;
        }
    }
    if(hal->joystick.isTiltedDownMax)
    {
        if (app->y > YMIN)
        {
            app->y = app->y - MOVESPEEDMAX;
        }
    }
    else if(hal->joystick.isTiltedDownMed)
    {
        if (app->y > YMIN)
        {
            app->y = app->y - MOVESPEEDMED;
        }
    }
    else if(hal->joystick.isTiltedDownLow)
    {
        if (app->y > YMIN)
        {
            app->y = app->y - MOVESPEEDLOW;
        }
    }

    if (Button_isTapped(&hal->boosterpackS2) && app->Shield > 0)
    {
        Application_ShieldOn(app, hal);
    }
    if (app->Shield_Active == true)
    {
        Graphics_setForegroundColor(&app->gfx.context, 255);
        Graphics_drawCircle(&app->gfx.context, app->x + CENTER, app->y + CENTER, 25);
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawCircle(&app->gfx.context, app->x_old + CENTER, app->y_old + CENTER, 25);
    }
    if(SWTimer_expired(&app->Shield_Timer))
    {
        Application_ShieldOff(app, hal);
    }
    if(Button_isPressed(&hal->boosterpackJS) && app->Shield >= 2)
    {
        Application_SpecialAttackOn(app, hal);
    }
    if(app->SpecialAttack_Active == true)
    {
        Graphics_setForegroundColor(&app->gfx.context, 255);
        Graphics_drawLineV(&app->gfx.context, app->x + 20 , 20, 120);
        Graphics_drawLineV(&app->gfx.context, app->x - 5, 20, 120);
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawLineV(&app->gfx.context, app->x_old + 20, 20, 120);
        Graphics_drawLineV(&app->gfx.context, app->x_old - 5, 20, 120);
        if(SWTimer_expired(&app->Special_Attack))
        {
            Application_SpecialAttack2On(app, hal);
        }
    }
    if(SWTimer_expired(&app->Special_Attack))
    {
        Application_SpecialAttackOff(app, hal);

    }
    if(app->SpecialAttack2_Active == true)
    {
        Graphics_setForegroundColor(&app->gfx.context, 255);
        Graphics_drawLineH(&app->gfx.context, 0, 127, app->y + 20);
        Graphics_drawLineH(&app->gfx.context, 0, 127, app->y - 5);
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawLineH(&app->gfx.context, 0, 127, app->y_old + 20);
        Graphics_drawLineH(&app->gfx.context, 0, 127, app->y_old - 5);
    }
    if(SWTimer_expired(&app->attack))
    {
        Application_SpecialAttack2Off(app, hal);
    }


      Graphics_drawImage(&app->gfx.context, &SpaceshipReset8BPP_UNCOMP, app->x_old, app->y_old);
      Graphics_drawImage(&app->gfx.context, &Spaceshipone8BPP_UNCOMP, app->x, app->y);


}

//Application_SpawnShield generates a shield pack every 5 seconds if there is not already a shield pack
//in the game
void Application_SpawnShield(Application* app, HAL* hal)
{
    if(SWTimer_expired(&app->Game_Timer) && app->shield_Pack == 0 && app->ShieldPack_Active == false)
    {
        app->ShieldPack_Active = true;
        int UpperX = XMAX;
        int LowerX = XMIN;
        int UpperY = YMAX;
        int LowerY = YMIN;
        int numX = (rand() % (UpperX - LowerX + CONFINER)) + LowerX;
        int numY = (rand() % (UpperY - LowerY + CONFINER)) + LowerY;
        app->S_locationx = numX;
        app->S_locationy = numY;
        Graphics_drawImage(&app->gfx.context, &Shield8BPP_UNCOMP, app->S_locationx, app->S_locationy);
        app->shield_Pack++;
        SWTimer_start(&app->Game_Timer);
    }

}

//Application_ShieldOn activates the shield
void Application_ShieldOn(Application* app, HAL* hal)
{
    SWTimer_start(&app->Shield_Timer);
    app->Shield_Active = true;
    app->Shield--;
    app->Stop_Attack = true;

}
//Application_ShieldOff deactivates the shield
void Application_ShieldOff(Application* app, HAL* hal)
{
    app->Shield_Active = false;
    if(app->Stop_Attack == true)
    {
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawCircle(&app->gfx.context, app->x + CENTER, app->y + CENTER, 25);
    }
    app->Stop_Attack = false;
}
//Application_SpecialAttackOn activates the first part of the special attack
void Application_SpecialAttackOn(Application* app, HAL* hal)
{
    SWTimer_start(&app->Special_Attack);
    app->SpecialAttack_Active = true;
    app->Shield--;
    app->Shield--;
    app->Stop_SpecialAttack = true;
}
//Application_SpecialAttack2On activates the second part of the special attack
void Application_SpecialAttack2On(Application* app, HAL* hal)
{
    SWTimer_start(&app->attack);
    app->SpecialAttack2_Active = true;
    app->Stop_SpecialAttack2 = true;
}
//Application_SpecialAttack2Off  deactivates the first part of the special attack
void Application_SpecialAttackOff(Application* app, HAL* hal)
{
    app->SpecialAttack_Active = false;
    if(app->Stop_SpecialAttack == true)
    {
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawLineV(&app->gfx.context, app->x + 20, 10, 120);
        Graphics_drawLineV(&app->gfx.context, app->x - 5, 10, 120);
    }
    app->Stop_SpecialAttack = false;
}
//Application_SpecialAttack2Off  deactivates the second part of the special attack
void Application_SpecialAttack2Off(Application* app, HAL* hal)
{
    app->SpecialAttack2_Active = false;
    if(app->Stop_SpecialAttack2 == true)
    {
        Graphics_setForegroundColor(&app->gfx.context, 0);
        Graphics_drawLineH(&app->gfx.context, 0, 127, app->y + 20);
        Graphics_drawLineH(&app->gfx.context, 0, 127, app->y - 5);
    }
    app->Stop_SpecialAttack2 = false;
}

//Application_SpawnEnemy generates a enemy every 3 seconds if there is not already a enemy
//in the game
void Application_SpawnEnemy(Application* app, HAL* hal)
{
    tImage Image;
    int x = rand();
    if(x > UPPERLIMIT)
    {
        Image = Enemyone8BPP_UNCOMP;
    }
    else if (x < UPPERLIMIT && x > MIDDLELIMIT)
    {
        Image = Enemytwo8BPP_UNCOMP;
    }
    else
    {
        Image = Enemythree8BPP_UNCOMP;
    }

    app->E_locationxold = app->E_locationx;
    app->E_locationyold = app->E_locationy;
    if (app->Enemy == 1  && app->Diff == 1)
    {
        if (app->x < app->E_locationx)
        {
            app->E_locationx = app->E_locationx - MOVESPEEDLOW;
            Graphics_drawImage(&app->gfx.context, &Reset8BPP_UNCOMP,app->E_locationxold, app->E_locationyold);
            Graphics_drawImage(&app->gfx.context, &Enemyone8BPP_UNCOMP, app->E_locationx, app->E_locationy);

        }
        else if (app->x > app->E_locationx)
        {
            app->E_locationx = app->E_locationx + MOVESPEEDLOW;
            Graphics_drawImage(&app->gfx.context, &Reset8BPP_UNCOMP, app->E_locationxold, app->E_locationyold);
            Graphics_drawImage(&app->gfx.context, &Enemyone8BPP_UNCOMP, app->E_locationx,app->E_locationy);

        }
        if (app->y < app->E_locationy)
        {
            app->E_locationy = app->E_locationy - MOVESPEEDLOW;
            Graphics_drawImage(&app->gfx.context, &Reset8BPP_UNCOMP, app->E_locationxold, app->E_locationyold);
            Graphics_drawImage(&app->gfx.context, &Enemyone8BPP_UNCOMP, app->E_locationx,app->E_locationy);
        }
        else if (app->y > app->E_locationy)
        {
            app->E_locationy = app->E_locationy + MOVESPEEDLOW;
            Graphics_drawImage(&app->gfx.context, &Reset8BPP_UNCOMP,app->E_locationxold, app->E_locationyold);
            Graphics_drawImage(&app->gfx.context, &Enemyone8BPP_UNCOMP, app->E_locationx, app->E_locationy);
        }
    }
    if(SWTimer_expired(&app->Enemy_Spawn) && app->Enemy == 0 && app->Enemy_Active == false)
    {
        app->Enemy_Active = true;
        int UpperX = XMAX;
        int LowerX = XMIN;
        int UpperY = YMAX;
        int LowerY = YMIN;
        int numX = (rand() % (UpperX - LowerX + CONFINER)) + LowerX;
        int numY = (rand() % (UpperY - LowerY + CONFINER)) + LowerY;
        app->E_locationx = numX;
        app->E_locationy = numY;
        Graphics_drawImage(&app->gfx.context, &Image, app->E_locationx, app->E_locationy);
        app->Enemy++;
        SWTimer_start(&app->Enemy_Spawn);
    }

}

//Application_Hit determines if the character was hit by an enemy. It also determines if an enemy was hit by
//a shield or a special atatck.
void Application_Hit(Application* app, HAL* hal)
{
    if (((abs((app->x + CENTER) - (app->E_locationx + CENTER))) < ENEMYRANGE)  && ((abs((app->y + CENTER)- (app->E_locationy + CENTER) ))) < ENEMYRANGE)
    {
        if(app->Health > 0)
        {
            app->Enemy_Active = false;
            app->Health--;
            Graphics_drawImage(&app->gfx.context, &SpaceshipReset8BPP_UNCOMP, app->E_locationx, app->E_locationy);
            app->E_locationx = 0;
            app->E_locationy = 0;
            app->Enemy--;
        }
    }
    if (((abs((app->x + CENTER) - (app->S_locationx + CENTER) )) < SHIELDPACKRANGE)  && ((abs((app->y + CENTER) - (app->S_locationy + CENTER)))) < SHIELDPACKRANGE)
    {
        app->ShieldPack_Active = false;
        app->Shield++;
        Graphics_drawImage(&app->gfx.context, &SpaceshipReset8BPP_UNCOMP, app->S_locationx, app->S_locationy);
        app->S_locationx = 0;
        app->S_locationy = 0;
        app->shield_Pack--;

    }
    if (((abs(app->x - app->E_locationx )) < SHIELDRANGE)  && ((abs(app->y - app->E_locationy))) < SHIELDRANGE && (app->Shield_Active == true))
    {
        app->Score++;
        Graphics_drawImage(&app->gfx.context, &SpaceshipReset8BPP_UNCOMP, app->E_locationx, app->E_locationy);
        app->E_locationx = 0;
        app->E_locationy = 0;
        app->Enemy--;
        app->Enemy_Active = false;
    }
    if(((abs(app->x- app->E_locationx)) < SPECIALMIN) && ((abs(app->y - app->E_locationy))) < SPECIALMAX && (app->SpecialAttack_Active == true))
    {
        app->Score++;
        Graphics_drawImage(&app->gfx.context, &SpaceshipReset8BPP_UNCOMP, app->E_locationx, app->E_locationy);
        app->E_locationx = 0;
        app->E_locationy = 0;
        app->Enemy--;
        app->Enemy_Active = false;
    }
    if(((abs(app->x- app->E_locationx)) < SPECIALMAX) && ((abs(app->y - app->E_locationy))) < SPECIALMIN && (app->SpecialAttack2_Active == true))
     {
         app->Score++;
         Graphics_drawImage(&app->gfx.context, &SpaceshipReset8BPP_UNCOMP, app->E_locationx, app->E_locationy);
         app->E_locationx = 0;
         app->E_locationy = 0;
         app->Enemy--;
         app->Enemy_Active = false;
     }
}
//Application_Death occurs when the players health is 0. The game will clear and show your score.
//It also support logics to go back to the main screen and reset the game by pressing the joystick.
void Application_Death(Application* app, HAL* hal)
{

    Graphics_drawString(&app->gfx.context, "You died", -1, 40, 60, false);
    Graphics_drawString(&app->gfx.context, "Your score:", -1, 20, 100, false);
    char score[10];
    sprintf(score, "%03d", app->Score);
    Graphics_drawString(&app->gfx.context, (int8_t*) score, -1, 95, 100, true);
    if (Button_isTapped(&hal->boosterpackJS))
    {
        Application_HighScoreChecker(app, hal);
        Application_returnHome(app, hal);
        Application_ResetGame(app, hal);
    }
}

//Application_ResetGame resets all of the past game values to its orginal starting values
void Application_ResetGame(Application* app, HAL* hal)
{
    app->DiffChoice = DIFF0;
    app->Health = 3;
    app->Diff = 0;
    app->Score = 0;
    app->Shield = 0;
    app->back = 0;
    app->x = 55;
    app->y = 55;
    app->shield_Pack = 0;
    app->Enemy = 0;
    app->E_locationx = 0;
    app->E_locationy = 0;
    app->S_locationx = 0;
    app->S_locationy = 0;
    app->Shield_Active = false;
    app->Stop_Attack = false;
    app->Enemy_Active = false;
    app->ShieldPack_Active = false;
}
//Application_HighScores draws everything that is seen in the High Score menu and print all of the high scores.
void Application_HighScores(Application* app, HAL* hal)
{

    char score1[10];
    sprintf(score1, "%03d", app->Highscore1);

    char score2[10];
    sprintf(score2, "%03d", app->Highscore2);

    char score3[10];
    sprintf(score3, "%03d", app->Highscore3);

    char score4[10];
    sprintf(score4, "%03d", app->Highscore4);

    char score5[10];
    sprintf(score5, "%03d", app->Highscore5);

    Graphics_drawString(&app->gfx.context, "High Scores", -1, 0, 0, false);
    Graphics_drawString(&app->gfx.context, "---------------------------", -1, 0, 10, false);

    Graphics_drawString(&app->gfx.context, "Score 1:", -1, 20, 20, false);
    Graphics_drawString(&app->gfx.context, (int8_t*) score1, -1, 70, 20, false);

    Graphics_drawString(&app->gfx.context, "Score 2:", -1, 20, 30, false);
    Graphics_drawString(&app->gfx.context, (int8_t*) score2, -1, 70, 30, false);

    Graphics_drawString(&app->gfx.context, "Score 3:", -1, 20, 40, false);
    Graphics_drawString(&app->gfx.context, (int8_t*) score3, -1, 70, 40, false);

    Graphics_drawString(&app->gfx.context, "Score 4:", -1, 20, 50, false);
    Graphics_drawString(&app->gfx.context, (int8_t*) score4, -1, 70, 50, false);

    Graphics_drawString(&app->gfx.context, "Score 5:", -1, 20, 60, false);
    Graphics_drawString(&app->gfx.context, (int8_t*) score5, -1, 70, 60, false);

    Graphics_drawString(&app->gfx.context, "To main menu: JS ", -1, 20, 70, false);
}

//Aplication_HighScoreChecker supports logic that determines whether the players score is high enough to be on the
//High score menu.
void Application_HighScoreChecker(Application* app, HAL* hal)
{
    static int Highscore[5] = {0,0,0,0,0};
    int score;
    score = app->Score;
    int temp;
    int i;
    int n = 5;
    for(i = 0; i < n; i++)
    {
        if (score > Highscore[i])
        {
            temp = Highscore[i];
            Highscore[i] = score;
            score = temp;
        }
    }
    app->Highscore1 = Highscore[0];
    app->Highscore2 = Highscore[1];
    app->Highscore3 = Highscore[2];
    app->Highscore4 = Highscore[3];
    app->Highscore5 = Highscore[4];
}

//Application_ChangeDifficulty changes the difficulty when boosterpack button S1 is pressed
void Application_ChangeDifficulty(Application* app, HAL* hal)
{
    uint32_t newDiff;
    if (Button_isTapped(&hal->boosterpackS1))
    {
        newDiff = CircularIncrement((uint32_t) app->DiffChoice, NUM_DIFF_CHOICES);
        app->DiffChoice = (difficulty) newDiff;
    }
    switch(app->DiffChoice)
    {
        case DIFF0:
            app->Diff = 0;
            break;
        case DIFF1:
            app->Diff = 1;
            break;
    }
}

//Application_HowtoPlay draws the strings that tell the user how to play the game.
void Application_HowtoPlay(Application* app, HAL* hal)
{
    Graphics_drawString(&app->gfx.context, "How to Play", -1, 0, 0, false);
    Graphics_drawString(&app->gfx.context, "---------------------------", -1, 0, 10, false);
    Graphics_drawString(&app->gfx.context, "Avoid all the aliens!", -1, 0, 20, false);
    Graphics_drawString(&app->gfx.context, "Pick up blue shields", -1, 0, 30, false);
    Graphics_drawString(&app->gfx.context, "to gain ammo (P).", -1, 0, 40, false);
    Graphics_drawString(&app->gfx.context, "Press B2 to gain a", -1, 0, 50, false);
    Graphics_drawString(&app->gfx.context, "temporary shield.", -1, 0, 60, false);
    Graphics_drawString(&app->gfx.context, "Save up 2 P and ", -1, 0, 70, false);
    Graphics_drawString(&app->gfx.context, "press the joystick", -1, 0, 80, false);
    Graphics_drawString(&app->gfx.context, "for a special attack.", -1, 0, 90, false);
    Graphics_drawString(&app->gfx.context, "Defeating aliens ", -1, 0, 100, false);
    Graphics_drawString(&app->gfx.context, "increases your score.", -1, 0, 110, false);
    Graphics_drawString(&app->gfx.context, "To main menu: JS", -1, 0, 120, false);
}
