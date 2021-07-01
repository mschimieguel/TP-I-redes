#ifndef list_H
#define list_H

#define POINTSZ 20

typedef struct node{
	int _X;
    int _Y;
	struct node *next;
} node_t;

typedef struct list{
	unsigned size;
	node_t *head;
	node_t *tail;		
}list_t;

list_t *create_list();

void add_start(list_t *list,int _X,int _Y);
void add_end(list_t *list,int _X,int _Y);
void add(list_t *list,int _X,int _Y,int pos);

int remove_start(list_t *list);
int remove_end(list_t *list);
int remove_element(list_t *list,int pos);

int search(list_t *list, int _X,int _Y);

void delete_list(list_t *list);
void clean(list_t *list);

void print_list(list_t *list);
void sprint_list(char *buffer, list_t *list);

int acess(list_t *list, int pos);

/* void snearest(char *buffer, int pos, list_t *list,int  _X, int _Y);
int distance(int _X1, int _Y1, int _X2, int _Y2);
 */
#endif