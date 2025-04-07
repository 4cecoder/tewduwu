# tewduwu Improvement Plan

## Overview
tewduwu is a terminal-based TODO list manager with hierarchical task support. This plan outlines improvements to make it more robust, feature-rich, and user-friendly.

## Code Improvements
- Add comprehensive error handling
- Fix potential buffer overflows and memory leaks
- Improve code organization and modularity
- Add detailed comments
- Implement proper input validation

## Feature Enhancements
### Core Functionality
- [x] Add priority levels (High, Medium, Low)
- [ ] Add due dates for tasks
- [ ] Add categories/tags for tasks
- [ ] Add multi-file support for different projects/contexts
- [ ] Implement undo/redo functionality

### Task Management
- [x] Add the ability to move tasks up and down
- [ ] Add bulk operations (mark multiple done/delete multiple)
- [ ] Add search functionality
- [ ] Add sorting options (by priority, due date, category, etc.)
- [ ] Add filtering options

### Data Management
- [x] Improve file format (now includes priority and due date fields)
- [ ] Add import/export functionality
- [ ] Add backup/restore features

## UI/UX Improvements
- [x] Add a status bar showing available commands
- [x] Add a help screen (accessed via '?')
- [x] Implement color coding for different priorities and states
- [ ] Add visual indicators for due dates (overdue, soon, etc.)
- [x] Improve visual hierarchy with better formatting
- [x] Add confirmation dialogs for destructive actions
- [x] Implement proper scrolling for long lists
- [x] Add more intuitive ways to manage nested tasks
- [x] Improve text editing experience

## Implementation Phases

### Phase 1: Core Improvements (Completed in this PR)
- Improved code quality and fixed bugs
- Added status bar and help screen
- Implemented proper scrolling
- Added color support
- Basic priority levels
- Task movement (up/down)

### Phase 2: Enhanced Task Management
- Due dates
- Enhanced categories/tags
- Improved nested task handling
- Sorting options

### Phase 3: Advanced Features
- Search and filtering
- Multi-file support
- Import/export
- Undo/redo

## Timeline
- Phase 1: Completed
- Phase 2: 2-3 weeks
- Phase 3: 3-4 weeks