#define MAX_NEIGHBORS 16

typedef struct neighbor
{
    int id;
    int port;
    int cost;
    char ip[16];

} neighbor;
