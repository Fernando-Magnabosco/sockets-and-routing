#define MSG_SIZE 100

enum msg_type
{
    INVALID,
    CONTROL,
    DATA
};

typedef struct message
{
    enum msg_type type;
    int source;
    int destiny_port;
    char destiny_ip[16];
    char data[MSG_SIZE];
} message;