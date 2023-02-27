#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 100
#define MAX_ITEM_LEN 80

// Declare struct to hold todo items
typedef struct {
    char text[MAX_ITEM_LEN + 1];
    int done;
    short *colors;
} todo_item;

// Initialize todo items array
todo_item items[MAX_ITEMS] = {0};
int num_items = 0;

// Function to add a new item
void add_item(char* text) {
    if (num_items < MAX_ITEMS) {
        strcpy(items[num_items].text, text);
        items[num_items].done = 0;
        items[num_items].colors = calloc((MAX_ITEM_LEN + 1), sizeof(short));
        srand(time(NULL));
        for (int i = 0; i <= MAX_ITEM_LEN; i++) {
            short color;
            do {
                color = (rand() % (COLORS - 1)) + 1;
            } while (color == COLOR_BLACK);
            items[num_items].colors[i] = color;
        }
        num_items++;
    }
}

// Function to remove an item
void remove_item(int index) {
    if (index >= 0 && index < num_items) {
        free(items[index].colors);
        for (int i = index; i < num_items - 1; i++) {
            items[i] = items[i+1];
        }
        num_items--;
    }
}

// Function to toggle an item's done status
void toggle_item(int index) {
    if (index >= 0 && index < num_items) {
        items[index].done = !items[index].done;
    }
}

// Function to draw the list of items
void draw_list(int selected_index) {
    clear();
    for (int i = 0; i < num_items; i++) {
        if (i == selected_index) {
            attron(A_REVERSE);
        }
        for (int j = 0; j < strlen(items[i].text); j++) {
            attron(COLOR_PAIR(items[i].colors[j]));
            mvprintw(i, j, "%c", items[i].text[j]);
            attroff(COLOR_PAIR(items[i].colors[j]));
        }
        if (i == selected_index) {
            attroff(A_REVERSE);
        }
    }
    refresh();
}

// Function to run the todo app
void run_todo_app() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);

    // Main loop
    int selected_index = 0;
    draw_list(selected_index);
    while (1) {
        int ch = getch();
        switch (ch) {
            case KEY_UP:
            case 'k':
                selected_index--;
                if (selected_index < 0) {
                    selected_index = num_items - 1;
                }
                break;
            case KEY_DOWN:
            case 'j':
                selected_index++;
                if (selected_index >= num_items) {
                    selected_index = 0;
                }
                break;
            case ' ':
                toggle_item(selected_index);
                break;
            case 'a':
                {
                    char buf[MAX_ITEM_LEN + 1];
                    echo();
                    move(num_items, 0);
                    getstr(buf);
                    noecho();
                    add_item(buf);
                    selected_index = num_items - 1;
                }
                break;
            case 'd':
                remove_item(selected_index);
                if (selected_index >= num_items) {
                    selected_index--;
                }
                break;
            case 'q':
                endwin();
                return;
        }
        draw_list(selected_index);
    }
}
