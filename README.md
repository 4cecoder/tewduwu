# tewduwu

Ohaiiii! UwU nuzzles Are you weady to be pwodwuctive today? Let's manage our tasks togethew with tewduwu!

A terminal-based hierarchical TODO list manager with rich features and kawaii vibes!

https://user-images.githubusercontent.com/88108711/221506625-3c68cf84-b84e-4d29-adc8-3ac1536d85d9.mp4

## Features

- 📝 Hierarchical task organization (with nested tasks)
- 🎯 Priority levels (High, Medium, Low)
- 🌈 Color-coded interface
- ⌨️ Vim-inspired keybindings
- 🔄 Task reordering
- 💾 Automatic saving
- 📱 Status bar with helpful commands
- 📊 Visual connection lines for nested tasks
- 🖱️ Scrolling for long lists
- ❓ Help screen with all commands

## Getting Started

### Prerequisites

- C compiler (gcc, clang, etc.)
- ncurses library

### Building

```bash
gcc -o tewduwu todo.c -lncurses
```

Or use the provided build scripts:

```bash
# For Linux
./linux_build.sh

# For macOS
./macos_build.sh
```

### Installing

Run the install script to make tewduwu available system-wide:

```bash
sudo ./install.sh
```

## Contwols:

### Navigation
- `up` and `down` arrow keys or `k` and `j` keys to move the cuwrsor between tasks in the task list
- `h` key to 🔙 move the selected task to its parent task
- `l` key to ➡️ move the selected task to its first child task

### Task Management
- `space bar` to ☑️ mark or unmark the currently selected task as complete
- `"a"` key to ➕ add a new top level task. Time to entewrr a description for the new task
- `"i"` key to 🔽➕ add a new subtask to the currently selected task. Entewrr a description for the new subtask
- `"d"` key to ➖ delete the currently selected task (with confirmation)
- `"r"` key to ✏️ weename the currently selected task

### New Features
- `1` key to set Low priority
- `2` key to set Medium priority  
- `3` key to set High priority
- `0` key to clear priority
- `J` key to move task down
- `K` key to move task up
- `?` key to show help screen

### Other
- `"q"` key to 🚪 quit the application

Nyaa~ 🐈 Don't forget to hit the `Enter` 🔑 key after typing your description to 🔒 lock the item into the todo list!

## Configuration

Tasks are saved to `~/.config/tewduwu/todo.txt`.

## Roadmap

See [plan.md](plan.md) for the full improvement plan and upcoming features.

## Building from Source

```bash
git clone https://github.com/4cecoder/tewduwu.git
cd tewduwu
./linux_build.sh  # or ./macos_build.sh
```