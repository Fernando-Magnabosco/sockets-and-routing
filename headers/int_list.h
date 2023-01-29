typedef struct node
{
    int value;
    struct node *next;

} int_list;

int_list *init_int_list();
int_list *add_int(int_list *list, int value);
int_list *remove_int(int_list *list, int value);
