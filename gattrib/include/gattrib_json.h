/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
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
 * \file gattrib_json.h
 * \brief A small, dependency-free JSON reader for supplier responses.
 *
 * gEDA links against GTK and GLib but not json-glib, and the parts-supplier
 * responses we consume are plain JSON.  Rather than add a build dependency
 * for that, this is a compact recursive-descent reader covering the whole
 * JSON grammar (RFC 8259) with the one deliberate extension of accepting
 * NaN/Infinity, which some suppliers emit for numeric fields.
 *
 * The design is read-only and deliberately boring: every node owns its
 * children, and freeing the root frees the whole tree.
 */

#ifndef GATTRIB_JSON_H
#define GATTRIB_JSON_H

#include <glib.h>
#include <gio/gio.h>

/*! Node kinds produced by the parser. */
typedef enum {
  GATTRIB_JSON_NULL,
  GATTRIB_JSON_BOOL,
  GATTRIB_JSON_NUMBER,
  GATTRIB_JSON_STRING,
  GATTRIB_JSON_ARRAY,
  GATTRIB_JSON_OBJECT
} GattribJsonType;

typedef struct _GattribJson GattribJson;

/*! \brief Parse a NUL-terminated UTF-8 document.
 *
 *  \param text      Document to parse.
 *  \param length    Number of bytes to read, or -1 to use strlen().
 *  \param error     Filled with a description and byte offset on failure.
 *  \return The root node, or NULL on failure.
 *
 *  \warning Free the result with gattrib_json_free().
 */
GattribJson *gattrib_json_parse (const gchar *text, gssize length,
                                 GError **error);

/*! \brief Free a node and everything below it.  Accepts NULL. */
void gattrib_json_free (GattribJson *node);

/*! \brief The node's kind, or GATTRIB_JSON_NULL for a NULL node. */
GattribJsonType gattrib_json_get_type (GattribJson *node);

/*! \brief Object member lookup.  Returns NULL when absent.
 *
 *  The result is owned by \a node. */
GattribJson *gattrib_json_object_get (GattribJson *node, const gchar *key);

/*! \brief Array element count, or 0 when \a node is not an array. */
guint gattrib_json_array_length (GattribJson *node);

/*! \brief Array element \a index, or NULL when out of range. */
GattribJson *gattrib_json_array_get (GattribJson *node, guint index);

/*! \brief Array member lookup by name.  Returns the first member of
 *  \a array whose object key \a key is a string equal to \a value.
 *
 *  This exists because GraphQL field aliases arrive as object members with
 *  caller-chosen names, so callers frequently need "the list whose key
 *  looks like this" rather than a fixed schema. */
GattribJson *gattrib_json_array_get_by_member (GattribJson *array,
                                               const gchar *key,
                                               const gchar *value);

/*! \brief String value, or NULL if \a node is not a string.
 *
 *  The result is owned by \a node. */
const gchar *gattrib_json_get_string (GattribJson *node);

/*! \brief Numeric value.  Returns FALSE when \a node is not a number, or when
 *  it holds a non-finite value. */
gboolean gattrib_json_get_double (GattribJson *node, gdouble *out);

/*! \brief Convenience: lookup \a key on \a object and return its string value.
 *
 *  Returns NULL when the member is missing or is not a string. */
const gchar *gattrib_json_object_get_string (GattribJson *object,
                                             const gchar *key);

/*! \brief Convenience: "a.b.c" style lookup through nested objects.
 *
 *  Returns NULL as soon as a level is missing or is not an object. */
GattribJson *gattrib_json_lookup_path (GattribJson *node, const gchar *path);

#endif /* GATTRIB_JSON_H */
