#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "list.h"

list_t *create_list(){
    //retorna uma list vazia alocada no heap
    list_t *list = (list_t*) malloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list; 
}
void add_start(list_t *list, int _X, int _Y){
    //alocando o no novo no heap
    node_t *new_node=(node_t*)malloc(sizeof(node_t));
    
    //atribuindo as caracteristicas do novo no e da list
    new_node->_X = _X;
    new_node->_Y = _Y;
    new_node->next = list->head;
    list->size++;
    

    if (list->size == 0){
        //empty list
        list->head=new_node;
        list->tail=new_node;
    }
    else {
        //size > 1
        new_node -> next = list->head;
        list->head=new_node;    
    }    
}
void add_end(list_t *list, int _X, int _Y){
    //alocando no heap o novo no
	node_t *new_node=(node_t*)malloc(sizeof(node_t));

    //atribuindo as caracteristicas do novo no e da list
    new_node->_X= _X;
    new_node->_Y= _Y;
    new_node->next = NULL;
    list->size++;
    
    if (list->head == NULL){
        //empty list
        list->head=new_node;
        list->tail=new_node;
    }
    else {
        //size > 1
        list->tail->next = new_node;
        list->tail=new_node;    
    }    
}
void add(list_t *list, int _X, int _Y, int pos){       
    if (pos > 0 && pos < list->size-1){
        node_t *to_visit=list->head;
        for (int i = 0;i < pos-1;i++){
            to_visit=to_visit->next;
        }
        node_t *ultrapassado = to_visit->next;
        node_t *new_node=(node_t*)malloc(sizeof(node_t));
        to_visit->next=new_node;
        new_node->next=ultrapassado;
        new_node->_X = _X;
        new_node->_Y = _Y;
        list->size++;
    }
    else if (pos == 0)
        add_start(list,_X,_Y);
    else if (pos == list->size)
        add_end(list,_X,_Y);
    else 
        printf("position not allowed\n");
     
}
int remove_start(list_t *list){
    
    if (list->size == 0)
        return -1;

    node_t *to_free = list->head;
    int to_return = to_free->_X;
    list->head = list->head->next;
    list->size--;

    free(to_free);
    return to_return;
}
int remove_end(list_t *list){
    
    if (list->size == 0)
        return -1;
    node_t *previus = list->head;
    while(previus->next != list->tail)
        previus = previus->next;
    
    node_t *to_free=previus->next;
    int to_return=to_free->_X;
    
    list->tail = previus;
    previus->next = NULL;
    free(to_free);
    list->size--;
    return to_return;
}
int remove_element(list_t *list, int pos){
    if (pos < 0){
        return -1;
    }
    else if (pos > 0 && pos < list->size - 1){
        node_t *to_visit =list->head;
        node_t *to_free = NULL;
        for (int i = 0;i < pos-1;i++)
            to_visit=to_visit->next;
        to_free = to_visit->next;
        to_visit->next = to_free->next;
        list->size--;
        int to_return = to_free->_X;
        free(to_free);
        return to_return;
    }    
    else if (pos==0)
        return remove_start(list);
    else if (pos == list->size - 1)
        return remove_end(list);
    else {
        //printf ("non-existent or not allowed position\n");
        return -2;
    } 
        
}
void delete_list(list_t *list){
    while (list->head != NULL){
        remove_start(list);
    }
    free(list);    
}
void clean(list_t *list){
   while (list->head != NULL){
         remove_start(list);
    } 
}
void print_list(list_t *list){
    if (list->size > 0){
        printf("list => size == %d; elements == { ", list->size);
        node_t *atual=list->head;
        while (atual != NULL){
            if(atual == list->head )
                printf("(%d,%d) ", atual->_X, atual->_Y);
            else
                printf(", (%d,%d) ", atual->_X, atual->_Y);
            atual = atual->next;
        }
        printf("};\n");
    }
    else {
       printf("empty list \n");    
       //printf("empty\n");
    }
        
    //printf("size == 0; elementos == {}\n");    
}
void sprint_list(char *buffer, list_t *list){
     if (list->size != 0){
        //printf("list => size == %d; elements == { ",list->size);
        node_t *atual = list->head;
        char point[POINTSZ];
        while (atual != NULL){
            memset(point, 0, POINTSZ);
            if (atual == list->head)
                sprintf(point, "%d %d", atual->_X, atual->_Y);
            else
                sprintf(point, " %d %d", atual->_X, atual->_Y);
            strcat(buffer,point);
            atual = atual->next;
        }
    }
    else 
        sprintf(buffer,"“none”");
}

int search(list_t *list, int _X, int _Y){
	if (list->size == 0){
		//printf("Error, empty list!!");
    	//exit(1);
        return -1;
	}
    int pos = 0;
    node_t *atual = list->head;
	while(atual != NULL && (atual->_X != _X || atual->_Y != _Y) ){
		pos++;
        atual = atual->next;
	}
	if (atual != NULL)
		return pos;
	//atual == NULL e nesse caso nao existe o elemento procurado
	return -1;
}
int acess(list_t *list, int pos){
     if (pos >= 0 && pos <= list->size - 1){
        
        node_t *to_visit = list->head;
        for (int i = 0;i < pos;i++)
            to_visit=to_visit->next;
  
        return to_visit->_X;
    }    
    else {
        printf ("non-existent or not allowed position\n");
        return -1;
    } 
}

void snearest(char *buffer, list_t *list, int  _X, int _Y){
    if(list->size > 0){
         int min = INT_MAX;
        node_t *atual = list->head;
	    while( atual != NULL ){
            if (distance(atual->_X, atual->_Y, _X, _Y) < min){
                min = distance(atual->_X, atual->_Y, _X, _Y);
                sprintf(buffer, "%d %d", atual->_X, atual->_Y);
            }
            atual = atual->next;
	    }
    }
    else{
        sprintf(buffer,"“none”");
    }
}
int distance(int _X1, int _Y1, int _X2, int _Y2){
    return sqrt( ((_X2 -_X1)*(_X2 -_X1)) + ((_Y2 -_Y1)*(_Y2 -_Y1))  );
}