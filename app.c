#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "app.h"
#include "todo.h"

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
                    draw_list(selected_index);
                    refresh();  // refresh the screen after adding a new item
                }
                break;
            case 'd':
                remove_item(selected_index);
                if (selected_index >= num_items) {
                    selected_index--;
                }
                draw_list(selected_index);
                refresh();  // refresh the screen after removing an item
                break;
            case 'q':
                endwin();
                return;
        }
        draw_list(selected_index);
    }
}

