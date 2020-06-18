/* 
 * Every file you submit should start with a header like this!
 * 
 * File:    Game.c
 * Author: Gurpinder Singh (gujsingh@ucsc.edu)
 *
 * Created on May 27, 2020, 7:10 PM
 */
// **** Include libraries here ****
// Standard libraries
#include <stdio.h> 
#include <string.h>
#include <stdlib.h> // For exit() 
#include "UNIXBOARD.h"
#include "Game.h"
#include "Player.h"
//CMPE13 Support Library
#define possibleExitPaths 4
#define HeaderLength 3
#define CharLengths 2
#define charEmpty ""
#define RoomPath "RoomFiles/room%d.txt"
struct GameContent {
    char title[GAME_MAX_ROOM_TITLE_LENGTH + 1];
    char description[GAME_MAX_ROOM_DESC_LENGTH + 1];
    uint8_t exitNorth;
    uint8_t exitSouth;
    uint8_t exitWest;
    uint8_t exitEast;
};
static struct GameContent GameIO = {};

int GoToFile(int room_num) {
    char fileName[30] = charEmpty;
    sprintf(fileName, RoomPath, room_num);
    FILE* roomFile = fopen(fileName, "rb");
    strcpy(GameIO.title,charEmpty);
    strcpy(GameIO.description,charEmpty);
    if (roomFile) {
        //get size of Title     
        int size=0, titleChar=0, i=0;
        fseek(roomFile, HeaderLength, SEEK_SET);
        fread(&size, sizeof (char), 1, roomFile);
        //get Title and 
        char character[CharLengths]=charEmpty ;
        for (i = 0; i < size; i++) {
            fread(&titleChar, sizeof (char), 1, roomFile);
            sprintf(character, "%c", titleChar);
            strcat(GameIO.title, character);
        }
        //get appropriate version descrition
        int item = 1, descInt=0, descSize=0, values=0;
        while (1) {
            //obtain items
            item = 0, descInt = 0, descSize = 0, values = 0;
            int verified = TRUE;
            fread(&size, sizeof (char), 1, roomFile);
            //check if items are required
            if (size) {
                //iterate and check if items are here
                for (i = 0; i < size; i++) {
                    fread(&item, sizeof (char), 1, roomFile);
                    //if item is not in item chest transition to next version requirements
                    if (FindInInventory(item) == STANDARD_ERROR) {
                        for (; i < size - 1; i++) {
                            fread(&item, sizeof (char), 1, roomFile);
                        }
                        //skip over version descrition and (exits through +4)      
                        fread(&descSize, sizeof (char), 1, roomFile);
                        fseek(roomFile, descSize, SEEK_CUR);
                        fread(&descSize, sizeof (char), 1, roomFile);
                        fseek(roomFile, descSize + possibleExitPaths, SEEK_CUR);
                        verified = FALSE;
                        break;
                    }
                }
            }
            // if this is the correct version then obtain description
            if (verified == TRUE) {
                fread(&values, sizeof (char), 1, roomFile);
                for (i = 0; i < values; i++) {
                    descInt = 0;
                    fread(&descInt, sizeof (char), 1, roomFile);
                    sprintf(character, "%c", descInt);
                    strcat(GameIO.description, character);
                }
                break;
            } else {
                continue;
            }
        }
        //take items
        fread(&size, sizeof (char), 1, roomFile);
        for (i = 0; i < size; i++) {
            if (i == size) {
                break;
            }
            fread(&item, sizeof (char), 1, roomFile);
            if (FindInInventory(item) == STANDARD_ERROR) {
                AddToInventory(item);
            }

        }
        //obtain exits
        fread(&GameIO.exitNorth, sizeof (char), 1, roomFile);
        fread(&GameIO.exitEast, sizeof (char), 1, roomFile);
        fread(&GameIO.exitSouth, sizeof (char), 1, roomFile);
        fread(&GameIO.exitWest, sizeof (char), 1, roomFile);
    } else {
        return STANDARD_ERROR;
    }
    fclose(roomFile);
    return SUCCESS;

}

int GameGoNorth(void) {
    return GoToFile(GameIO.exitNorth);

}

int GameGoEast(void) {
    return GoToFile(GameIO.exitEast);
}

/**
 * @see GameGoNorth
 */
int GameGoSouth(void) {
    return GoToFile(GameIO.exitSouth);
}

/**
 * @see GameGoNorth
 */
int GameGoWest(void) {
    return GoToFile(GameIO.exitWest);
}

/**
 * This function sets up anything that needs to happen at the start of the game. This is just
 * setting the current room to STARTING_ROOM and loading it. It should return SUCCESS if it succeeds
 * and STANDARD_ERROR if it doesn't.
 * @return SUCCESS or STANDARD_ERROR
 */
int GameInit(void) {
    return GoToFile(STARTING_ROOM);

}

/**
 * Copies the current room title as a NULL-terminated string into the provided character array.
 * Only a NULL-character is copied if there was an error so that the resultant output string
 * length is 0.
 * @param title A character array to copy the room title into. Should be GAME_MAX_ROOM_TITLE_LENGTH+1
 *             in length in order to allow for all possible titles to be copied into it.
 * @return The length of the string stored into `title`. Note that the actual number of chars
 *         written into `title` will be this value + 1 to account for the NULL terminating
 *         character.
 */
int GameGetCurrentRoomTitle(char *title) {
    strcpy(title, GameIO.title);
    return strlen(title);
}

/**
 * GetCurrentRoomDescription() copies the description of the current room into the argument desc as
 * a C-style string with a NULL-terminating character. The room description is guaranteed to be less
 * -than-or-equal to GAME_MAX_ROOM_DESC_LENGTH characters, so the provided argument must be at least
 * GAME_MAX_ROOM_DESC_LENGTH + 1 characters long. Only a NULL-character is copied if there was an
 * error so that the resultant output string length is 0.
 * @param desc A character array to copy the room description into.
 * @return The length of the string stored into `desc`. Note that the actual number of chars
 *          written into `desc` will be this value + 1 to account for the NULL terminating
 *          character.
 */
int GameGetCurrentRoomDescription(char *desc) {
    strcpy(desc, GameIO.description);
    return strlen(desc);
}

/**
 * This function returns the exits from the current room in the lowest-four bits of the returned
 * uint8 in the order of NORTH, EAST, SOUTH, and WEST such that NORTH is in the MSB and WEST is in
 * the LSB. A bit value of 1 corresponds to there being a valid exit in that direction and a bit
 * value of 0 corresponds to there being no exit in that direction. The GameRoomExitFlags enum
 * provides bit-flags for checking the return value.
 *
 * @see GameRoomExitFlags
 *
 * @return a 4-bit bitfield signifying which exits are available to this room.
 *
 */

uint8_t GameGetCurrentRoomExits(void) {
    uint8_t value = 0;
    if (GameIO.exitNorth) {
        value += GAME_ROOM_EXIT_NORTH_EXISTS;
    }
    if (GameIO.exitEast) {
        value += GAME_ROOM_EXIT_EAST_EXISTS;
    }
    if (GameIO.exitSouth) {
        value += GAME_ROOM_EXIT_SOUTH_EXISTS;
    }
    if (GameIO.exitWest) {
        value += GAME_ROOM_EXIT_WEST_EXISTS;
    }
    return value;
}
