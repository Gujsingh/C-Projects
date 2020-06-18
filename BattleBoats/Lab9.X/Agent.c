/* 
 * Every file you submit should start with a header like this!
 * 
 * File:    Agent.c
 * Author: Gurpinder Singh (gujsingh@ucsc.edu)
 *
 * Created on May 30, 2020, 7:10 PM
 */
// **** Include libraries here ****
// Standard libraries
#include "Agent.h"
#include "Field.h"
#include "FieldOled.h"
#include "Negotiation.h"
#include "Message.h"
#include "BOARD.h"

/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts.  At a minimum, this requires:
 *   -setting the start state of the Agent SM.
 *   -setting turn counter to 0
 * If you are using any other persistent data in Agent SM, that should be reset as well.
 * 
 * It is not advised to call srand() inside of AgentInit.  
 *  */
#define cheater "\nYou are a CHEATER!"
#define loser "\nYOU LOSE\n"
#define winner "\nYOU WIN\n"
#define checkErrorString "\nBB_ERROR_BAD_CHECKSUM\n"
#define PayloadErrorString "\nBB_ERROR_PAYLOAD_LEN_EXCEEDED\n"
#define CheckExtra "\nBB_ERROR_CHECKSUM_LEN_EXCEEDED\n"
#define CheckLess "\nBB_ERROR_CHECKSUM_LEN_INSUFFICIENT\n"
#define badMessageString "\nBB_ERROR_INVALID_MESSAGE_TYPE\n"
#define ParseErrorString "\nBB_ERROR_MESSAGE_PARSE_FAILURE\n"
static Field field;
static Field fieldOther;
static AgentState agentState;
static int numberOfCount = 0;
static NegotiationData NegotiationValues[3];
static FieldOledTurn turnIdentifier = FIELD_OLED_TURN_NONE;
/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts.  At a minimum, this requires:
 *   -setting the start state of the Agent SM.
 *   -setting turn counter to 0
 * If you are using any other persistent data in Agent SM, that should be reset as well.
 * 
 * It is not advised to call srand() inside of AgentInit.  
 *  */
void FieldInitAgent(void);
void CheatingDetected(void);
Message SendAttackShow(void);
void Defeat(void);
void Victory(void);
Message Defend(BB_Event event);
// This function Sends the attack when one is in waiting

Message SendAttackShow(void) {
    GuessData attack = FieldAIDecideGuess(&fieldOther);
    Message messageSend = {MESSAGE_SHO, attack.row, attack.col};
    FieldOledDrawScreen(&field, &fieldOther, turnIdentifier, numberOfCount);
    numberOfCount++;
    agentState = AGENT_STATE_ATTACKING;
    return messageSend;
}
// Defend updates the attack that was recieved

Message Defend(BB_Event event) {
    GuessData guessResult = {event.param0, event.param1, event.param2};
    FieldRegisterEnemyAttack(&field, &guessResult);
    Message messageSend = {MESSAGE_RES, guessResult.row, guessResult.col, guessResult.result};
    FieldOledDrawScreen(&field, &fieldOther, turnIdentifier, numberOfCount);
    agentState = AGENT_STATE_WAITING_TO_SEND;
    return messageSend;
}
// function if cheating is detected

void CheatingDetected(void) {
    agentState = AGENT_STATE_END_SCREEN;
    OledClear(OLED_COLOR_BLACK);
    OledDrawString(cheater);
    OledUpdate();
}
//function that tells us if player has been defeated

void Defeat(void) {
    OledClear(OLED_COLOR_BLACK);
    OledDrawString(loser);
    OledUpdate();
    agentState = AGENT_STATE_END_SCREEN;
}
//function that declares victory

void Victory(void) {
    OledClear(OLED_COLOR_BLACK);
    OledDrawString(winner);
    OledUpdate();
    agentState = AGENT_STATE_END_SCREEN;
}
//initializes all values used

void AgentInit(void) {
    numberOfCount = 0;
    agentState = AGENT_STATE_START;
    NegotiationValues[0] = 0;
    NegotiationValues[1] = 0;
    NegotiationValues[2] = 0;
    turnIdentifier = FIELD_OLED_TURN_NONE;
    FieldInitAgent();
}
//initializes field

void FieldInitAgent(void) {
    FieldInit(&field, &fieldOther);
    FieldAIPlaceAllBoats(&field);
}

/**
 * AgentRun evolves the Agent state machine in response to an event.
 * 
 * @param  The most recently detected event
 * @return Message, a Message struct to send to the opponent. 
 * 
 * If the returned Message struct is a valid message
 * (that is, not of type MESSAGE_NONE), then it will be
 * passed to the transmission module and sent via UART.
 * This is handled at the top level! AgentRun is ONLY responsible 
 * for generating the Message struct, not for encoding or sending it.
 */
Message AgentRun(BB_Event event) {
    Message sendToPlayer;
    sendToPlayer.type = MESSAGE_NONE;
    //declares message default value to message_non
    //no matter what state if there is an error print error message and return string 
    if (event.type == BB_EVENT_ERROR) {
        OledClear(OLED_COLOR_BLACK);
        switch (event.param0) {
            case BB_ERROR_BAD_CHECKSUM:
                OledDrawString(checkErrorString);
                break;
            case BB_ERROR_PAYLOAD_LEN_EXCEEDED:
                OledDrawString(PayloadErrorString);
                break;
            case BB_ERROR_CHECKSUM_LEN_EXCEEDED:
                OledDrawString(CheckExtra);
                break;
            case BB_ERROR_CHECKSUM_LEN_INSUFFICIENT:
                OledDrawString(CheckLess);
                break;
            case BB_ERROR_INVALID_MESSAGE_TYPE:
                OledDrawString(badMessageString);
                break;
            case BB_ERROR_MESSAGE_PARSE_FAILURE:
                OledDrawString(ParseErrorString);
                break;
        }
        OledUpdate();
        return sendToPlayer;
    }
    // no matter what state if restart is pressed restart game
    if (event.type == BB_EVENT_RESET_BUTTON) {
        AgentInit();
        return sendToPlayer;
    }
    // this checks if you have one
    if (FieldGetBoatStates(&fieldOther) == 0) {
        Victory();
    }
    //this checks if you have been defeated
    if (FieldGetBoatStates(&field) == 0) {
        Defeat();
    }
    switch (agentState) {
        case AGENT_STATE_START:
            // if the state is start and event is start button start the oled screen and 
            //start the flipping of the coin through negotiation
            // if it is a cha event set up vallue of param0 and change state to accepting
            switch (event.type) {
                case BB_EVENT_START_BUTTON:
                    FieldOledDrawScreen(&field, &fieldOther, turnIdentifier, numberOfCount);
                    NegotiationValues[0] = rand();
                    sendToPlayer.type = MESSAGE_CHA;
                    NegotiationValues[1] = NegotiationHash(NegotiationValues[0]);
                    sendToPlayer.param0 = NegotiationValues[1];
                    agentState = AGENT_STATE_CHALLENGING;
                    break;
                case BB_EVENT_CHA_RECEIVED:
                    sendToPlayer.param0 = rand();
                    sendToPlayer.type = MESSAGE_ACC;
                    agentState = AGENT_STATE_ACCEPTING;
                    break;
                default:
                    break;
            }
            break;
            // if state is challenging and acc has been recieved for correcct message
            //and flip coin and cahnge state either to waiting or defending
        case AGENT_STATE_CHALLENGING:
            if (event.type == BB_EVENT_ACC_RECEIVED) {
                NegotiationValues[2] = event.param0;
                sendToPlayer.type = MESSAGE_REV;
                sendToPlayer.param0 = NegotiationValues[0];
                if (NegotiateCoinFlip(NegotiationValues[0], NegotiationValues[2])) {
                    agentState = AGENT_STATE_DEFENDING;
                } else {
                    agentState = AGENT_STATE_WAITING_TO_SEND;
                }
            }
            break;
            // state accepting detects cheating and changes state according to flip
        case AGENT_STATE_ACCEPTING:
            if (event.type == BB_EVENT_REV_RECEIVED) {
                if (NegotiationVerify(event.param0, NegotiationValues[1])) {
                    if (NegotiateCoinFlip(NegotiationValues[0], NegotiationValues[2])) {
                        agentState = AGENT_STATE_ATTACKING;
                        return sendToPlayer;
                    } else {
                        turnIdentifier = FIELD_OLED_TURN_THEIRS;
                        agentState = AGENT_STATE_DEFENDING;
                    }
                } else {
                    CheatingDetected();
                }
            }
            break;
        case AGENT_STATE_ATTACKING:
            // if res has been recieved for guess is created and state is changed to deefending 
            if (event.type == BB_EVENT_RES_RECEIVED) {
                GuessData data = {event.param0, event.param1, event.param2};
                FieldUpdateKnowledge(&fieldOther, &data);
                FieldOledDrawScreen(&field, &fieldOther, turnIdentifier, numberOfCount);
                agentState = AGENT_STATE_DEFENDING;
            }
            break;
            //defending is done through a function nd field is updated 
        case AGENT_STATE_DEFENDING:
            if (event.type == BB_EVENT_SHO_RECEIVED) {
                turnIdentifier = FIELD_OLED_TURN_THEIRS;
                FieldOledDrawScreen(&field, &fieldOther, turnIdentifier, numberOfCount);
                return Defend(event);
            }
            break;
            // waiting is where menssage sent is sent and if so guess is sent through SHO
        case AGENT_STATE_WAITING_TO_SEND:
            if (event.type == BB_EVENT_MESSAGE_SENT) {
                return SendAttackShow();
            }
            break;
    }
    return sendToPlayer;
}

/** * 
 * @return Returns the current state that AgentGetState is in.  
 * 
 * This function is very useful for testing AgentRun.
 */
AgentState AgentGetState(void) {
    return agentState;
}

/** * 
 * @param Force the agent into the state given by AgentState
 * 
 * This function is very useful for testing AgentRun.
 */
void AgentSetState(AgentState newState) {
    agentState = newState;
}
