# tewduwu - Terminal TODO Manager

A terminal-based hierarchical TODO list manager with rich features.

## Features

- Hierarchical task organization (with nested tasks)
- Priority levels (High, Medium, Low)
- Color-coded interface
- Vim-inspired keybindings
- Task reordering
- Automatic saving

## Getting Started

### Prerequisites

- C compiler (gcc, clang, etc.)
- ncurses library

### Building

```bash
gcc -o tewduwu todo.c -lncurses
```

### Running

```bash
./tewduwu
```

## Usage

### Basic Controls

- `j` or `Down Arrow`: Move cursor down
- `k` or `Up Arrow`: Move cursor up
- `Space`: Toggle task completion
- `a`: Add a new task
- `i`: Insert a subtask under the current task
- `d`: Delete task and all subtasks
- `r`: Rename/edit the current task
- `q`: Quit application
- `?`: Show help screen

### Advanced Controls

- `h`: Go to parent item
- `l`: Go to first child item
- `1`: Set Low priority
- `2`: Set Medium priority
- `3`: Set High priority
- `0`: Clear priority
- `J`: Move task down
- `K`: Move task up

## Configuration

Tasks are saved to `~/.config/tewduwu/todo.txt`.