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

/*! \file test_gattrib_bom.c
 *  \brief Tests for the JSON reader and the supplier hooks.
 *
 *  The supplier tests inject a fake transport, so they exercise the real
 *  OAuth2 and GraphQL code paths -- URL selection, request bodies, response
 *  decoding -- without a network or credentials.
 */

#include <string.h>

#include "gattrib_bom.h"
#include "gattrib_json.h"

static gint checks = 0;
static gint failures = 0;

#define CHECK(cond)                                                     \
  do {                                                                  \
    checks++;                                                           \
    if (!(cond)) {                                                      \
      failures++;                                                       \
      g_printerr ("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);       \
    }                                                                   \
  } while (0)

#define CHECK_STR(actual, expected)                                     \
  do {                                                                  \
    checks++;                                                           \
    const gchar *_a = (actual);                                         \
    if (!g_str_equal (_a != NULL ? _a : "", (expected))) {              \
      failures++;                                                       \
      g_printerr ("FAIL %s:%d: '%s' != '%s'\n", __FILE__, __LINE__,     \
                  _a != NULL ? _a : "(null)", (expected));              \
    }                                                                   \
  } while (0)

/* JSON ------------------------------------------------------------------ */

static void
test_json_scalars (void)
{
  GError *error = NULL;
  GattribJson *root;

  root = gattrib_json_parse ("{\"a\":1,\"b\":\"two\",\"c\":true,"
                             "\"d\":null,\"e\":[1,2,3]}", -1, &error);
  CHECK (root != NULL);
  CHECK (error == NULL);
  CHECK (gattrib_json_get_type (root) == GATTRIB_JSON_OBJECT);

  {
    gdouble v = 0.0;
    CHECK (gattrib_json_get_double (gattrib_json_object_get (root, "a"), &v));
    CHECK (v == 1.0);
  }

  CHECK_STR (gattrib_json_object_get_string (root, "b"), "two");
  CHECK (gattrib_json_get_type (gattrib_json_object_get (root, "c")) ==
         GATTRIB_JSON_BOOL);
  CHECK (gattrib_json_get_type (gattrib_json_object_get (root, "d")) ==
         GATTRIB_JSON_NULL);
  CHECK (gattrib_json_array_length (gattrib_json_object_get (root, "e")) == 3);

  /* Lookups that should miss, not crash. */
  CHECK (gattrib_json_object_get (root, "zzz") == NULL);
  CHECK (gattrib_json_object_get_string (root, "a") == NULL);
  CHECK (gattrib_json_array_get (gattrib_json_object_get (root, "e"), 9) == NULL);
  CHECK (gattrib_json_array_length (root) == 0);

  gattrib_json_free (root);
}

static void
test_json_escapes (void)
{
  GError *error = NULL;
  GattribJson *root = gattrib_json_parse (
    "{\"s\":\"a\\\"b\\\\c\\nd\\u00e9\\ud83d\\ude00\"}", -1, &error);

  CHECK (root != NULL);
  CHECK_STR (gattrib_json_object_get_string (root, "s"),
             "a\"b\\c\nd\u00e9\U0001F600");

  gattrib_json_free (root);
}

static void
test_json_errors (void)
{
  GError *error = NULL;
  GattribJson *root;

  root = gattrib_json_parse ("{", -1, &error);
  CHECK (root == NULL);
  CHECK (error != NULL);
  g_clear_error (&error);

  root = gattrib_json_parse ("{\"a\":1} junk", -1, &error);
  CHECK (root == NULL);
  CHECK (error != NULL);
  g_clear_error (&error);

  root = gattrib_json_parse ("[1,]", -1, &error);
  CHECK (root == NULL);
  CHECK (error != NULL);
  g_clear_error (&error);
}

static void
test_json_paths (void)
{
  GError *error = NULL;
  GattribJson *root = gattrib_json_parse (
    "{\"data\":{\"sup\":{\"search\":{\"results\":[{\"part\":{\"mpn\":\"X1\"}}]}}}}",
    -1, &error);

  CHECK (root != NULL);
  CHECK_STR (gattrib_json_get_string (
               gattrib_json_lookup_path (root, "data.sup.search.results.0.part.mpn")),
             "");
  CHECK (gattrib_json_lookup_path (root, "data.nope.deeper") == NULL);
  CHECK (gattrib_json_lookup_path (root, NULL) == NULL);

  gattrib_json_free (root);
  gattrib_json_free (NULL);          /* must be a no-op */
}

/* Offline supplier ------------------------------------------------------- */

static void
test_offline_supplier (void)
{
  GattribBomSupplier *supplier = gattrib_bom_supplier_new_offline ();
  GattribBomPartInfo *info;
  gchar *summary;
  GError *error = NULL;

  CHECK (supplier != NULL);
  CHECK (gattrib_bom_supplier_get_name (supplier) != NULL);

  info = gattrib_bom_supplier_lookup (supplier, "LM555CN", &error);
  CHECK (info != NULL);
  CHECK (error == NULL);
  CHECK (info->found);
  CHECK_STR (info->mpn, "LM555CN");
  CHECK_STR (info->manufacturer, "Texas Instruments");
  CHECK (info->datasheet_url != NULL);
  CHECK (info->offers->len == 1);

  summary = gattrib_bom_format_best_offer (info);
  CHECK_STR (summary, "USD 0.4200 @1 (12500 in stock)");
  g_free (summary);
  gattrib_bom_part_info_free (info);

  /* Case-insensitive match. */
  info = gattrib_bom_supplier_lookup (supplier, "lm317t", &error);
  CHECK (info != NULL && info->found);
  gattrib_bom_part_info_free (info);

  /* Unknown part: no error, found == FALSE. */
  info = gattrib_bom_supplier_lookup (supplier, "NOSUCHPART", &error);
  CHECK (info != NULL);
  CHECK (!info->found);
  CHECK (gattrib_bom_format_best_offer (info) == NULL);
  gattrib_bom_part_info_free (info);

  /* Empty part number is a programming error, reported as such. */
  info = gattrib_bom_supplier_lookup (supplier, "", &error);
  CHECK (info == NULL);
  CHECK (error != NULL);
  g_clear_error (&error);

  gattrib_bom_supplier_free (supplier);
}

/* Config ----------------------------------------------------------------- */

static void
test_config_defaults (void)
{
  GattribBomConfig *config = gattrib_bom_config_new ();

  CHECK (config != NULL);
  CHECK_STR (config->token_url,
             "https://identity.nexar.com/connect/token");
  CHECK_STR (config->graphql_url, "https://api.nexar.com/graphql");
  CHECK_STR (config->currency, "USD");
  CHECK (config->timeout_seconds > 0);

  gattrib_bom_config_free (config);
  gattrib_bom_config_free (NULL);    /* must be a no-op */
  CHECK (!gattrib_bom_config_is_configured (NULL));
}

/* Octopart supplier with a fake transport -------------------------------- */

typedef struct {
  gint     calls;
  gchar   *last_url;
  gchar   *last_body;
  gchar   *token_response;
  gchar   *graphql_response;
  gint     token_status;
  gboolean fail;
} FakeTransport;

static gboolean
fake_transport (gpointer user_data, const gchar *url,
                const gchar *content_type, const gchar *body,
                gint *status_out, gchar **response_out, GError **error)
{
  FakeTransport *f = user_data;

  (void) content_type;
  f->calls++;
  g_free (f->last_url);
  g_free (f->last_body);
  f->last_url = g_strdup (url);
  f->last_body = g_strdup (body);

  if (f->fail) {
    g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED, "offline");
    return FALSE;
  }

  if (strstr (url, "token") != NULL) {
    *status_out = f->token_status;
    *response_out = g_strdup (f->token_response);
  } else {
    *status_out = 200;
    *response_out = g_strdup (f->graphql_response);
  }
  return TRUE;
}

static void
fake_clear (FakeTransport *f)
{
  g_free (f->last_url);
  g_free (f->last_body);
  g_free (f->token_response);
  g_free (f->graphql_response);
}

static GattribBomConfig *
config_with_credentials (void)
{
  GattribBomConfig *config = gattrib_bom_config_new ();
  g_free (config->client_id);
  g_free (config->client_secret);
  config->client_id = g_strdup ("id");
  config->client_secret = g_strdup ("secret");
  return config;
}

static const gchar *SAMPLE_GRAPHQL =
  "{\"data\":{\"sup\":{\"search\":{\"results\":[{\"part\":{"
  "\"mpn\":\"LM555CN\","
  "\"manufacturer\":{\"name\":\"Texas Instruments\"},"
  "\"shortDescription\":\"Timer\","
  "\"lifecycleStatus\":\"Active\","
  "\"bestDatasheet\":{\"url\":\"https://example.invalid/ds.pdf\"},"
  "\"sellers\":["
  " {\"company\":{\"name\":\"Digi-Key\"},"
  "  \"offers\":[{\"inventoryLevel\":1000,\"moq\":1,"
  "    \"clickUrl\":\"https://example.invalid/dk\","
  "    \"prices\":[{\"price\":1.24,\"quantity\":100,\"currency\":\"USD\"}]}]},"
  " {\"company\":{\"name\":\"Mouser\"},"
  "  \"offers\":[{\"inventoryLevel\":0,\"moq\":1,"
  "    \"prices\":[{\"price\":0.90,\"quantity\":1,\"currency\":\"USD\"}]}]}"
  "]}}]}}}}";

static void
test_octopart_supplier (void)
{
  GattribBomConfig *config;
  GattribBomSupplier *supplier;
  GattribBomPartInfo *info;
  FakeTransport fake = { 0 };
  GError *error = NULL;
  gchar *summary;

  /* No credentials: a clear, actionable failure rather than a crash. */
  config = gattrib_bom_config_new ();
  supplier = gattrib_bom_supplier_new_octopart (config, fake_transport, &fake);
  CHECK (supplier != NULL);
  CHECK_STR (gattrib_bom_supplier_get_name (supplier), "Octopart (Nexar)");

  info = gattrib_bom_supplier_lookup (supplier, "LM555CN", &error);
  CHECK (info == NULL);
  CHECK (error != NULL);
  CHECK (fake.calls == 0);           /* nowhere to send the request */
  g_clear_error (&error);
  gattrib_bom_supplier_free (supplier);
  gattrib_bom_config_free (config);

  /* With credentials, drive the full token + GraphQL sequence. */
  config = config_with_credentials ();
  CHECK (gattrib_bom_config_is_configured (config));

  fake.token_response = g_strdup (
    "{\"access_token\":\"tok\",\"expires_in\":3600}");
  fake.graphql_response = g_strdup (SAMPLE_GRAPHQL);
  fake.token_status = 200;

  supplier = gattrib_bom_supplier_new_octopart (config, fake_transport, &fake);

  info = gattrib_bom_supplier_lookup (supplier, "LM555CN", &error);
  CHECK (info != NULL);
  CHECK (error == NULL);
  CHECK (fake.calls == 2);           /* token, then query */
  CHECK (strstr (fake.last_url, "graphql") != NULL);

  /* The part number must reach the request body. */
  CHECK (strstr (fake.last_body, "LM555CN") != NULL);
  CHECK (strstr (fake.last_body, "\"query\"") != NULL);

  CHECK (info->found);
  CHECK_STR (info->mpn, "LM555CN");
  CHECK_STR (info->manufacturer, "Texas Instruments");
  CHECK_STR (info->lifecycle, "Active");
  CHECK_STR (info->datasheet_url, "https://example.invalid/ds.pdf");
  CHECK (info->offers->len == 2);

  /* Mouser is cheaper but has no stock, so Digi-Key must win. */
  summary = gattrib_bom_format_best_offer (info);
  CHECK_STR (summary, "USD 1.2400 @100 (1000 in stock)");
  g_free (summary);

  gattrib_bom_part_info_free (info);

  /* A second lookup reuses the cached token. */
  info = gattrib_bom_supplier_lookup (supplier, "LM555CN", &error);
  CHECK (info != NULL);
  CHECK (fake.calls == 3);           /* one more query, no new token */
  gattrib_bom_part_info_free (info);

  gattrib_bom_supplier_free (supplier);
  gattrib_bom_config_free (config);
  fake_clear (&fake);
}

static void
test_octopart_graphql_error (void)
{
  GattribBomConfig *config = config_with_credentials ();
  GattribBomSupplier *supplier;
  GattribBomPartInfo *info;
  FakeTransport fake = { 0 };
  GError *error = NULL;

  fake.token_response = g_strdup (
    "{\"access_token\":\"tok\",\"expires_in\":3600}");
  /* GraphQL reports failure with HTTP 200. */
  fake.graphql_response = g_strdup (
    "{\"errors\":[{\"message\":\"Rate limit exceeded\"}]}");
  fake.token_status = 200;

  supplier = gattrib_bom_supplier_new_octopart (config, fake_transport, &fake);
  info = gattrib_bom_supplier_lookup (supplier, "LM555CN", &error);

  CHECK (info == NULL);
  CHECK (error != NULL);
  CHECK (strstr (error->message, "Rate limit exceeded") != NULL);
  g_clear_error (&error);

  /* An empty result set is not an error: found == FALSE. */
  g_free (fake.graphql_response);
  fake.graphql_response = g_strdup (
    "{\"data\":{\"sup\":{\"search\":{\"results\":[]}}}}");
  info = gattrib_bom_supplier_lookup (supplier, "LM555CN", &error);
  CHECK (info != NULL);
  CHECK (error == NULL);
  CHECK (!info->found);
  gattrib_bom_part_info_free (info);

  gattrib_bom_supplier_free (supplier);
  gattrib_bom_config_free (config);
  fake_clear (&fake);
}

static void
test_octopart_transport_failure (void)
{
  GattribBomConfig *config = config_with_credentials ();
  GattribBomSupplier *supplier;
  FakeTransport fake = { 0 };
  GError *error = NULL;

  fake.fail = TRUE;
  supplier = gattrib_bom_supplier_new_octopart (config, fake_transport, &fake);

  CHECK (gattrib_bom_supplier_lookup (supplier, "X", &error) == NULL);
  CHECK (error != NULL);
  g_clear_error (&error);

  gattrib_bom_supplier_free (supplier);
  gattrib_bom_config_free (config);
  fake_clear (&fake);
}

int
main (int argc, char **argv)
{
  (void) argc;
  (void) argv;

  test_json_scalars ();
  test_json_escapes ();
  test_json_errors ();
  test_json_paths ();
  test_offline_supplier ();
  test_config_defaults ();
  test_octopart_supplier ();
  test_octopart_graphql_error ();
  test_octopart_transport_failure ();

  g_print ("%d checks, %d failures\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
