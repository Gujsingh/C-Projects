/* 
 * Every file you submit should start with a header like this!
 * 
 * File:    Message.c
 * Author: Gurpinder Singh (gujsingh@ucsc.edu)
 *
 * Created on May 30, 2020, 7:10 PM
 */
// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "BOARD.h"
#include "Message.h"
#include "Negotiation.h"
//enum states
#define messageIdLength 3
#define HexStringConvert  "0x%s"
#define emptyString ""
#define FullHexLength 5
#define Comma ","
#define nullString "\0"
#define zeroString "0"
#define ChaString "CHA"
#define AccString "ACC"
#define RevString "REV"
#define ResString "RES"
#define ShoString "SHO"
#define CashChar  '$'
#define nullchar '\0'
#define lineChar '\n'
#define StarChar '*'
#define zeroChar '0'
#define nineChar '9'
#define AChar 'A'
#define FChar 'F'

typedef enum {
    DELIMITER,
    PAYLOAD,
    CHECKSUM
} DecodeStateMachine;

static DecodeStateMachine decodeState = DELIMITER;
static char payLoadDecode[MESSAGE_MAX_PAYLOAD_LEN];
static char checkSumDecode[MESSAGE_CHECKSUM_LEN];
static int checkindex = 0;
static int payindex = 0;
// function defining
uint8_t ConvertHex(const char *checksum_string);

uint8_t ConvertHex(const char *checksum_string) {
    // turn hex into sting and use strtol to convert to number
    char hex[FullHexLength] = emptyString;
    sprintf(hex, HexStringConvert, checksum_string);
    int number = (int) strtol(hex, NULL, 0);
    return (uint8_t) number;
}

/**
 * Given a payload string, calculate its checksum
 * 
 * @param payload       //the string whose checksum we wish to calculate
 * @return   //The resulting 8-bit checksum 
 */
uint8_t Message_CalculateChecksum(const char* payload) {
    // use cchecksum formula to calculate the checksum through xoring values
    int len = strlen(payload), i = 0;
    uint8_t sum = 0;
    while (i != len) {
        sum ^= payload[i];
        i++;
    }
    return sum;
}

/**
 * ParseMessage() converts a message string into a BB_Event.  The payload and
 * checksum of a message are passed into ParseMessage(), and it modifies a
 * BB_Event struct in place to reflect the contents of the message.
 * 
 * @param payload       //the payload of a message
 * @param checksum      //the checksum (in string form) of  a message,
 *                          should be exactly 2 chars long, plus a null char
 * @param message_event //A BB_Event which will be modified by this function.
 *                      //If the message could be parsed successfully,
 *                          message_event's type will correspond to the message type and 
 *                          its parameters will match the message's data fields.
 *                      //If the message could not be parsed,
 *                          message_events type will be BB_EVENT_ERROR
 * 
 * @return STANDARD_ERROR if:
 *              the payload does not match the checksum
 *              the checksum string is not two characters long
 *              the message does not match any message template
 *          SUCCESS otherwise
 * 
 * Please note!  sscanf() has a couple compiler bugs that make it a very
 * unreliable tool for implementing this function. * 
 */

//typedef enum {
//    BB_SUCCESS = 0, //0
//    BB_ERROR_BAD_CHECKSUM, //1 check 
//    BB_ERROR_PAYLOAD_LEN_EXCEEDED, //2  check
//    BB_ERROR_CHECKSUM_LEN_EXCEEDED, //3 check 
//    BB_ERROR_CHECKSUM_LEN_INSUFFICIENT, //3 check
//    BB_ERROR_INVALID_MESSAGE_TYPE, //4
//    BB_ERROR_MESSAGE_PARSE_FAILURE,
//} BB_Error;

int Message_ParseMessage(const char* payload,
        const char* checksum_string, BB_Event * message_event) {
    // load copy since strok changes char value
    char payloadCopy[MESSAGE_MAX_PAYLOAD_LEN];
    strcpy(payloadCopy, payload);
    //strlen of both payload and checksum and check if they are valid 
    int payLoadLen = strlen(payloadCopy), checkSumLen = strlen(checksum_string);
    if (payLoadLen > MESSAGE_MAX_PAYLOAD_LEN) {
        message_event->param0 = BB_ERROR_PAYLOAD_LEN_EXCEEDED;
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    if (checkSumLen != MESSAGE_CHECKSUM_LEN) {
        if (checkSumLen > MESSAGE_CHECKSUM_LEN) {
            message_event->param0 = BB_ERROR_CHECKSUM_LEN_EXCEEDED;
        } else {
            message_event->param0 = BB_ERROR_CHECKSUM_LEN_INSUFFICIENT;
        }
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    //check if the the check sum is the same as the payload 
    if (Message_CalculateChecksum(payloadCopy) != ConvertHex(checksum_string)) {
        message_event->param0 = BB_ERROR_BAD_CHECKSUM;
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    //check if value is a corect check sum digit
    if (isxdigit(checksum_string[0]) == 0 || isxdigit(checksum_string[1]) == 0) {
        message_event->param0 = BB_ERROR_BAD_CHECKSUM;
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    //tokenize first header
    char* token = strtok(payloadCopy, Comma);
    if (strcmp(token, ChaString) == 0) {
        //tokenize par 0 and check if corect value
        token = strtok(NULL, nullString);
        //check if token is null
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            //change token from string to integer
            message_event->param0 = atoi(token);
            message_event->type = BB_EVENT_CHA_RECEIVED;
            return SUCCESS;
        } else {
            //if token is null return err
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
        //similar format for the rest except different amounts of parm
    } else if (strcmp(token, AccString) == 0) {
        token = strtok(NULL, nullString);
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            message_event->param0 = atoi(token);
            message_event->type = BB_EVENT_ACC_RECEIVED;
            return SUCCESS;
        } else {
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
    } else if (strcmp(token, RevString) == 0) {
        token = strtok(NULL, nullString);
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            message_event->param0 = atoi(token);
            message_event->type = BB_EVENT_REV_RECEIVED;
            return SUCCESS;
        } else {
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }

    } else if (strcmp(token, ShoString) == 0) {
        token = strtok(NULL, Comma);
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            message_event->param0 = atoi(token);
        } else {
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
        token = strtok(NULL, nullString);
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            message_event->param1 = atoi(token);
            message_event->type = BB_EVENT_SHO_RECEIVED;
            return SUCCESS;
        } else {
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
    } else if (strcmp(token, ResString) == 0) {
        token = strtok(NULL, Comma);
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            message_event->param0 = atoi(token);
        } else {
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
        token = strtok(NULL, Comma);
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            message_event->param1 = atoi(token);
        } else {
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
        token = strtok(NULL, nullString);
        if (token != NULL) {
            if (atoi(token) == 0 && strcmp(token, zeroString) == 0) {
                message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
                message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            message_event->param2 = atoi(token);
            message_event->type = BB_EVENT_RES_RECEIVED;
            return SUCCESS;
        } else {
            message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
            message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
    } else {
        message_event->param0 = BB_ERROR_MESSAGE_PARSE_FAILURE;
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    return STANDARD_ERROR;

}

/**
 * Encodes the coordinate data for a guess into the string `message`. This string must be big
 * enough to contain all of the necessary data. The format is specified in PAYLOAD_TEMPLATE_COO,
 * which is then wrapped within the message as defined by MESSAGE_TEMPLATE. 
 * 
 * The final length of this
 * message is then returned. There is no failure mode for this function as there is no checking
 * for NULL pointers.
 * 
 * @param message            The character array used for storing the output. 
 *                              Must be long enough to store the entire string,
 *                              see MESSAGE_MAX_LEN.
 * @param message_to_encode  A message to encode
 * @return                   The length of the string stored into 'message_string'.
                             Return 0 if message type is MESSAGE_NONE.
 */
int Message_Encode(char *message_string, Message message_to_encode) {
    // declare variables
    char payload_string[MESSAGE_MAX_PAYLOAD_LEN];
    MessageType type = message_to_encode.type;
    // using sprintf compose correct encode value
    if (message_to_encode.type == MESSAGE_NONE) {
        return 0;
    } else if (type == MESSAGE_CHA) {
        sprintf(payload_string, PAYLOAD_TEMPLATE_CHA, message_to_encode.param0);
    } else if (type == MESSAGE_ACC) {
        sprintf(payload_string, PAYLOAD_TEMPLATE_CHA, message_to_encode.param0);
    } else if (type == MESSAGE_REV) {
        sprintf(payload_string, PAYLOAD_TEMPLATE_REV, message_to_encode.param0);
    } else if (type == MESSAGE_SHO) {
        sprintf(payload_string, PAYLOAD_TEMPLATE_SHO, message_to_encode.param0, message_to_encode.param1);
    } else if (type == MESSAGE_RES) {
        sprintf(payload_string, PAYLOAD_TEMPLATE_RES, message_to_encode.param0, message_to_encode.param1, message_to_encode.param2);
    }
    // correct full message string using tempalte given and return length
    sprintf(message_string, MESSAGE_TEMPLATE, payload_string, Message_CalculateChecksum(payload_string));
    return strlen(message_string);
}

/**
 * Message_Decode reads one character at a time.  If it detects a full NMEA message,
 * it translates that message into a BB_Event struct, which can be passed to other 
 * services.
 * 
 * @param char_in - The next character in the NMEA0183 message to be decoded.
 * @param decoded_message - a pointer to a message struct, used to "return" a message
 *                          if char_in is the last character of a valid message, 
 *                              then decoded_message
 *                              should have the appropriate message type.
 *                          if char_in is the last character of an invalid message,
 *                              then decoded_message should have an ERROR type.
 *                          otherwise, it should have type NO_EVENT.
 * @return SUCCESS if no error was detected
 *         STANDARD_ERROR if an error was detected
 * 
 * note that ANY call to Message_Decode may modify decoded_message.
 */
int Message_Decode(unsigned char char_in, BB_Event * decoded_message_event) {
    // switch statement to decifer which state the machine is in
    switch (decodeState) {
        case DELIMITER:
            // if char in is $ then mvoe on to next else produce error
            if (char_in == CashChar) {
                checkindex = 0;
                payindex = 0;
                memset(payLoadDecode, nullchar, MESSAGE_MAX_PAYLOAD_LEN);
                memset(checkSumDecode, nullchar, MESSAGE_CHECKSUM_LEN);
                decodeState = PAYLOAD;
                decoded_message_event->type = BB_EVENT_NO_EVENT;
                return SUCCESS;
            } else {
                decoded_message_event->param0 = BB_ERROR_INVALID_MESSAGE_TYPE;
                decoded_message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
            break;
        case PAYLOAD:
            // check if values are correct 
            if (char_in == CashChar || char_in == lineChar) {
                decoded_message_event->param0 = BB_ERROR_INVALID_MESSAGE_TYPE;
                decoded_message_event->type = BB_EVENT_ERROR;
                decodeState = DELIMITER;
                return STANDARD_ERROR;
            }// if char in is * move onto next 
            else if (char_in == StarChar) {
                decodeState = CHECKSUM;
                decoded_message_event->type = BB_EVENT_NO_EVENT;
                return SUCCESS;
            }// produce error if payload is to large
            else if (strlen(payLoadDecode) == MESSAGE_MAX_PAYLOAD_LEN) {
                decoded_message_event->type = BB_EVENT_ERROR;
                decoded_message_event->param0 = BB_ERROR_PAYLOAD_LEN_EXCEEDED;
                decodeState = DELIMITER;
                return STANDARD_ERROR;
            }// record value
            else {
                payLoadDecode[payindex] = char_in;
                payindex++;
                decoded_message_event->type = BB_EVENT_NO_EVENT;
                return SUCCESS;
            }
            break;
        case CHECKSUM:
            // check if charin /n to finnish check sum recording 
            if (char_in != lineChar) {
                // check for error else record char in in checksum
                if (strlen(checkSumDecode) == MESSAGE_CHECKSUM_LEN) {
                    decoded_message_event->type = BB_EVENT_ERROR;
                    decoded_message_event->param0 = BB_ERROR_CHECKSUM_LEN_EXCEEDED;
                    decodeState = DELIMITER;
                    return STANDARD_ERROR;
                } else if ((char_in >= zeroChar && char_in <= nineChar) || (char_in >= AChar && char_in <= FChar)) {
                    checkSumDecode[checkindex] = char_in;
                    checkindex++;
                    decoded_message_event->type = BB_EVENT_NO_EVENT;
                    return SUCCESS;
                } else {
                    decoded_message_event->type = BB_EVENT_ERROR;
                    decoded_message_event->param0 = BB_ERROR_INVALID_MESSAGE_TYPE;
                    decodeState = DELIMITER;
                    return STANDARD_ERROR;
                }
            } else {
                // parse message and check if any errors or correct parsing
                decodeState = DELIMITER;
                return Message_ParseMessage(payLoadDecode, checkSumDecode, decoded_message_event);
            }
            break;
    }
    return STANDARD_ERROR;

}

