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
        strcpy(items[num_items].text, text);
        items[num_items].done = 0;
        items[num_items].nested_level = level;
        num_items++;
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
        for (int j = 0; j < items[i].nested_level; j++) {
            mvprintw(i, j * 2, "  ");
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

void save_items(char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp) {
        for (int i = 0; i < num_items; i++) {
            fprintf(fp, "%d,%d,%s\n", items[i].done, items[i].nested_level, items[i].text);
        }
        fclose(fp);
    }
}

void load_items(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        char line[MAX_ITEM_LEN + 2];
        while (fgets(line, sizeof(line), file) != NULL) {
            int len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
            }
            int done, level;
            char text[MAX_ITEM_LEN + 1];
            if (sscanf(line, "%d,%d,%s", &done, &level, text) == 3) {
                add_item(text, level);
            } else if (sscanf(line, "%d,%s", &done, text) == 2) {
                add_item(text, 0);
            }
        }
        fclose(file);
    }
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Load the todos from a file
    load_items("todos.txt");

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
            case 'a': {
                char buf[MAX_ITEM_LEN + 1];
                echo();
                move(num_items, 0);
                getstr(buf);
                noecho();
                add_item(buf, 0);
                selected_index = num_items - 1;
                save_items("todos.txt");
                break;
            }
            case 'd':
                remove_item(selected_index);
                if (selected_index >= num_items) {
                    selected_index--;
                }
                save_items("todos.txt");
                break;
            case 'q':
                endwin();
                save_items("todos.txt");
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
            case 'r': {
                echo();
                move(selected_index, items[selected_index].nested_level * 2 + 4);
                clrtoeol();
                mvprintw(selected_index, items[selected_index].nested_level * 2, "[%c]",
                         items[selected_index].done ? 'x' : ' ');
                move(selected_index, items[selected_index].nested_level * 2 + 4);
                char buf[MAX_ITEM_LEN + 1];
                getstr(buf);
                strcpy(items[selected_index].text, buf);
                noecho();
                save_items("todos.txt");
                break;
            }
            case 'i': {
                char buf[MAX_ITEM_LEN + 1];
                echo();
                move(num_items, 0);
                getstr(buf);
                noecho();
                add_item(buf, items[selected_index].nested_level + 1);
                selected_index = num_items - 1;
                save_items("todos.txt");
                break;
            }
        }
        draw_list(selected_index);
    }

    // Clean up ncurses
    endwin();

    return 0;
}
