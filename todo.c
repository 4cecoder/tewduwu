#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100
#define MAX_ITEM_LEN 80

typedef struct {
    char text[MAX_ITEM_LEN + 1];
    int done;
    int nested_level;
} todo_item;

todo_item items[MAX_ITEMS] = {0};
int num_items = 0;

void add_item(char* text, int level) {
    if (num_items < MAX_ITEMS) {
        if (level == 0) {
// Top-level item
            strcpy(items[num_items].text, text);
            items[num_items].done = 0;
            items[num_items].nested_level = 0;
            num_items++;
        } else {
// Subtask
            int i;
            for (i = num_items - 1; i >= 0; i--) {
                if (items[i].nested_level == level - 1) {
                    break;
                }
            }
            if (i >= 0) {
// Found parent task, insert after it
                memmove(&items[i+2], &items[i+1], (num_items - i - 1) * sizeof(todo_item));
                strcpy(items[i+1].text, text);
                items[i+1].done = 0;
                items[i+1].nested_level = level;
                num_items++;
            }
        }
    }
}

void remove_item(int index) {
    if (index >= 0 && index < num_items) {
        for (int i = index; i < num_items - 1; i++) {
            items[i] = items[i+1];
        }
        num_items--;
    }
}

void toggle_item(int index) {
    if (index >= 0 && index < num_items) {
        items[index].done = !items[index].done;
    }
}

void draw_list(int selected_index) {
    clear();
    for (int i = 0; i < num_items; i++) {
        if (i == selected_index) {
            attron(A_REVERSE);
        }
        mvprintw(i, items[i].nested_level * 2, "[%c] %s",
                 items[i].done ? 'x' : ' ',
                 items[i].text);
        if (i == selected_index) {
            attroff(A_REVERSE);
        }
    }
    refresh();
}

int get_max_nested_level() {
    int max_level = 0;
    for (int i = 0; i < num_items; i++) {
        if (items[i].nested_level > max_level) {
            max_level = items[i].nested_level;
        }
    }
    return max_level;
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

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
                add_item(buf, 0);
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
                return 0;
            case 'l':
                if (items[selected_index].nested_level < get_max_nested_level()) {
                    for (int i = selected_index + 1; i < num_items; i++) {
                        if (items[i].nested_level <= items[selected_index].nested_level) {
                            break;
                        }
                        if (items[i].nested_level == items[selected_index].nested_level + 1) {
                            selected_index = i;
                            break;
                        }
                    }
                }
                break;
            case 'h':
                if (items[selected_index].nested_level > 0) {
                    int parent = -1;
                    for (int i = selected_index - 1; i >= 0; i--) {
                        if (items[i].nested_level < items[selected_index].nested_level) {
                            parent = i;
                            break;
                        }
                    }
                    if (parent >= 0) {
                        selected_index = parent;
                    }
                }
                break;
            case 'r':
            {
                echo();
                move(selected_index, items[selected_index].nested_level * 2 + 4);
                clrtoeol();
                mvprintw(selected_index, items[selected_index].nested_level * 2, "[%c]", items[selected_index].done ? 'x' : ' ');
                move(selected_index, items[selected_index].nested_level * 2 + 4);
                char buf[MAX_ITEM_LEN + 1];
                getstr(buf);
                strcpy(items[selected_index].text, buf);
                noecho();
            }
                break;
            case 'i':
            {
                char buf[MAX_ITEM_LEN + 1];
                echo();
                move(num_items, 0);
                getstr(buf);
                noecho();
                add_item(buf, items[selected_index].nested_level + 1);
                selected_index = num_items - 1;
            }
                break;
        }
        draw_list(selected_index);
    }
}
