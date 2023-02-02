#ifndef MESSAGE_H
#define MESSAGE_H

#define MSG_SIZE 100

enum msg_type
{
    INVALID,
    CONTROL,
    DATA
};

enum control_type
{
    DISCONNECT,
    DISTANCE_VECTOR
};

typedef struct message
{
    enum msg_type type;
    int origin; // whoever wrote the message
    int sender; // whoever sent the message
    int sequence;
    int destiny_id;

    char data[MSG_SIZE];
} message;

#endif