/* 
 * Every file you submit should start with a header like this!
 * 
 * File:    Lab08_main.c
 * Author: Gurpinder Singh (gujsingh@ucsc.edu)
 *
 * Created on May 27, 2020, 7:10 PM
 */
// **** Include libraries here ****
// Standard libraries
#include <string.h>
#include <math.h>
#include <stdio.h>

//CSE013E Support Library
#include "UNIXBOARD.h"
#include "Game.h"
#include "Player.h"

#define emptyChar ""
#define titleStringOutput "Title: %s\n"
#define descStringOutput "Description: %s\n"
#define nCapital "N "
#define sCapital "S "
#define wCapital "W "
#define eCapital "E "
#define exitString "Exits: %s\n"
#define enterValueString "Enter Direction To Travel (n,e,w,s) or q to quit followed by enter: "
#define lowerQ "q"
#define lowerN "n"
#define lowerE "e"
#define lowerS "s"
#define lowerW "w"
#define enterValidArg "Enter Valid Direction\n\n\n"
#define newLines "\n\n\n"
#define exitStrings "EXIT\n"
int main() {


/******************************** Your custom code goes below here ********************************/
    printf("Welcome to Gujsingh's World  Compiled on %s %s.\n\n\n", __TIME__, __DATE__);
    char enterCharacter[] = emptyChar;
    char title[] = emptyChar, description[] = emptyChar, exitChar[15] = emptyChar;
    int exits;
    GameInit();
    while (1) {
        strcpy(exitChar, emptyChar);
        strcpy(title, emptyChar);
        strcpy(description, emptyChar);
        GameGetCurrentRoomTitle(title);
        exits = GameGetCurrentRoomExits();
        printf(titleStringOutput, title);
        GameGetCurrentRoomDescription(description);
        printf(descStringOutput, description);
        if (exits & GAME_ROOM_EXIT_NORTH_EXISTS) {
            strcat(exitChar, nCapital);
        }
        if (exits & GAME_ROOM_EXIT_SOUTH_EXISTS) {
            strcat(exitChar, sCapital);
        }
        if (exits & GAME_ROOM_EXIT_EAST_EXISTS) {
            strcat(exitChar, eCapital);
        }
        if (exits & GAME_ROOM_EXIT_WEST_EXISTS) {
            strcat(exitChar, wCapital);
        }
        printf(exitString, exitChar);
        printf(enterValueString);
        scanf("%s", enterCharacter); //obtains operator
        if (strcmp(enterCharacter, lowerQ) == 0) {
            printf(exitStrings);
            break;
        }
        if (strcmp(enterCharacter, lowerN) == 0 && (exits & GAME_ROOM_EXIT_NORTH_EXISTS)) {
            GameGoNorth();
        } else if (strcmp(enterCharacter, lowerS) == 0 && (exits & GAME_ROOM_EXIT_SOUTH_EXISTS)) {
            GameGoSouth();
        } else if (strcmp(enterCharacter, lowerE) == 0 && (exits & GAME_ROOM_EXIT_EAST_EXISTS)) {
            GameGoEast();
        } else if (strcmp(enterCharacter, lowerW) == 0 && (exits & GAME_ROOM_EXIT_WEST_EXISTS)) {
            GameGoWest();
        }
        else {
            printf(enterValidArg);
            continue;
        }
        printf(newLines);
    }
    while (1);

    /**************************************************************************************************/
}

