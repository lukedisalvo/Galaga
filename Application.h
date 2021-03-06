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

enum _DIFFICULTY
{
    DIFF0, DIFF1, NUM_DIFF_CHOICES
};
typedef enum _DIFFICULTY difficulty;

struct _Application
{

    // Put your application members and FSM state variables here!
    // =========================================================================
    startScreen MenuChoice;
    difficulty DiffChoice;
    bool firstCall;

    //The Graphics Structure
    GFX gfx;
    SWTimer Timer;
    SWTimer Game_Timer;
    SWTimer Shield_Timer;
    SWTimer Enemy_Spawn;
    SWTimer Special_Attack;
    SWTimer Special_Attack2;
    SWTimer attack;

    int state;
    int Health;
    int Diff;
    int Score;
    int Highscore1;
    int Highscore2;
    int Highscore3;
    int Highscore4;
    int Highscore5;
    int Shield;
    int back;
    int x;
    int y;
    int x_old;
    int y_old;
    int shield_Pack;
    int Enemy;
    int S_locationx;
    int S_locationy;
    int E_locationx;
    int E_locationy;
    int E_locationxold;
    int E_locationyold;
    int game_Counter;




    bool Shield_Active;
    bool Stop_Attack;
    bool Enemy_Active;
    bool ShieldPack_Active;
    bool SpecialAttack_Active;
    bool SpecialAttack2_Active;
    bool Stop_SpecialAttack;
    bool Stop_SpecialAttack2;




};
typedef struct _Application Application;


// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal);

// Generic circular increment function
uint32_t CircularIncrement(uint32_t value, uint32_t maximum);

uint32_t CircularDecrement(uint32_t value, uint32_t maximum);

void Application_Mainmenu(Application* app, HAL* hal);

void Application_Game(Application* app, HAL* hal);

void Application_returnHome(Application* app, HAL* hal);

void Application_changeScreen(Application* app, HAL* hal);

void Application_moveCharacter(Application* app, HAL* hal);

void Application_SpawnShield(Application* app, HAL* hal);

void Application_ShieldOff(Application* app, HAL* hal);

void Application_ShieldOn(Application* app, HAL* hal);

void Application_SpawnEnemy(Application* app, HAL* hal);

void Application_Hit(Application* app, HAL* hal);

void Application_Death(Application* app, HAL* hal);

void Application_ResetGame(Application* app, HAL* hal);

void Application_HighScores(Application* app, HAL* hal);

void Application_HighScoreChecker(Application* app, HAL* hal);

void Application_SpecialAttackOff(Application* app, HAL* hal);

void Application_SpecialAttackOn(Application* app, HAL* hal);

void Application_SpecialAttack2On(Application* app, HAL* hal);

void Application_SpecialAttack2Off(Application* app, HAL* hal);

void Application_ChangeDifficulty(Application* app, HAL* hal);

void Application_HowtoPlay(Application* app, HAL* hal);

#endif /* APPLICATION_H_ */
