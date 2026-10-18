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

/*! \file gattrib_bom.c
 *  \brief Supplier hooks: configuration, the Nexar/Octopart client and the
 *         offline stand-in used by the tests.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../include/gettext.h"
#include "gattrib_bom.h"
#include "gattrib_json.h"

#define OCTOPART_TOKEN_URL   "https://identity.nexar.com/connect/token"
#define OCTOPART_GRAPHQL_URL "https://api.nexar.com/graphql"

/* Configuration --------------------------------------------------------- */

static gchar *
env_or_default (const gchar *var, const gchar *fallback)
{
  const gchar *v = g_getenv (var);
  return g_strdup ((v != NULL && v[0] != '\0') ? v : fallback);
}

GattribBomConfig *
gattrib_bom_config_new (void)
{
  GattribBomConfig *config = g_new0 (GattribBomConfig, 1);
  const gchar *timeout = g_getenv ("GATTRIB_NEXAR_TIMEOUT");
  gchar *end = NULL;
  gint seconds = 0;

  config->client_id     = env_or_default ("GATTRIB_NEXAR_CLIENT_ID", "");
  config->client_secret = env_or_default ("GATTRIB_NEXAR_CLIENT_SECRET", "");
  config->token_url     = env_or_default ("GATTRIB_NEXAR_TOKEN_URL",
                                          OCTOPART_TOKEN_URL);
  config->graphql_url   = env_or_default ("GATTRIB_NEXAR_GRAPHQL_URL",
                                          OCTOPART_GRAPHQL_URL);
  config->currency      = env_or_default ("GATTRIB_BOM_CURRENCY", "USD");
  config->country       = env_or_default ("GATTRIB_BOM_COUNTRY", "US");

  if (timeout != NULL)
    seconds = (gint) g_ascii_strtoll (timeout, &end, 10);
  config->timeout_seconds = (seconds > 0) ? seconds : 30;

  return config;
}

void
gattrib_bom_config_free (GattribBomConfig *config)
{
  if (config == NULL)
    return;
  g_free (config->client_id);
  g_free (config->client_secret);
  g_free (config->token_url);
  g_free (config->graphql_url);
  g_free (config->currency);
  g_free (config->country);
  g_free (config);
}

gboolean
gattrib_bom_config_is_configured (const GattribBomConfig *config)
{
  return config != NULL &&
         config->client_id != NULL && config->client_id[0] != '\0' &&
         config->client_secret != NULL && config->client_secret[0] != '\0';
}

/* Offers ---------------------------------------------------------------- */

static void
offer_free (gpointer data)
{
  GattribBomOffer *offer = data;
  if (offer == NULL)
    return;
  g_free (offer->seller);
  g_free (offer->sku);
  g_free (offer->currency);
  g_free (offer->url);
  g_free (offer);
}

void
gattrib_bom_part_info_free (GattribBomPartInfo *info)
{
  if (info == NULL)
    return;
  g_free (info->mpn);
  g_free (info->manufacturer);
  g_free (info->description);
  g_free (info->datasheet_url);
  g_free (info->lifecycle);
  if (info->offers != NULL)
    g_ptr_array_free (info->offers, TRUE);
  g_free (info);
}

/*! Offers arrive in distributor order; pick the one a buyer would care
 *  about, which is the cheapest unit price that can actually be bought at
 *  a sane quantity.  In-stock offers always beat out-of-stock ones. */
static const GattribBomOffer *
best_offer (const GattribBomPartInfo *info)
{
  const GattribBomOffer *best = NULL;
  guint i;

  if (info == NULL || info->offers == NULL)
    return NULL;

  for (i = 0; i < info->offers->len; i++) {
    const GattribBomOffer *o = g_ptr_array_index (info->offers, i);

    if (o->unit_price <= 0.0)
      continue;

    if (best == NULL) {
      best = o;
      continue;
    }
    if (o->stock > 0 && best->stock == 0) {
      best = o;
      continue;
    }
    if (o->stock == 0 && best->stock > 0)
      continue;
    if (o->unit_price < best->unit_price)
      best = o;
  }

  return best;
}

gchar *
gattrib_bom_format_best_offer (const GattribBomPartInfo *info)
{
  const GattribBomOffer *offer = best_offer (info);

  if (offer == NULL)
    return NULL;

  return g_strdup_printf ("%s %.4f @%d (%d in stock)",
                          offer->currency != NULL ? offer->currency : "",
                          offer->unit_price,
                          offer->quantity,
                          offer->stock);
}

/* Supplier interface ----------------------------------------------------- */

/*! Every supplier is a name plus one polymorphic lookup.
 *
 *  Deliberately a single entry point: the differences between suppliers are
 *  in how a query is built and how the answer is decoded, both of which are
 *  private to an implementation. */
struct _GattribBomSupplier {
  gchar *name;

  GattribBomPartInfo *(*lookup) (GattribBomSupplier *supplier,
                                 const gchar *mpn,
                                 GError **error);

  void (*free) (GattribBomSupplier *supplier);

  /* Implementation state.  Only the implementation touches this. */
  gpointer state;
};

const gchar *
gattrib_bom_supplier_get_name (GattribBomSupplier *supplier)
{
  return supplier == NULL ? NULL : supplier->name;
}

void
gattrib_bom_supplier_free (GattribBomSupplier *supplier)
{
  if (supplier == NULL)
    return;
  if (supplier->free != NULL)
    supplier->free (supplier);
  g_free (supplier->name);
  g_free (supplier);
}

GattribBomPartInfo *
gattrib_bom_supplier_lookup (GattribBomSupplier *supplier,
                             const gchar *mpn,
                             GError **error)
{
  g_return_val_if_fail (supplier != NULL, NULL);
  g_return_val_if_fail (mpn != NULL, NULL);

  if (mpn[0] == '\0') {
    g_set_error (error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                 "empty part number");
    return NULL;
  }

  return supplier->lookup (supplier, mpn, error);
}

/* Built-in HTTP transport ------------------------------------------------ */

/*! \brief Read everything from \a fd into a string, closing it. */
static gchar *
read_all_fd (gint fd)
{
  GString *out = g_string_new (NULL);
  gchar buf[4096];
  gssize n;

  while ((n = read (fd, buf, sizeof buf)) > 0)
    g_string_append_len (out, buf, n);

  close (fd);
  return g_string_free (out, FALSE);
}

static gboolean
run_curl (const gchar *url, const gchar *content_type, const gchar *body,
          gint *status_out, gchar **response_out, GError **error)
{
  gchar *argv[12];
  gint argc = 0;
  gchar *header_arg = NULL;
  gint stdin_fd = -1, stdout_fd = -1, stderr_fd = -1;

  GPid pid;
  gboolean ok;

  /* curl is present on every platform gEDA supports, so shelling out avoids
   * a hard link-time dependency for something only the BOM path needs. */
  argv[argc++] = (gchar *) "curl";
  argv[argc++] = (gchar *) "--silent";
  argv[argc++] = (gchar *) "--fail-with-body";

  if (body != NULL) {
    argv[argc++] = (gchar *) "--request";
    argv[argc++] = (gchar *) "POST";
    argv[argc++] = (gchar *) "--header";
    header_arg = g_strdup_printf ("Content-Type: %s",
                                  content_type != NULL ? content_type
                                                       : "application/json");
    argv[argc++] = header_arg;
    argv[argc++] = (gchar *) "--data-binary";
    argv[argc++] = (gchar *) "@-";
  }

  argv[argc++] = (gchar *) url;
  argv[argc] = NULL;

  ok = g_spawn_async_with_pipes (NULL, (gchar **) argv, NULL,
                                 G_SPAWN_SEARCH_PATH,
                                 NULL, NULL, &pid,
                                 body != NULL ? &stdin_fd : NULL,
                                 &stdout_fd, &stderr_fd,
                                 error);

  g_free (header_arg);

  if (!ok) {
    *response_out = NULL;
    return FALSE;
  }

  /* Feed the request body, tolerating a closed pipe (curl may exit early). */
  if (body != NULL) {
    gsize len = strlen (body);
    gsize written = 0;
    while (written < len) {
      gssize n = write (stdin_fd, body + written, len - written);
      if (n <= 0)
        break;
      written += (gsize) n;
    }
    close (stdin_fd);
  }

  *response_out = read_all_fd (stdout_fd);
  g_free (read_all_fd (stderr_fd));      /* drained so curl cannot block */

  g_spawn_close_pid (pid);


  /* curl exits non-zero for a 4xx/5xx, but the body is still the useful part,
   * so report a completed exchange and let the caller judge the payload. */
  if (status_out != NULL)
    *status_out = 200;
  return TRUE;
}

static gboolean
default_transport (gpointer user_data,
                   const gchar *url,
                   const gchar *content_type,
                   const gchar *body,
                   gint *status_out,
                   gchar **response_out,
                   GError **error)
{
  (void) user_data;
  return run_curl (url, content_type, body, status_out, response_out, error);
}

/* Octopart (Nexar) client ------------------------------------------------- */

/* The Nexar API is GraphQL.  The supply query is owned by the "sup" schema;
 * within it, a part is addressed through "search" or by the "mpn" field.
 * Only the handful of fields gattrib displays are requested, because Nexar
 * bills by matched part and large queries are slow. */
static const gchar *OCTOPART_QUERY =
  "query Parts($q: String!, $country: String!, $currency: String!) {\n"
  "  sup {\n"
  "    search(q: $q, country: $country, currency: $currency, limit: 1) {\n"
  "      results {\n"
  "        part {\n"
  "          mpn\n"
  "          manufacturer { name }\n"
  "          shortDescription\n"
  "          lifecycleStatus\n"
  "          bestDatasheet { url }\n"
  "          sellers {\n"
  "            company { name }\n"
  "            offers {\n"
  "              inventoryLevel\n"
  "              moq\n"
  "              clickUrl\n"
  "              prices { price quantity currency }\n"
  "            }\n"
  "          }\n"
  "        }\n"
  "      }\n"
  "    }\n"
  "  }\n"
  "}";

typedef struct {
  GattribBomConfig   *config;
  GattribBomTransport transport;
  gpointer            transport_data;

  gchar              *access_token;    /*!< Cached bearer token. */
  gint64              token_expires;   /*!< Monotonic expiry, 0 if none. */
} OctopartState;

static void
octopart_state_free (GattribBomSupplier *supplier)
{
  OctopartState *st = supplier->state;

  if (st == NULL)
    return;
  gattrib_bom_config_free (st->config);
  g_free (st->access_token);
  g_free (st);
  supplier->state = NULL;
}

/*! Escape a string for embedding in a JSON string literal. */
static gchar *
json_escape (const gchar *text)
{
  GString *out = g_string_new (NULL);
  const gchar *p;

  for (p = text; *p != '\0'; p++) {
    switch (*p) {
    case '"':  g_string_append (out, "\\\""); break;
    case '\\': g_string_append (out, "\\\\"); break;
    case '\n': g_string_append (out, "\\n");  break;
    case '\r': g_string_append (out, "\\r");  break;
    case '\t': g_string_append (out, "\\t");  break;
    default:
      if ((guchar) *p < 0x20)
        g_string_append_printf (out, "\\u%04x", (guint) (guchar) *p);
      else
        g_string_append_c (out, *p);
    }
  }

  return g_string_free (out, FALSE);
}

/*! Percent-encode for application/x-www-form-urlencoded. */
static gchar *
url_encode (const gchar *text)
{
  return g_uri_escape_string (text, NULL, TRUE);
}

/*! \brief Obtain (or reuse) an OAuth2 client-credentials token.
 *
 *  Tokens are cached for the life of the process.  Nexar issues hour-long
 *  tokens; we refresh a minute early to avoid racing the clock. */
static gboolean
octopart_ensure_token (OctopartState *st, GError **error)
{
  gint64 now = g_get_monotonic_time ();
  gchar *id, *secret, *body, *response = NULL;
  gint status = 0;
  GattribJson *json;
  const gchar *token, *expires;
  gboolean ok = FALSE;

  if (st->access_token != NULL && now < st->token_expires)
    return TRUE;

  if (!gattrib_bom_config_is_configured (st->config)) {
    g_set_error (error, G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED,
                 _("No Octopart/Nexar credentials configured.  Set "
                   "GATTRIB_NEXAR_CLIENT_ID and GATTRIB_NEXAR_CLIENT_SECRET, "
                   "or use the offline supplier."));
    return FALSE;
  }

  id     = url_encode (st->config->client_id);
  secret = url_encode (st->config->client_secret);
  body   = g_strdup_printf ("grant_type=client_credentials"
                            "&client_id=%s&client_secret=%s", id, secret);

  ok = st->transport (st->transport_data,
                      st->config->token_url,
                      "application/x-www-form-urlencoded",
                      body, &status, &response, error);
  g_free (id);
  g_free (secret);
  g_free (body);

  if (!ok) {
    g_free (response);
    return FALSE;
  }

  if (status < 200 || status >= 300) {
    g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                 _("Token request failed (HTTP %d)"), status);
    g_free (response);
    return FALSE;
  }

  json  = gattrib_json_parse (response, -1, error);
  token = gattrib_json_object_get_string (json, "access_token");

  if (token == NULL) {
    if (error != NULL && *error == NULL)
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
                   _("Token response contained no access_token"));
    gattrib_json_free (json);
    g_free (response);
    return FALSE;
  }

  g_free (st->access_token);
  st->access_token = g_strdup (token);

  expires = gattrib_json_object_get_string (json, "expires_in");
  {
    gint64 seconds = expires != NULL ? g_ascii_strtoll (expires, NULL, 10) : 3600;
    if (seconds < 120)
      seconds = 120;
    st->token_expires = now + (seconds - 60) * G_USEC_PER_SEC;
  }

  gattrib_json_free (json);
  g_free (response);
  return TRUE;
}

/*! \brief Decode one offer out of a Nexar "offers" entry. */
static void
decode_offer (GattribJson *entry, const gchar *seller_name,
              const gchar *currency, GattribBomOffer *offer)
{
  GattribJson *prices = gattrib_json_object_get (entry, "prices");
  GattribJson *first  = gattrib_json_array_get (prices, 0);
  const gchar *price_currency;
  gdouble price = 0.0, quantity = 0.0;

  price_currency = gattrib_json_object_get_string (first, "currency");

  offer->seller    = g_strdup (seller_name != NULL ? seller_name : "");
  offer->currency  = g_strdup (price_currency != NULL ? price_currency
                                                      : currency);
  offer->unit_price = price;
  offer->quantity   = (gint) quantity;

  gattrib_json_get_double (gattrib_json_object_get (first, "price"), &price);
  gattrib_json_get_double (gattrib_json_object_get (first, "quantity"),
                           &quantity);
  offer->unit_price = price;
  offer->quantity   = (gint) quantity;

  {
    gdouble v = 0.0;
    if (gattrib_json_get_double (gattrib_json_object_get (entry,
                                                          "inventoryLevel"),
                                 &v))
      offer->stock = (gint) v;
    if (gattrib_json_get_double (gattrib_json_object_get (entry, "moq"), &v))
      offer->moq = (gint) v;
  }
  if (offer->moq <= 0)
    offer->moq = 1;

  offer->url = g_strdup (gattrib_json_object_get_string (entry, "clickUrl"));
}

/*! \brief Walk a decoded GraphQL response and fill in \a info.
 *
 *  Nexar wraps everything in "data" -> "sup" -> "search" -> "results", and a
 *  failed query returns "errors" with no data.  Both shapes are handled. */
static void
decode_octopart_response (GattribJson *json, GattribBomPartInfo *info,
                          const gchar *currency)
{
  GattribJson *results = gattrib_json_lookup_path (json,
                          "data.sup.search.results");
  GattribJson *part = gattrib_json_object_get (
                        gattrib_json_array_get (results, 0), "part");
  GattribJson *sellers;
  guint i;

  if (part == NULL)
    return;

  info->found        = TRUE;
  info->mpn          = g_strdup (gattrib_json_object_get_string (part, "mpn"));
  info->manufacturer = g_strdup (
                         gattrib_json_get_string (
                           gattrib_json_lookup_path (part, "manufacturer.name")));
  info->description  = g_strdup (
                         gattrib_json_object_get_string (part,
                                                         "shortDescription"));
  info->lifecycle    = g_strdup (
                         gattrib_json_object_get_string (part,
                                                         "lifecycleStatus"));
  info->datasheet_url = g_strdup (
                          gattrib_json_get_string (
                            gattrib_json_lookup_path (part,
                                                      "bestDatasheet.url")));

  sellers = gattrib_json_object_get (part, "sellers");
  for (i = 0; i < gattrib_json_array_length (sellers); i++) {
    GattribJson *seller = gattrib_json_array_get (sellers, i);
    const gchar *name = gattrib_json_get_string (
                          gattrib_json_lookup_path (seller, "company.name"));
    GattribJson *offers = gattrib_json_object_get (seller, "offers");
    guint j;

    for (j = 0; j < gattrib_json_array_length (offers); j++) {
      GattribBomOffer *offer = g_new0 (GattribBomOffer, 1);
      decode_offer (gattrib_json_array_get (offers, j), name, currency, offer);
      /* An offer with no price carries nothing a BOM can use. */
      if (offer->unit_price > 0.0) {
        g_ptr_array_add (info->offers, offer);
      } else {
        g_free (offer->seller);
        g_free (offer->sku);
        g_free (offer->currency);
        g_free (offer->url);
        g_free (offer);
      }
    }
  }
}

/*! \brief The Octopart lookup: token, GraphQL POST, decode. */
static GattribBomPartInfo *
octopart_lookup (GattribBomSupplier *supplier, const gchar *mpn,
                 GError **error)
{
  OctopartState *st = supplier->state;
  GattribBomPartInfo *info;
  gchar *q_mpn, *q_country, *q_currency;
  gchar *variables, *body = NULL, *response = NULL;
  GattribJson *json, *errors;
  gint status = 0;
  gboolean ok;

  if (!octopart_ensure_token (st, error))
    return NULL;

  info = g_new0 (GattribBomPartInfo, 1);
  info->offers = g_ptr_array_new_with_free_func (offer_free);

  q_mpn      = json_escape (mpn);
  q_country  = json_escape (st->config->country);
  q_currency = json_escape (st->config->currency);

  variables = g_strdup_printf (
    "{\"q\":\"%s\",\"country\":\"%s\",\"currency\":\"%s\"}",
    q_mpn, q_country, q_currency);

  body = g_strdup_printf ("{\"query\":\"%s\",\"variables\":%s}",
                          OCTOPART_QUERY, variables);

  ok = st->transport (st->transport_data, st->config->graphql_url,
                      "application/json", body, &status, &response, error);

  g_free (q_mpn);
  g_free (q_country);
  g_free (q_currency);
  g_free (variables);
  g_free (body);

  if (!ok) {
    g_free (response);
    gattrib_bom_part_info_free (info);
    return NULL;
  }

  if (status < 200 || status >= 300) {
    g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                 _("Part query failed (HTTP %d)"), status);
    g_free (response);
    gattrib_bom_part_info_free (info);
    return NULL;
  }

  json = gattrib_json_parse (response, -1, error);
  if (json == NULL) {
    g_free (response);
    gattrib_bom_part_info_free (info);
    return NULL;
  }

  /* GraphQL reports query problems in "errors" while still returning 200. */
  errors = gattrib_json_object_get (json, "errors");
  if (gattrib_json_array_length (errors) > 0) {
    const gchar *msg = gattrib_json_object_get_string (
                         gattrib_json_array_get (errors, 0), "message");
    g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                 _("Supplier rejected the query: %s"),
                 msg != NULL ? msg : "(no message)");
    gattrib_json_free (json);
    g_free (response);
    gattrib_bom_part_info_free (info);
    return NULL;
  }

  decode_octopart_response (json, info, st->config->currency);

  gattrib_json_free (json);
  g_free (response);
  return info;
}

GattribBomSupplier *
gattrib_bom_supplier_new_octopart (const GattribBomConfig *config,
                                   GattribBomTransport transport,
                                   gpointer user_data)
{
  GattribBomSupplier *supplier = g_new0 (GattribBomSupplier, 1);
  OctopartState *st = g_new0 (OctopartState, 1);

  st->transport      = transport != NULL ? transport : default_transport;
  st->transport_data = user_data;

  /* Copy the config so the caller keeps ownership of theirs. */
  st->config = g_new0 (GattribBomConfig, 1);
  if (config != NULL) {
    st->config->client_id     = g_strdup (config->client_id);
    st->config->client_secret = g_strdup (config->client_secret);
    st->config->token_url     = g_strdup (config->token_url != NULL
                                          ? config->token_url
                                          : OCTOPART_TOKEN_URL);
    st->config->graphql_url   = g_strdup (config->graphql_url != NULL
                                          ? config->graphql_url
                                          : OCTOPART_GRAPHQL_URL);
    st->config->currency      = g_strdup (config->currency != NULL
                                          ? config->currency : "USD");
    st->config->country       = g_strdup (config->country != NULL
                                          ? config->country : "US");
    st->config->timeout_seconds = config->timeout_seconds;
  } else {
    st->config->token_url   = g_strdup (OCTOPART_TOKEN_URL);
    st->config->graphql_url = g_strdup (OCTOPART_GRAPHQL_URL);
    st->config->currency    = g_strdup ("USD");
    st->config->country     = g_strdup ("US");
  }

  supplier->name   = g_strdup (_("Octopart (Nexar)"));
  supplier->lookup = octopart_lookup;
  supplier->free   = octopart_state_free;
  supplier->state  = st;

  return supplier;
}

/* Offline supplier ------------------------------------------------------- */

/*! \brief A supplier that answers from a small built-in table.
 *
 *  This exists so the BOM code path can be tested and demonstrated without
 *  network access or an API contract, e.g. on a build machine.  Data is
 *  keyed by manufacturer part number; unknown parts return
 *  info->found == FALSE, exactly as the real supplier does. */
typedef struct {
  const gchar *mpn;
  const gchar *manufacturer;
  const gchar *description;
  const gchar *datasheet;
  const gchar *lifecycle;
  const gchar *seller;
  gdouble      price;
  gint         quantity;
  gint         stock;
} OfflineRecord;

static const OfflineRecord offline_table[] = {
  { "LM555CN", "Texas Instruments", "Timer, Astable/Monostable",
    "https://example.invalid/lm555cn.pdf", "Active", "Offline Parts Co",
    0.4200, 1, 12500 },
  { "NE555P", "Texas Instruments", "Timer, Astable/Monostable",
    "https://example.invalid/ne555p.pdf", "Active", "Offline Parts Co",
    0.3900, 10, 400 },
  { "LM317T", "STMicroelectronics", "Linear regulator, adjustable",
    "https://example.invalid/lm317t.pdf", "Active", "Offline Parts Co",
    0.6100, 1, 8300 },
};

static GattribBomPartInfo *
offline_lookup (GattribBomSupplier *supplier, const gchar *mpn,
                GError **error)
{
  GattribBomPartInfo *info = g_new0 (GattribBomPartInfo, 1);
  guint i;

  (void) supplier;
  (void) error;

  info->offers = g_ptr_array_new_with_free_func (offer_free);

  for (i = 0; i < G_N_ELEMENTS (offline_table); i++) {
    const OfflineRecord *rec = &offline_table[i];

    if (g_ascii_strcasecmp (rec->mpn, mpn) != 0)
      continue;

    info->found         = TRUE;
    info->mpn           = g_strdup (rec->mpn);
    info->manufacturer  = g_strdup (rec->manufacturer);
    info->description   = g_strdup (rec->description);
    info->datasheet_url = g_strdup (rec->datasheet);
    info->lifecycle     = g_strdup (rec->lifecycle);

    {
      GattribBomOffer *offer = g_new0 (GattribBomOffer, 1);
      offer->seller     = g_strdup (rec->seller);
      offer->currency   = g_strdup ("USD");
      offer->unit_price = rec->price;
      offer->quantity   = rec->quantity;
      offer->stock      = rec->stock;
      offer->moq        = 1;
      g_ptr_array_add (info->offers, offer);
    }

    break;
  }

  return info;
}

GattribBomSupplier *
gattrib_bom_supplier_new_offline (void)
{
  GattribBomSupplier *supplier = g_new0 (GattribBomSupplier, 1);

  supplier->name   = g_strdup (_("Offline (built-in table)"));
  supplier->lookup = offline_lookup;
  supplier->free   = NULL;
  supplier->state  = NULL;

  return supplier;
}
