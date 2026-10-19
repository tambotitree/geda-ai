/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2020 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef GSCHEM_ACTION_H
#define GSCHEM_ACTION_H

#include <gtk/gtk.h>          // For GtkWidget, GtkToolItem
#include <libguile.h>         // For SCM
#include "gschem.h"           // For GschemToplevel

#define GSCHEM_ACTION(obj) ((GschemAction *) (obj))

typedef enum {
  GSCHEM_ACTION_TYPE_ACTUATE,
  GSCHEM_ACTION_TYPE_TOGGLE_PLAIN,
  GSCHEM_ACTION_TYPE_TOGGLE_CHECK,
  GSCHEM_ACTION_TYPE_TOGGLE_RADIO
} GschemActionType;

/* Legacy macro aliases for action types */
#define ACTUATE GSCHEM_ACTION_TYPE_ACTUATE
#define TOGGLE_PLAIN GSCHEM_ACTION_TYPE_TOGGLE_PLAIN
#define TOGGLE_CHECK GSCHEM_ACTION_TYPE_TOGGLE_CHECK
#define TOGGLE_RADIO GSCHEM_ACTION_TYPE_TOGGLE_RADIO

typedef struct _GschemAction GschemAction;

/*! \brief Signature of an action activation callback.
 *
 * \param [in] action    The action being activated.
 * \param [in] w_current The toplevel whose context the action runs in.
 */
typedef void (*GschemActionCallback) (GschemAction *action,
                                      GschemToplevel *w_current);

/*! \brief Declarative description of one action.
 *
 * One ::GschemActionSpec exists per action, held in the ::action_specs table
 * in actions.c.  Keeping the description as data means the metadata can be
 * reviewed, diffed and generated without preprocessor tricks, and the
 * callback stays an ordinary function pointer.
 *
 * The string fields are \c N_()-marked literals: they are translated at
 * registration time, since a static initialiser cannot call \c gettext().
 *
 * \todo Tier C actions, owned by an external executable, are not implemented.
 *       When they are, they must reach activation through this structure so
 *       that one registration path serves all three tiers; the planned
 *       contract is \c TOOL \c --describe for the manifest and
 *       \c TOOL \c [args] with an s-expression on stdin and stdout.  See
 *       gschem/src/TRANSFORM_PROTOCOL.md.  Deliberately not stubbed here:
 *       an unused \c x_run_transform() would rot before it is needed.
 */
typedef struct {
  const gchar         *id;        /*!< Stable id, e.g. "file-open"        */
  const gchar         *icon_name; /*!< Icon name, or NULL                 */
  const gchar         *name;      /*!< Long name, e.g. "Open File"        */
  const gchar         *label;     /*!< Label, e.g. "Open..."              */
  const gchar         *menu_label;/*!< Mnemonic label, e.g. "_Open..."    */
  const gchar         *tooltip;   /*!< Tooltip, or NULL                   */
  GschemActionType     type;      /*!< Actuate or one of the toggle types */
  GschemActionCallback activate;  /*!< Function invoked on activation    */
  GschemAction       **action;    /*!< Set to the registered object      */
} GschemActionSpec;

struct _GschemAction {
  gchar *id;
  gchar *icon_name;
  gchar *name;
  gchar *label;
  gchar *menu_label;
  gchar *tooltip;
  GschemActionType type;

  void (*activate) (GschemAction *action,
                    GschemToplevel *w_current);

  SCM thunk;
  SCM smob;  /* not owned */
};


int scm_is_action (SCM x);
GschemAction *scm_to_action (SCM smob);

void gschem_action_activate (GschemAction *action,
                             GschemToplevel *w_current);

GtkWidget *gschem_action_create_menu_item (GschemAction *action,
                                           gboolean use_menu_label,
                                           GschemToplevel *w_current);
GtkToolItem *gschem_action_create_tool_button (GschemAction *action,
                                               GschemToplevel *w_current);

void gschem_action_set_sensitive (GschemAction *action, gboolean sensitive,
                                  GschemToplevel *w_current);
void gschem_action_set_active (GschemAction *action, gboolean is_active,
                               GschemToplevel *w_current);
void gschem_action_set_strings (GschemAction *action,
                                gchar *name, gchar *label, gchar *menu_label,
                                GschemToplevel *w_current);

GschemAction *gschem_action_register (const gchar *id,
    const gchar *icon_name,
    const gchar *name,
    const gchar *label,
    const gchar *menu_label,
    const gchar *tooltip,
    GschemActionType type,
    void (*activate) (GschemAction *,
        GschemToplevel *));
        
void gschem_action_init (void);

#endif /* GSCHEM_ACTION_H */
