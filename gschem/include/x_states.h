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

#ifndef X_STATES_H
#define X_STATES_H

#undef NONE

/* NOTE: when adding states, also update i_status_string() function */

enum x_states {
  /* Basic modes */
  SELECT,
  GRIPS,
  /* Draw modes */
  ARCMODE,
  BOXMODE,
  BUSMODE,
  CIRCLEMODE,
  LINEMODE,
  NETMODE,
  PATHMODE,
  PICTUREMODE,
  PINMODE,
  /* Place modes */
  COMPMODE,
  COPYMODE,
  MCOPYMODE,
  MOVEMODE,
  PASTEMODE,
  TEXTMODE,
  /* Box modes */
  SBOX,
  ZOOMBOX,
  /* Special modes */
  PAN,
  MIRRORMODE,
  ROTATEMODE,
  OGNRSTMODE,
};


#endif
