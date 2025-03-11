/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
 * Copyright (C) 2003-2010 Stuart D. Brorson.
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

/*!
 * \file
 * \brief Functions to operate on attributes in  STRING_LISTs
 *
 * Various functions to operate on attribute name=value pairs in supplied
 * STRING_LIST structs.
 */

#include <config.h>

#include <stdio.h>
#include <math.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

/*------------------------------------------------------------------
 * Gattrib specific includes
 *------------------------------------------------------------------*/
#include <libgeda/libgeda.h>       /* geda library fcns  */
#include "../include/struct.h"     /* typdef and struct declarations */
#include "../include/prototype.h"  /* function prototypes */
#include "../include/globals.h"
#include "../include/gettext.h"


/*------------------------------------------------------------------*/
/*! \brief Detect "name" in STRING_LIST
 *
 * This function is passed a STRING_LIST of name=value pairs, and a
 * name.
 * \param name_value_list pointer to STRING_LIST to search
 * \param name name string to search for
 * \returns 1 (TRUE) if the name is in the STRING_LIST, otherwise
 *          it returns 0 (FALSE).
 */
int s_attrib_name_in_list(STRING_LIST *name_value_list, char *name)
{
  STRING_LIST *local_list_item;
  char *local_name;

  for (local_list_item = name_value_list;
       local_list_item != NULL;
       local_list_item = local_list_item->next) {

    if (local_list_item->data == NULL)
      continue;

    local_name = u_basic_breakup_string(local_list_item->data, '=', 0);
    if (strcmp(local_name, name) == 0) {
      g_free (local_name);
      return TRUE;
    }
    g_free (local_name);
  }
  return FALSE;
}


/*------------------------------------------------------------------*/
/*! \brief Locate the refdes associated with an object.
 * 
 * This fcn takes an object, finds its refdes and returns it.
 * \param object Pointer to the object to search for.
 * \return For normal components, it returns a pointer to a
 *         string containing the refdes. If the component is slotted,
 *         it returns a refdes of the form
 *         refdes.slot. If no refdes is found, it returns NULL.
 */
char *s_attrib_get_refdes(OBJECT *object)
{
  char *temp_uref;
  char *numslots_value;
  char *slot_value;
  OBJECT *slot_text_object;

  /*------ Try to get the refdes -----*/
  temp_uref = o_attrib_search_object_attribs_by_name (object, "refdes", 0);
  if (!temp_uref) {
    temp_uref = o_attrib_search_object_attribs_by_name (object, "uref", 0); // deprecated
    if (temp_uref) {
      printf(_("WARNING: Found uref=%s, uref= is deprecated, please use refdes=\n"), temp_uref);
    } else {        /* didn't find refdes.  Report error to log. */
#ifdef DEBUG
      printf("In s_attrib_get_refdes, found non-graphical component with no refdes.\n");
      printf(". . . . complex_basename = %s.\n", object->complex_basename);
#endif
      return NULL;
    } 
  }

#ifdef DEBUG
  printf("In s_attrib_get_refdes, found component with refdes %s.\n", temp_uref);
#endif   
  
  /*------- Now append .slot to refdes if part is slotted -------- */
  /* Find out if this is a multislotted component */
  numslots_value =
    o_attrib_search_object_attribs_by_name (object, "numslots", 0);
  if (numslots_value != NULL) {  /* this is a slotted component; 
				    append slot number to refdes. */
    slot_value = s_slot_search_slot (object, &slot_text_object);
#if DEBUG
    printf(". . .  , found slotted component with slot = %s\n", slot_value);
#endif
    temp_uref = g_strconcat(temp_uref, ".", slot_value, NULL);
  }

#ifdef DEBUG
  printf(". . . .   returning refdes %s.\n", temp_uref);
#endif   
  
  return temp_uref;

}

