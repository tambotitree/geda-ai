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

/*! \file gattrib_json.c
 *  \brief Implementation of the minimal JSON reader.
 */

#include <math.h>
#include <string.h>

#include "gattrib_json.h"

struct _GattribJson {
  GattribJsonType type;

  gchar    *string;          /* STRING: decoded UTF-8; NUMBER: original text */
  gdouble   number;
  gboolean  boolean;

  GPtrArray  *items;         /* ARRAY elements, in order */
  GHashTable *members;       /* OBJECT: key -> GattribJson */
};

typedef struct {
  const gchar *start;
  const gchar *p;
  const gchar *end;
  gint         depth;
  GError     **error;
} Parser;

#define GATTRIB_JSON_MAX_DEPTH 256

static GattribJson *parse_value (Parser *ps);

static GattribJson *
node_new (GattribJsonType type)
{
  GattribJson *node = g_new0 (GattribJson, 1);
  node->type = type;
  return node;
}

static gboolean
parse_fail (Parser *ps, const gchar *msg)
{
  if (ps->error != NULL && *ps->error == NULL) {
    g_set_error (ps->error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
                 "%s (at byte %" G_GSIZE_FORMAT ")",
                 msg, (gsize) (ps->p - ps->start));
  }
  return FALSE;
}

static void
skip_ws (Parser *ps)
{
  while (ps->p < ps->end) {
    gchar c = *ps->p;
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
      ps->p++;
    else
      break;
  }
}

static gboolean
peek (Parser *ps, gchar c)
{
  return ps->p < ps->end && *ps->p == c;
}

static gboolean
match_literal (Parser *ps, const gchar *lit)
{
  gsize len = strlen (lit);
  if ((gsize) (ps->end - ps->p) < len)
    return FALSE;
  if (memcmp (ps->p, lit, len) != 0)
    return FALSE;
  ps->p += len;
  return TRUE;
}

/* String ---------------------------------------------------------------- */

static void
append_utf8 (GString *out, guint32 cp)
{
  gchar buf[6];
  gint len = g_unichar_to_utf8 ((gunichar) cp, buf);
  g_string_append_len (out, buf, len);
}

static gboolean
parse_hex4 (Parser *ps, guint32 *out)
{
  guint32 v = 0;
  gint i;

  if (ps->end - ps->p < 4)
    return parse_fail (ps, "truncated \\u escape");

  for (i = 0; i < 4; i++) {
    gchar c = *ps->p++;
    v <<= 4;
    if (c >= '0' && c <= '9')      v |= (guint32) (c - '0');
    else if (c >= 'a' && c <= 'f') v |= (guint32) (c - 'a' + 10);
    else if (c >= 'A' && c <= 'F') v |= (guint32) (c - 'A' + 10);
    else return parse_fail (ps, "bad digit in \\u escape");
  }

  *out = v;
  return TRUE;
}

/* Reads a quoted string, leaving ps->p just past the closing quote. */
static gchar *
parse_string_raw (Parser *ps)
{
  GString *out;

  if (!peek (ps, '"')) {
    parse_fail (ps, "expected string");
    return NULL;
  }
  ps->p++;

  out = g_string_new (NULL);

  while (ps->p < ps->end) {
    gchar c = *ps->p++;

    if (c == '"')
      return g_string_free (out, FALSE);

    if (c != '\\') {
      /* Control characters must be escaped in valid JSON, but tolerating
       * them is friendlier than rejecting an otherwise usable document. */
      g_string_append_c (out, c);
      continue;
    }

    if (ps->p >= ps->end)
      break;

    c = *ps->p++;
    switch (c) {
    case '"':  g_string_append_c (out, '"');  break;
    case '\\': g_string_append_c (out, '\\'); break;
    case '/':  g_string_append_c (out, '/');  break;
    case 'b':  g_string_append_c (out, '\b'); break;
    case 'f':  g_string_append_c (out, '\f'); break;
    case 'n':  g_string_append_c (out, '\n'); break;
    case 'r':  g_string_append_c (out, '\r'); break;
    case 't':  g_string_append_c (out, '\t'); break;
    case 'u': {
      guint32 cp;
      if (!parse_hex4 (ps, &cp)) {
        g_string_free (out, TRUE);
        return NULL;
      }
      /* Recombine a surrogate pair into one code point. */
      if (cp >= 0xD800 && cp <= 0xDBFF &&
          (gsize) (ps->end - ps->p) >= 6 &&
          ps->p[0] == '\\' && ps->p[1] == 'u') {
        Parser saved = *ps;
        guint32 lo;
        ps->p += 2;
        if (parse_hex4 (ps, &lo) && lo >= 0xDC00 && lo <= 0xDFFF) {
          cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
        } else {
          *ps = saved;   /* lone high surrogate; emit it as-is */
        }
      }
      append_utf8 (out, cp);
      break;
    }
    default:
      g_string_free (out, TRUE);
      parse_fail (ps, "unknown escape sequence");
      return NULL;
    }
  }

  g_string_free (out, TRUE);
  parse_fail (ps, "unterminated string");
  return NULL;
}

/* Number ---------------------------------------------------------------- */

static gboolean
is_number_char (gchar c)
{
  return g_ascii_isdigit (c) || c == '-' || c == '+' || c == '.' ||
         c == 'e' || c == 'E' || c == 'n' || c == 'N' ||
         c == 'i' || c == 'I' || c == 'f' || c == 'F' ||
         c == 'a' || c == 'A' || c == 't' || c == 'T' || c == 'y';
}

static GattribJson *
parse_number (Parser *ps)
{
  const gchar *begin = ps->p;
  gchar *text;
  gchar *endptr = NULL;
  GattribJson *node;

  /* NaN and Infinity are not legal JSON but appear in supplier data. */
  if (match_literal (ps, "NaN")) {
    node = node_new (GATTRIB_JSON_NUMBER);
    node->number = (gdouble) NAN;
    return node;
  }
  if (match_literal (ps, "-Infinity")) {
    node = node_new (GATTRIB_JSON_NUMBER);
    node->number = (gdouble) -INFINITY;
    return node;
  }
  if (match_literal (ps, "Infinity")) {
    node = node_new (GATTRIB_JSON_NUMBER);
    node->number = (gdouble) INFINITY;
    return node;
  }

  while (ps->p < ps->end && is_number_char (*ps->p))
    ps->p++;

  if (ps->p == begin) {
    parse_fail (ps, "expected number");
    return NULL;
  }

  text = g_strndup (begin, (gsize) (ps->p - begin));
  node = node_new (GATTRIB_JSON_NUMBER);
  node->number = g_ascii_strtod (text, &endptr);

  if (endptr != NULL && *endptr != '\0') {
    g_free (text);
    gattrib_json_free (node);
    parse_fail (ps, "malformed number");
    return NULL;
  }

  node->string = text;       /* keep the original text for exact echo */
  return node;
}

/* Containers ------------------------------------------------------------ */

static void
json_free_value_cb (gpointer data)
{
  gattrib_json_free ((GattribJson *) data);
}

static GattribJson *
parse_array (Parser *ps)
{
  GattribJson *node = node_new (GATTRIB_JSON_ARRAY);
  node->items = g_ptr_array_new_with_free_func (json_free_value_cb);

  ps->p++;                                    /* '[' */

  skip_ws (ps);
  if (peek (ps, ']')) {
    ps->p++;
    return node;
  }

  for (;;) {
    GattribJson *item = parse_value (ps);
    if (item == NULL) {
      gattrib_json_free (node);
      return NULL;
    }
    g_ptr_array_add (node->items, item);

    skip_ws (ps);
    if (peek (ps, ',')) {
      ps->p++;
      continue;
    }
    if (peek (ps, ']')) {
      ps->p++;
      return node;
    }

    gattrib_json_free (node);
    parse_fail (ps, "expected ',' or ']' in array");
    return NULL;
  }
}

static GattribJson *
parse_object (Parser *ps)
{
  GattribJson *node = node_new (GATTRIB_JSON_OBJECT);
  node->members = g_hash_table_new_full (g_str_hash, g_str_equal,
                                         g_free, json_free_value_cb);

  ps->p++;                                    /* '{' */

  skip_ws (ps);
  if (peek (ps, '}')) {
    ps->p++;
    return node;
  }

  for (;;) {
    gchar *key;
    GattribJson *value;

    skip_ws (ps);
    key = parse_string_raw (ps);
    if (key == NULL) {
      gattrib_json_free (node);
      return NULL;
    }

    skip_ws (ps);
    if (!peek (ps, ':')) {
      g_free (key);
      gattrib_json_free (node);
      parse_fail (ps, "expected ':' in object");
      return NULL;
    }
    ps->p++;

    value = parse_value (ps);
    if (value == NULL) {
      g_free (key);
      gattrib_json_free (node);
      return NULL;
    }

    /* Last duplicate key wins, matching common JSON implementations. */
    g_hash_table_replace (node->members, key, value);

    skip_ws (ps);
    if (peek (ps, ',')) {
      ps->p++;
      continue;
    }
    if (peek (ps, '}')) {
      ps->p++;
      return node;
    }

    gattrib_json_free (node);
    parse_fail (ps, "expected ',' or '}' in object");
    return NULL;
  }
}

static GattribJson *
parse_value (Parser *ps)
{
  GattribJson *node;

  skip_ws (ps);

  if (ps->p >= ps->end) {
    parse_fail (ps, "unexpected end of input");
    return NULL;
  }

  if (ps->depth >= GATTRIB_JSON_MAX_DEPTH) {
    parse_fail (ps, "nesting too deep");
    return NULL;
  }

  switch (*ps->p) {
  case '{':
    ps->depth++;
    node = parse_object (ps);
    ps->depth--;
    return node;

  case '[':
    ps->depth++;
    node = parse_array (ps);
    ps->depth--;
    return node;

  case '"': {
    gchar *s = parse_string_raw (ps);
    if (s == NULL)
      return NULL;
    node = node_new (GATTRIB_JSON_STRING);
    node->string = s;
    return node;
  }

  case 't':
    if (match_literal (ps, "true")) {
      node = node_new (GATTRIB_JSON_BOOL);
      node->boolean = TRUE;
      return node;
    }
    break;

  case 'f':
    if (match_literal (ps, "false")) {
      node = node_new (GATTRIB_JSON_BOOL);
      node->boolean = FALSE;
      return node;
    }
    break;

  case 'n':
    if (match_literal (ps, "null"))
      return node_new (GATTRIB_JSON_NULL);
    break;

  default:
    return parse_number (ps);
  }

  parse_fail (ps, "unexpected token");
  return NULL;
}

/* Public API ------------------------------------------------------------ */

GattribJson *
gattrib_json_parse (const gchar *text, gssize length, GError **error)
{
  Parser ps;
  GattribJson *root;

  g_return_val_if_fail (text != NULL, NULL);

  ps.start = text;
  ps.p     = text;
  ps.end   = text + (length < 0 ? (gssize) strlen (text) : length);
  ps.depth = 0;
  ps.error = error;

  root = parse_value (&ps);
  if (root == NULL)
    return NULL;

  skip_ws (&ps);
  if (ps.p != ps.end) {
    parse_fail (&ps, "trailing data after document");
    gattrib_json_free (root);
    return NULL;
  }

  return root;
}

void
gattrib_json_free (GattribJson *node)
{
  if (node == NULL)
    return;

  g_free (node->string);
  if (node->items != NULL)
    g_ptr_array_free (node->items, TRUE);
  if (node->members != NULL)
    g_hash_table_destroy (node->members);
  g_free (node);
}

GattribJsonType
gattrib_json_get_type (GattribJson *node)
{
  return node == NULL ? GATTRIB_JSON_NULL : node->type;
}

GattribJson *
gattrib_json_object_get (GattribJson *node, const gchar *key)
{
  if (node == NULL || node->type != GATTRIB_JSON_OBJECT || key == NULL)
    return NULL;
  return g_hash_table_lookup (node->members, key);
}

guint
gattrib_json_array_length (GattribJson *node)
{
  if (node == NULL || node->type != GATTRIB_JSON_ARRAY)
    return 0;
  return node->items->len;
}

GattribJson *
gattrib_json_array_get (GattribJson *node, guint index)
{
  if (node == NULL || node->type != GATTRIB_JSON_ARRAY)
    return NULL;
  if (index >= node->items->len)
    return NULL;
  return g_ptr_array_index (node->items, index);
}

GattribJson *
gattrib_json_array_get_by_member (GattribJson *array,
                                  const gchar *key,
                                  const gchar *value)
{
  guint i;

  if (array == NULL || array->type != GATTRIB_JSON_ARRAY ||
      key == NULL || value == NULL)
    return NULL;

  for (i = 0; i < array->items->len; i++) {
    GattribJson *item = g_ptr_array_index (array->items, i);
    const gchar *found = gattrib_json_object_get_string (item, key);
    if (found != NULL && strcmp (found, value) == 0)
      return item;
  }

  return NULL;
}

const gchar *
gattrib_json_get_string (GattribJson *node)
{
  if (node == NULL || node->type != GATTRIB_JSON_STRING)
    return NULL;
  return node->string;
}

gboolean
gattrib_json_get_double (GattribJson *node, gdouble *out)
{
  if (node == NULL || node->type != GATTRIB_JSON_NUMBER)
    return FALSE;
  if (!isfinite (node->number))
    return FALSE;
  if (out != NULL)
    *out = node->number;
  return TRUE;
}

const gchar *
gattrib_json_object_get_string (GattribJson *object, const gchar *key)
{
  return gattrib_json_get_string (gattrib_json_object_get (object, key));
}

GattribJson *
gattrib_json_lookup_path (GattribJson *node, const gchar *path)
{
  gchar **parts;
  GattribJson *cur = node;
  guint i;

  if (path == NULL)
    return NULL;

  parts = g_strsplit (path, ".", -1);
  for (i = 0; parts[i] != NULL && cur != NULL; i++) {
    if (parts[i][0] == '\0')
      continue;
    cur = gattrib_json_object_get (cur, parts[i]);
  }
  g_strfreev (parts);

  return cur;
}
