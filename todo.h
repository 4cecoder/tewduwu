#ifndef TODO_H
#define TODO_H

#include <ncurses.h>

#define MAX_ITEMS 100
#define MAX_ITEM_LEN 80

typedef struct {
    char text[MAX_ITEM_LEN + 1];
    int done;
    short *colors;
} todo_item;

extern todo_item items[MAX_ITEMS];
extern int num_items;

void add_item(char* text);
void remove_item(int index);

#endif

