#include "Message.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "BOARD.h"

static BB_Event battle;
void TestValue(char* message);

int main(void) {
    printf("\nWelcome to Gujsingh's Message Test:\n");
    //Since decode uses all the functiones except encode This is the test
    //for all except encode
    battle.type = BB_EVENT_NO_EVENT;
    int result = 0;
    // values to input is from the Lab Manuel
    printf("Please enter entire message or character by character:\n");
    while (battle.type == BB_EVENT_NO_EVENT) {
        result = Message_Decode((char) getchar(), &battle);
    }
    if (result) {
        printf("SUCCESS:\nType: %d\nParameters: %d %d %d\n", battle.type, battle.param0, battle.param1, battle.param2);
    } else {
        printf("ERROR NUMBER: %d\n", battle.param0);
    }
    /*    BB_EVENT_CHA_RECEIVED, //3
    BB_EVENT_ACC_RECEIVED, //4
    BB_EVENT_REV_RECEIVED, //5
    BB_EVENT_SHO_RECEIVED, //6
    BB_EVENT_RES_RECEIVED, //7
     
     
     */
    // encode Test:
    // using the given string by user this encodes the message given
    Message message;
    if (battle.type == BB_EVENT_CHA_RECEIVED) {
        message.type = MESSAGE_CHA;
        message.param0 = battle.param0;
    }
    if (battle.type == BB_EVENT_ACC_RECEIVED) {
        message.type = MESSAGE_ACC;
        message.param0 = battle.param0;
    }
    if (battle.type == BB_EVENT_REV_RECEIVED) {
        message.type = MESSAGE_REV;
        message.param0 = battle.param0;
    }
    if (battle.type == BB_EVENT_SHO_RECEIVED) {
        message.type = MESSAGE_SHO;
        message.param0 = battle.param0;
        message.param1 = battle.param1;
    }
    if (battle.type == BB_EVENT_RES_RECEIVED) {
        message.type = MESSAGE_RES;
        message.param0 = battle.param0;
        message.param1 = battle.param1;
        message.param2 = battle.param2;
    }
    char messagestring[MESSAGE_MAX_LEN];
    int length = Message_Encode(messagestring, message);
    printf("Encoded String: %sEncoded length: %d\n", messagestring, length);
    return 1;

}

