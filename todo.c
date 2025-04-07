#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

// Constants
#define MAX_ITEMS 500
#define MAX_ITEM_LEN 120
#define VERSION "1.0.0"
#define APP_NAME "tewduwu"

// Priority levels
#define PRIORITY_NONE 0
#define PRIORITY_LOW 1
#define PRIORITY_MEDIUM 2
#define PRIORITY_HIGH 3

// Colors
#define COLOR_DEFAULT 1
#define COLOR_DONE 2
#define COLOR_LOW_PRIORITY 3
#define COLOR_MEDIUM_PRIORITY 4
#define COLOR_HIGH_PRIORITY 5
#define COLOR_HELP 6
#define COLOR_STATUS 7

// Data structures
typedef struct {
    char text[MAX_ITEM_LEN + 1];
    int done;
    int nested_level;
    int priority;
    time_t due_date;
} todo_item;

// Global variables
todo_item items[MAX_ITEMS] = {0};
int num_items = 0;
int selected_index = 0;
int scroll_offset = 0;
int max_display_lines;
int help_mode = 0;

// Function prototypes
char* get_config_dir(void);
void add_item(const char* text, int level, int priority);
void remove_item(int index);
void toggle_item(int index);
void move_item_up(int index);
void move_item_down(int index);
void change_priority(int index, int new_priority);
void draw_list(int selected_index);
void draw_status_bar(void);
void draw_help_screen(void);
int get_max_nested_level(void);
void save_items(void);
void load_items(void);
void init_colors(void);
void handle_resize(void);
const char* priority_to_string(int priority);
void ensure_selected_visible(void);

// Utility functions
char* get_config_dir(void) {
    char* home_dir = getenv("HOME");
    if (home_dir == NULL) {
        return NULL;
    }

    static char config_dir[PATH_MAX];
    snprintf(config_dir, sizeof(config_dir), "%s/.config/" APP_NAME, home_dir);
    return config_dir;
}

// Item management functions
void add_item(const char* text, int level, int priority) {
    if (num_items >= MAX_ITEMS) {
        return;
    }
    
    strncpy(items[num_items].text, text, MAX_ITEM_LEN);
    items[num_items].text[MAX_ITEM_LEN] = '\0';
    items[num_items].done = 0;
    items[num_items].nested_level = level;
    items[num_items].priority = priority;
    items[num_items].due_date = 0;
    num_items++;
}

void remove_item(int index) {
    if (index < 0 || index >= num_items) {
        return;
    }

    // Remove the item and its children
    int level = items[index].nested_level;
    int i = index + 1;
    while (i < num_items && items[i].nested_level > level) {
        i++;
    }
    
    int num_to_remove = i - index;
    memmove(&items[index], &items[index + num_to_remove], 
            (num_items - index - num_to_remove) * sizeof(todo_item));
    num_items -= num_to_remove;
}

void toggle_item(int index) {
    if (index >= 0 && index < num_items) {
        items[index].done = !items[index].done;
    }
}

void move_item_up(int index) {
    if (index <= 0 || index >= num_items) {
        return;
    }

    // Find the previous item at the same level
    int level = items[index].nested_level;
    int prev_index = index - 1;
    while (prev_index >= 0 && items[prev_index].nested_level > level) {
        prev_index--;
    }

    if (prev_index < 0 || items[prev_index].nested_level != level) {
        return; // No previous item at the same level
    }

    // Count how many items we need to move (including children)
    int i = index + 1;
    while (i < num_items && items[i].nested_level > level) {
        i++;
    }
    int num_to_move = i - index;

    // Store the items we're moving
    todo_item temp[MAX_ITEMS];
    memcpy(temp, &items[index], num_to_move * sizeof(todo_item));

    // Find how many items to skip (previous item and its children)
    i = prev_index + 1;
    while (i < index && items[i].nested_level > items[prev_index].nested_level) {
        i++;
    }
    int skip_count = i - prev_index;

    // Shift items to make room
    memmove(&items[prev_index + num_to_move], &items[prev_index], 
            skip_count * sizeof(todo_item));

    // Move our items to their new position
    memcpy(&items[prev_index], temp, num_to_move * sizeof(todo_item));

    // Update selected index
    selected_index = prev_index;
}

void move_item_down(int index) {
    if (index < 0 || index >= num_items - 1) {
        return;
    }

    // Find the next item at the same level
    int level = items[index].nested_level;
    int i = index + 1;
    while (i < num_items && items[i].nested_level > level) {
        i++;
    }
    
    if (i >= num_items || items[i].nested_level != level) {
        return; // No next item at the same level
    }
    
    int next_index = i;
    
    // Count items to move (next item and its children)
    i = next_index + 1;
    while (i < num_items && items[i].nested_level > level) {
        i++;
    }
    int next_count = i - next_index;
    
    // Count our items (including children)
    i = index + 1;
    while (i < next_index && items[i].nested_level > level) {
        i++;
    }
    int our_count = i - index;
    
    // Store the next items
    todo_item temp[MAX_ITEMS];
    memcpy(temp, &items[next_index], next_count * sizeof(todo_item));
    
    // Move our items down
    memmove(&items[next_index + our_count - next_count], &items[index], 
            our_count * sizeof(todo_item));
    
    // Place the next items where ours were
    memcpy(&items[index], temp, next_count * sizeof(todo_item));
    
    // Update selected index
    selected_index = index + next_count;
}

void change_priority(int index, int new_priority) {
    if (index >= 0 && index < num_items) {
        items[index].priority = new_priority;
    }
}

// UI functions
void init_colors(void) {
    start_color();
    init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_DONE, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_LOW_PRIORITY, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_MEDIUM_PRIORITY, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_HIGH_PRIORITY, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_HELP, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_STATUS, COLOR_BLACK, COLOR_WHITE);
}

void handle_resize(void) {
    endwin();
    refresh();
    clear();
    max_display_lines = LINES - 2;  // Reserve space for status bar
    ensure_selected_visible();
}

void ensure_selected_visible(void) {
    if (selected_index < scroll_offset) {
        scroll_offset = selected_index;
    } else if (selected_index >= scroll_offset + max_display_lines) {
        scroll_offset = selected_index - max_display_lines + 1;
    }
    
    if (scroll_offset < 0) {
        scroll_offset = 0;
    }
}

void draw_list(int selected_index) {
    clear();
    
    if (help_mode) {
        draw_help_screen();
        draw_status_bar();
        refresh();
        return;
    }
    
    ensure_selected_visible();
    
    int display_count = 0;
    for (int i = scroll_offset; i < num_items && display_count < max_display_lines; i++) {
        int y = display_count;
        
        // Choose color based on state and priority
        if (items[i].done) {
            attron(COLOR_PAIR(COLOR_DONE));
        } else {
            switch (items[i].priority) {
                case PRIORITY_HIGH:
                    attron(COLOR_PAIR(COLOR_HIGH_PRIORITY));
                    break;
                case PRIORITY_MEDIUM:
                    attron(COLOR_PAIR(COLOR_MEDIUM_PRIORITY));
                    break;
                case PRIORITY_LOW:
                    attron(COLOR_PAIR(COLOR_LOW_PRIORITY));
                    break;
                default:
                    attron(COLOR_PAIR(COLOR_DEFAULT));
                    break;
            }
        }
        
        // Highlight selected item
        if (i == selected_index) {
            attron(A_REVERSE);
        }
        
        // Clear the line
        move(y, 0);
        clrtoeol();
        
        // Draw the nesting indentation
        for (int j = 0; j < items[i].nested_level; j++) {
            mvprintw(y, j * 2, "│ ");
        }
        
        // Draw checkbox and priority indicator
        const char* priority_str = "";
        if (!items[i].done && items[i].priority > PRIORITY_NONE) {
            priority_str = priority_to_string(items[i].priority);
        }
        
        mvprintw(y, items[i].nested_level * 2, "[%c] %s%s",
                 items[i].done ? 'x' : ' ',
                 priority_str,
                 items[i].text);
        
        // Reset attributes
        if (i == selected_index) {
            attroff(A_REVERSE);
        }
        
        if (items[i].done) {
            attroff(COLOR_PAIR(COLOR_DONE));
        } else {
            switch (items[i].priority) {
                case PRIORITY_HIGH:
                    attroff(COLOR_PAIR(COLOR_HIGH_PRIORITY));
                    break;
                case PRIORITY_MEDIUM:
                    attroff(COLOR_PAIR(COLOR_MEDIUM_PRIORITY));
                    break;
                case PRIORITY_LOW:
                    attroff(COLOR_PAIR(COLOR_LOW_PRIORITY));
                    break;
                default:
                    attroff(COLOR_PAIR(COLOR_DEFAULT));
                    break;
            }
        }
        
        display_count++;
    }
    
    // Draw scrollbar if needed
    if (num_items > max_display_lines) {
        int scrollbar_height = (max_display_lines * max_display_lines) / num_items;
        if (scrollbar_height < 1) scrollbar_height = 1;
        
        int scrollbar_pos = (scroll_offset * max_display_lines) / num_items;
        
        for (int i = 0; i < max_display_lines; i++) {
            mvprintw(i, COLS - 1, " ");
        }
        
        attron(A_REVERSE);
        for (int i = scrollbar_pos; i < scrollbar_pos + scrollbar_height && i < max_display_lines; i++) {
            mvprintw(i, COLS - 1, " ");
        }
        attroff(A_REVERSE);
    }
    
    draw_status_bar();
    refresh();
}

void draw_status_bar(void) {
    int y = LINES - 1;
    attron(COLOR_PAIR(COLOR_STATUS));
    mvhline(y, 0, ' ', COLS);
    
    if (help_mode) {
        mvprintw(y, 0, " ? Help | q Return to list ");
    } else {
        mvprintw(y, 0, " %s v%s | %d/%d items | j/k Move | Space Toggle | a Add | i Insert | d Delete | ? Help ",
                 APP_NAME, VERSION, selected_index + 1, num_items);
    }
    
    attroff(COLOR_PAIR(COLOR_STATUS));
    refresh();
}

void draw_help_screen(void) {
    attron(COLOR_PAIR(COLOR_HELP));
    mvprintw(0, 0, " %s v%s - Help ", APP_NAME, VERSION);
    attroff(COLOR_PAIR(COLOR_HELP));
    
    int y = 2;
    mvprintw(y++, 2, "Navigation:");
    mvprintw(y++, 4, "j, DOWN          Move cursor down");
    mvprintw(y++, 4, "k, UP            Move cursor up");
    mvprintw(y++, 4, "h                Go to parent item");
    mvprintw(y++, 4, "l                Go to first child item");
    y++;
    
    mvprintw(y++, 2, "Task Management:");
    mvprintw(y++, 4, "a                Add a new task");
    mvprintw(y++, 4, "i                Insert a subtask under the current task");
    mvprintw(y++, 4, "d                Delete task (and all subtasks)");
    mvprintw(y++, 4, "r                Rename/edit the current task");
    mvprintw(y++, 4, "SPACE            Toggle task completion status");
    mvprintw(y++, 4, "1,2,3            Set priority (1=Low, 2=Medium, 3=High)");
    mvprintw(y++, 4, "0                Clear priority");
    mvprintw(y++, 4, "J                Move task down");
    mvprintw(y++, 4, "K                Move task up");
    y++;
    
    mvprintw(y++, 2, "Other:");
    mvprintw(y++, 4, "?                Toggle this help screen");
    mvprintw(y++, 4, "q                Quit (from help) or Exit application");
    
    refresh();
}

const char* priority_to_string(int priority) {
    switch (priority) {
        case PRIORITY_HIGH:
            return "[H] ";
        case PRIORITY_MEDIUM:
            return "[M] ";
        case PRIORITY_LOW:
            return "[L] ";
        default:
            return "";
    }
}

// File operations
void save_items(void) {
    char* config_dir = get_config_dir();
    if (config_dir == NULL) {
        return;
    }

    struct stat st = {0};
    if (stat(config_dir, &st) == -1) {
        if (mkdir(config_dir, 0700) != 0) {
            return;
        }
    }

    char filename[PATH_MAX];
    snprintf(filename, sizeof(filename), "%s/todo.txt", config_dir);

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        return;
    }

    // Format: done,level,priority,due_date,text
    for (int i = 0; i < num_items; i++) {
        fprintf(fp, "%d,%d,%d,%ld,%s\n", 
                items[i].done, 
                items[i].nested_level, 
                items[i].priority,
                items[i].due_date,
                items[i].text);
    }
    
    fclose(fp);
}

void load_items(void) {
    char* config_dir = get_config_dir();
    if (config_dir == NULL) {
        return;
    }

    struct stat st = {0};
    if (stat(config_dir, &st) == -1) {
        if (mkdir(config_dir, 0700) != 0) {
            return;
        }
    }

    char filename[PATH_MAX];
    snprintf(filename, sizeof(filename), "%s/todo.txt", config_dir);

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        return;
    }

    num_items = 0;
    char line[MAX_ITEM_LEN + 50]; // Extra space for metadata

    while (fgets(line, sizeof(line), fp) != NULL && num_items < MAX_ITEMS) {
        // Remove trailing newline
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        int done, level, priority;
        time_t due_date;
        char text[MAX_ITEM_LEN + 1];

        // Try the new format first
        if (sscanf(line, "%d,%d,%d,%ld,%120[^\n]", &done, &level, &priority, &due_date, text) == 5) {
            add_item(text, level, priority);
            items[num_items-1].done = done;
            items[num_items-1].due_date = due_date;
        }
        // Try old format without priority and due date
        else if (sscanf(line, "%d,%d,%120[^\n]", &done, &level, text) == 3) {
            add_item(text, level, PRIORITY_NONE);
            items[num_items-1].done = done;
        }
        // Try very old format without nesting level
        else if (sscanf(line, "%d,%120[^\n]", &done, text) == 2) {
            add_item(text, 0, PRIORITY_NONE);
            items[num_items-1].done = done;
        }
    }

    fclose(fp);
}

int get_max_nested_level(void) {
    int max_level = 0;
    for (int i = 0; i < num_items; i++) {
        if (items[i].nested_level > max_level) {
            max_level = items[i].nested_level;
        }
    }
    return max_level;
}

int main(void) {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);  // Hide cursor
    
    if (has_colors()) {
        init_colors();
    }
    
    // Set up the screen
    max_display_lines = LINES - 2;  // Reserve space for status bar
    
    // Load the todos from a file
    load_items();
    
    // Initial draw
    draw_list(selected_index);
    
    // Main loop
    int running = 1;
    while (running) {
        int ch = getch();
        
        if (help_mode) {
            // In help mode, only respond to a few keys
            switch (ch) {
                case '?':
                case 'q':
                    help_mode = 0;
                    break;
                case KEY_RESIZE:
                    handle_resize();
                    break;
            }
        } else {
            // Normal mode
            switch (ch) {
                case KEY_UP:
                case 'k':
                    if (selected_index > 0) {
                        selected_index--;
                    } else if (num_items > 0) {
                        selected_index = num_items - 1;
                    }
                    break;
                    
                case KEY_DOWN:
                case 'j':
                    if (selected_index < num_items - 1) {
                        selected_index++;
                    } else if (num_items > 0) {
                        selected_index = 0;
                    }
                    break;
                    
                case ' ':
                    toggle_item(selected_index);
                    save_items();
                    break;
                    
                case 'a': {
                    // Add new task at root level
                    echo();
                    curs_set(1);
                    
                    // Create input area at the bottom
                    attron(COLOR_PAIR(COLOR_STATUS));
                    mvhline(LINES - 1, 0, ' ', COLS);
                    mvprintw(LINES - 1, 0, " New task: ");
                    attroff(COLOR_PAIR(COLOR_STATUS));
                    
                    char buf[MAX_ITEM_LEN + 1] = {0};
                    mvgetnstr(LINES - 1, 11, buf, MAX_ITEM_LEN);
                    
                    if (strlen(buf) > 0) {
                        add_item(buf, 0, PRIORITY_NONE);
                        selected_index = num_items - 1;
                        save_items();
                    }
                    
                    noecho();
                    curs_set(0);
                    break;
                }
                
                case 'i': {
                    // Insert subtask
                    if (num_items <= 0) {
                        break;
                    }
                    
                    echo();
                    curs_set(1);
                    
                    // Create input area at the bottom
                    attron(COLOR_PAIR(COLOR_STATUS));
                    mvhline(LINES - 1, 0, ' ', COLS);
                    mvprintw(LINES - 1, 0, " New subtask: ");
                    attroff(COLOR_PAIR(COLOR_STATUS));
                    
                    char buf[MAX_ITEM_LEN + 1] = {0};
                    mvgetnstr(LINES - 1, 14, buf, MAX_ITEM_LEN);
                    
                    if (strlen(buf) > 0) {
                        add_item(buf, items[selected_index].nested_level + 1, PRIORITY_NONE);
                        selected_index = num_items - 1;
                        save_items();
                    }
                    
                    noecho();
                    curs_set(0);
                    break;
                }
                
                case 'd': {
                    // Delete item and its children
                    if (num_items <= 0) {
                        break;
                    }
                    
                    // Confirm deletion
                    attron(COLOR_PAIR(COLOR_STATUS));
                    mvhline(LINES - 1, 0, ' ', COLS);
                    mvprintw(LINES - 1, 0, " Delete this task and all subtasks? (y/n): ");
                    attroff(COLOR_PAIR(COLOR_STATUS));
                    
                    int confirm = getch();
                    if (confirm == 'y' || confirm == 'Y') {
                        remove_item(selected_index);
                        if (selected_index >= num_items && num_items > 0) {
                            selected_index = num_items - 1;
                        }
                        save_items();
                    }
                    break;
                }
                
                case 'r': {
                    // Rename/edit item
                    if (num_items <= 0) {
                        break;
                    }
                    
                    echo();
                    curs_set(1);
                    
                    // Show the current text for editing
                    attron(COLOR_PAIR(COLOR_STATUS));
                    mvhline(LINES - 1, 0, ' ', COLS);
                    mvprintw(LINES - 1, 0, " Edit: ");
                    attroff(COLOR_PAIR(COLOR_STATUS));
                    
                    char buf[MAX_ITEM_LEN + 1];
                    strncpy(buf, items[selected_index].text, MAX_ITEM_LEN);
                    buf[MAX_ITEM_LEN] = '\0';
                    
                    mvgetnstr(LINES - 1, 7, buf, MAX_ITEM_LEN);
                    
                    if (strlen(buf) > 0) {
                        strncpy(items[selected_index].text, buf, MAX_ITEM_LEN);
                        items[selected_index].text[MAX_ITEM_LEN] = '\0';
                        save_items();
                    }
                    
                    noecho();
                    curs_set(0);
                    break;
                }
                
                case 'h':
                    // Go to parent item
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
                    
                case 'l':
                    // Go to first child item
                    if (selected_index < num_items - 1) {
                        if (items[selected_index + 1].nested_level > items[selected_index].nested_level) {
                            selected_index++;
                        }
                    }
                    break;
                    
                case 'K':
                    // Move item up
                    move_item_up(selected_index);
                    save_items();
                    break;
                    
                case 'J':
                    // Move item down
                    move_item_down(selected_index);
                    save_items();
                    break;
                    
                case '0':
                    // Clear priority
                    change_priority(selected_index, PRIORITY_NONE);
                    save_items();
                    break;
                    
                case '1':
                    // Set low priority
                    change_priority(selected_index, PRIORITY_LOW);
                    save_items();
                    break;
                    
                case '2':
                    // Set medium priority
                    change_priority(selected_index, PRIORITY_MEDIUM);
                    save_items();
                    break;
                    
                case '3':
                    // Set high priority
                    change_priority(selected_index, PRIORITY_HIGH);
                    save_items();
                    break;
                    
                case '?':
                    // Show help screen
                    help_mode = 1;
                    break;
                    
                case 'q':
                    // Quit application
                    running = 0;
                    break;
                    
                case KEY_RESIZE:
                    handle_resize();
                    break;
            }
        }
        
        draw_list(selected_index);
    }
    
    // Save before exit
    save_items();
    
    // Clean up ncurses
    endwin();
    
    return 0;
}