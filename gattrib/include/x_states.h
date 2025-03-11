/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
 * Copyright (C) 1998-2010 Ales Hvezda
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this library; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef X_STATES_H
#define X_STATES_H

#undef NONE

/* NOTE: when adding states, also update i_status_string() function */

enum x_states { NONE, SELECT, DRAWLINE, DRAWBOX, MOVE, COPY, DRAWCIRCLE, ZOOM,
  PAN, DRAWNET, NETCONT, TEXTENTRY, DRAWPIN, DRAWARC, STARTDRAWNET, DRAWCOMP,
  SBOX, STARTPAN, STARTSELECT, STARTCOPY, STARTMOVE, ENDCOPY, ENDMOVE, 
  ENDLINE, ENDBOX, ENDCIRCLE, ENDARC, ENDPIN, ENDCOMP, DRAWATTRIB, ENDATTRIB,
  DRAWTEXT, ENDTEXT, ENDROTATEP, ENDMIRROR, ZOOMBOXSTART, ZOOMBOXEND, 
  STARTROUTENET, ENDROUTENET, MOUSEPAN, DRAWBUS, BUSCONT, STARTDRAWBUS, 
  STARTPASTE, ENDPASTE, GRIPS};


#endif
