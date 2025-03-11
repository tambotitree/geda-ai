/* gEDA - GPL Electronic Design Automation
 * libgeda - gEDA's library
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

/*! \file o_embed.c
 *  \brief functions to embed and unembed symbols
 */

#include <config.h>

#include <stdio.h>

#include "libgeda_priv.h"


/*! \brief embed an object into a schematic
 *  \par Function Description
 *  This functions embeds an object \a o_current into a
 *  libgeda. Currently complex objects are just marked to
 *  be embedded later. Picture objects are embedded immediatly.
 *
 *  \param toplevel  The TOPLEVEL object
 *  \param o_current The OBJECT to embed
 *
 *  \returns whether the function was successful
 */
gboolean o_embed(TOPLEVEL *toplevel, OBJECT *o_current)
{
  gboolean page_modified = FALSE;

  /* check o_current is a complex and is not already embedded */
  if (o_current->type == OBJ_COMPLEX &&
      !o_complex_is_embedded (o_current))
  {

    /* set the embedded flag */
    o_current->complex_embedded = TRUE;

    s_log_message (_("Component [%s] has been embedded\n"),
                   o_current->complex_basename);
    page_modified = TRUE;
  }

  /* If it's a picture and it's not embedded */
  if ( (o_current->type == OBJ_PICTURE) &&
       !o_picture_is_embedded (o_current) ) {
    o_picture_embed (toplevel, o_current);

    page_modified = TRUE;
  }

  return page_modified;
}

/*! \brief unembed an object from a schematic
 *  \par Function Description
 *  This functions unembeds an object \a o_current from a
 *  libgeda structure. Complex objects are just marked to
 *  be not embedded. Picture objects are unembeded immediatly.
 *
 *  \param toplevel  The TOPLEVEL object
 *  \param o_current The OBJECT to unembed
 *
 *  \returns whether the function was successful
 */
gboolean o_unembed(TOPLEVEL *toplevel, OBJECT *o_current)
{
  const CLibSymbol *sym;
  gboolean page_modified = FALSE;

  /* check o_current is an embedded complex */
  if (o_current->type == OBJ_COMPLEX &&
      o_complex_is_embedded (o_current))
  {

    /* search for the symbol in the library */
    sym = s_clib_get_symbol_by_name (o_current->complex_basename);

    if (sym == NULL) {
      /* symbol not found in the symbol library: signal an error */
      s_log_message (_("Could not find component [%s], while trying to "
                       "unembed. Component is still embedded\n"),
                     o_current->complex_basename);

    } else {
      /* clear the embedded flag */
      o_current->complex_embedded = FALSE;

      s_log_message (_("Component [%s] has been successfully unembedded\n"),
                     o_current->complex_basename);

      page_modified = TRUE;
    }
  }

  /* If it's a picture and it's embedded */
  if ( (o_current->type == OBJ_PICTURE) &&
       o_picture_is_embedded (o_current)) {
    o_picture_unembed (toplevel, o_current);

    page_modified = TRUE;
  }

  return page_modified;
}
