/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
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

/*! \file gschem_ai_assist.h
 *
 * \brief The "AI Assist" action.
 *
 * \note This lives outside actions.h because that header is generated from
 *       the action oracle by `python3 -m gedai.refactor gen`, which rewrites
 *       the file in full.  Declarations added there by hand would be lost on
 *       the next generation, so new actions that are not part of the
 *       refactored set are declared here instead.
 */

#ifndef GSCHEM_AI_ASSIST_H
#define GSCHEM_AI_ASSIST_H

#include "gschem_action.h"
#include "gschem_toplevel.h"

extern GschemAction *action_ai_assist;

void action_callback_ai_assist (GschemAction *action,
                                GschemToplevel *w_current);

gchar *ai_assist_schematic_text (GschemToplevel *w_current);
gchar *ai_assist_describe_schematic (GschemToplevel *w_current);
void ai_assist_refresh (GschemToplevel *w_current);

gboolean ai_assist_write_prompt (GschemToplevel *w_current,
                                 const gchar *prompt_file,
                                 gchar **output_file,
                                 GError **error);

#endif /* GSCHEM_AI_ASSIST_H */
