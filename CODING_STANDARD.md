This is not a casual hacker’s playground anymore.
It is a state-managed, rigorously documented, modular engineering artifact.

💡 “Readable by wizards, maintainable by mortals.”

⸻

# 1. Naming Conventions

o Functions: Use module_action_object() (verb first, module prefix).
o Variables: Descriptive and specific. drawing_area_widget, not da.
o Booleans: Start with is_, has_, should_.
o Constants: SCREAMING_SNAKE_CASE.
o Types: PascalCase: GschemToplevel.

⸻

# 2. Code Layout and Size Rules

o Functions:
	•	Max 150 lines.
	•	Break complex logic into state machines with explicit transition diagrams.

o Files:
	•	Max 500 lines per file.
	•	Split into logical modules (x_window_main.c, x_window_events.c).

o Comments:
	•	Docstrings on EVERY function, even static/internal ones.
	•	200 lines of comments per 10 lines of code is acceptable.

o Trees for nested structures:
Draw a text diagram for every hierarchy packed into a container:

```
Main Window
├── Main Box
│   ├── Toolbar
│   └── Paned Left
│       ├── Left Notebook
│       └── Paned Right
│           ├── Right Notebook
│           └── Vertical Paned
│               ├── Work Box
│               │   ├── File Notification
│               │   └── Drawing Area
│               └── Bottom Notebook
```


o Indentation: 2 spaces, no tabs.
o Line length: Soft limit 80, hard limit 100.

## 3. State Machines Everywhere

o Large functions must become explicit state machines.
o Use enum for state identifiers:

```
typedef enum {
  STATE_INIT,
  STATE_LOADING,
  STATE_RUNNING,
  STATE_CLEANUP
} AppState;
```

Each transition documented in a table:


---------------+---------------+-----------
State | Event  | New State
------+---------+--------------
STATE_INIT| LOAD_COMPLETE| STATE_RUNNING
STATE_RUNNING|USER_QUIT|STATE_CLEANUP
## 4. Logging & Debugging

o Replace printf() with GTK/GLib logging (g_debug, g_warning).
o Implement verbosity flags (-v, -vvv).
o Insert debug tree dumps to log container/widget hierarchies at startup.

⸻

## 5. Documentation Rules


o Every module starts with:
```
/*!
 * \file x_window_main.c
 * \brief Handles main window creation and layout.
 * \details
 * This module initializes GTK containers, manages dockable notebooks,
 * and embeds the drawing area within a GtkScrolledWindow.
 */
 Every function must include:
	•	Brief summary
	•	Parameter descriptions
	•	Return value details
	•	Side effects
```
o Maintain a developer README with tree diagrams for every major UI section.

⸻

##️ 6. Commit Discipline

o Each commit must:
	Do ONE thing (bugfix, refactor, feature).
	•	Be <500 lines of diff.
o Commit message format:

```
[gtk3] Refactor: x_window_create_main split into state machine
[cleanup] Removed GtkHandleBox, deleted legacy icons
```
## Golden Rules
	1.	No global variables unless absolutely unavoidable.
	2.	No ad-hoc debugging in main. Always use proper logging.
	3.	The next developer should understand your code by reading it once.

