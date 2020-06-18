/* 
 * Every file you submit should start with a header like this!
 * 
 * File:    Player.c
 * Author: Gurpinder Singh (gujsingh@ucsc.edu)
 *
 * Created on May 27, 2020, 7:10 PM
 */
// **** Include libraries here ****
// Standard libraries
#include <stdio.h>

//CMPE13 Support Library
#include "Player.h"
#include "UNIXBOARD.h"
#define max_item_size 255
#define min_item_size 0
static int i = 0;
static int arrayName [INVENTORY_SIZE];
static int arraySize = 0;

/**
 * Adds the specified item to the player's inventory if the inventory isn't full.
 * @param item The item number to be stored: valid values are 0-255.
 * @return SUCCESS if the item was added, STANDARD_ERRROR if the item couldn't be added.
 */
int AddToInventory(uint8_t item) {
    if (arraySize == INVENTORY_SIZE || item > max_item_size || item < min_item_size) {
        return STANDARD_ERROR;
    } else {
        arrayName[arraySize] = item;
        arraySize++;
        return SUCCESS;
    }
}

/**
 * Check if the given item exists in the player's inventory.
 * @param item The number of the item to be searched for: valid values are 0-255.
 * @return SUCCESS if it was found or STANDARD_ERROR if it wasn't.
 */
int FindInInventory(uint8_t item) {
    i = 0;
    while (i != arraySize) {
        if (arrayName[i] == item) {
            return SUCCESS;
        }
        i++;
    }
    return STANDARD_ERROR;
}