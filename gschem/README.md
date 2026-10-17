# Introduction to gschem
## Welcome to gschem (gEDA-AI Edition)

This is the modernized GTK3 port of `gschem`, part of the gEDA-AI suite, a fully open-source electronic design automation (EDA) toolkit. 

This fork retains the spirit and power of the original `gschem`, but modernizes its internals and graphical toolkit for compatibility, maintainability, and AI-assisted schematic editing.

- 🐝 Maintainer: John Ryan
- 🛠 GTK 3 Migration: In progress
- 🤖 AI-Assisted Design: Planned

If you're a developer or contributor, the notes below include architectural diagrams and TODOs that help navigate the codebase.

# Introduction to gschem

## Main Window Architecture

The following diagram summarizes the layout created in `x_window_create_main()` during application initialization:

```
┌──────────────────────────────────────────────┐
│ [ Menubar ]                                  │
├──────────────────────────────────────────────┤
│ [ Toolbar ]                                  │
├──────────────────────────────────────────────┤
│ ┌────────────────────┬────────────────────┐ │
│ │ [left_notebook]    │ [right_hpaned]     │ │
│ │ (Library browser,  │ (Main drawing area │ │
│ │  object lists, etc)│  and side panels)  │ │
│ └────────────────────┴────────────────────┘ │
├──────────────────────────────────────────────┤
│ [ Status bar ]                               │
└──────────────────────────────────────────────┘
```

This view is essential for reasoning about layout bugs or modernizing individual widget components.