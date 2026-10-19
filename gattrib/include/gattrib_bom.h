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
 * \file gattrib_bom.h
 * \brief Hooks for enriching the component table with distributor data.
 *
 * gattrib knows a part's refdes, value and any attributes the designer
 * typed in, which is enough to build a bill of materials but tells you
 * nothing about whether the part can be bought, what it costs, or what its
 * datasheet says.  This module is the seam for fixing that: it takes a part
 * and annotates it with manufacturer part numbers, stock, price breaks and
 * datasheet links obtained from a supplier.
 *
 * Two design decisions matter here.
 *
 * First, the network is behind a callback (GattribBomTransport) rather than
 * hard-coded, so the whole pipeline can be exercised by the test suite with
 * no credentials and no sockets.  The default transport uses libcurl when
 * it was available at configure time and otherwise shells out to curl(1).
 *
 * Second, the supplier itself is an interface.  GattribBomSupplier is an
 * opaque struct of function pointers, and the Octopart/Nexar implementation
 * lives behind it.  That keeps the sheet code free of GraphQL and makes it
 * possible to add Mouser, Digi-Key or a local cache without touching it.
 *
 * The Octopart API gained a new home in 2022: the old
 * octopart.com/api/v4 REST interface is retired and the supported product is
 * the Nexar API, whose "sup" (supply) GraphQL query is served from
 * https://api.nexar.com/graphql and authorised by an OAuth2 client-credentials
 * token from https://identity.nexar.com/connect/token.  Credentials are read
 * from the environment (see gattrib_bom_config_new()) and never written to
 * the schematic or the exported CSV.
 */

#ifndef GATTRIB_BOM_H
#define GATTRIB_BOM_H

#include <glib.h>

/*! \brief An HTTP exchange: caller supplies a body, callee fills in the rest.
 *
 *  \param user_data      Set when the transport is installed.
 *  \param url            Endpoint to call.
 *  \param content_type   MIME type of \a body, or NULL for a GET.
 *  \param body           Request body, or NULL.
 *  \param status_out     Receives the HTTP status code, or 0 if the request
 *                        did not complete.
 *  \param response_out   Receives a newly-allocated response body.
 *  \param error          Set on transport-level (not HTTP-level) failure.
 *  \return TRUE if an HTTP response was obtained, whatever its status.
 */
typedef gboolean (*GattribBomTransport) (gpointer user_data,
                                         const gchar *url,
                                         const gchar *content_type,
                                         const gchar *body,
                                         gint *status_out,
                                         gchar **response_out,
                                         GError **error);

/*! \brief A single distributor offer for a part. */
typedef struct {
  gchar  *seller;            /*!< Distributor name, e.g. "Digi-Key". */
  gchar  *sku;               /*!< Distributor's own part number. */
  gint    stock;             /*!< Units in stock; 0 when unknown. */
  gint    moq;               /*!< Minimum order quantity; 1 when unknown. */
  gdouble unit_price;        /*!< Unit price in \c currency at \a quantity. */
  gint    quantity;          /*!< Quantity tier this price applies to. */
  gchar  *currency;          /*!< ISO 4217 code, e.g. "USD". */
  gchar  *url;               /*!< Product page, may be NULL. */
} GattribBomOffer;

/*! \brief Everything a supplier could tell us about one part. */
typedef struct {
  gchar  *mpn;               /*!< Manufacturer part number. */
  gchar  *manufacturer;      /*!< Manufacturer name. */
  gchar  *description;       /*!< Short description. */
  gchar  *datasheet_url;     /*!< Datasheet PDF link, may be NULL. */
  gchar  *lifecycle;         /*!< e.g. "Active", "Obsolete"; may be NULL. */

  gboolean  found;           /*!< FALSE when the lookup matched nothing. */
  GPtrArray *offers;         /*!< GattribBomOffer*, best offer first. */
} GattribBomPartInfo;


/*! \brief Supplier interface.  Treat as opaque; use the constructors below. */
typedef struct _GattribBomSupplier GattribBomSupplier;

/*! \brief Credentials and endpoint configuration for a supplier. */
typedef struct {
  gchar *client_id;
  gchar *client_secret;
  gchar *token_url;
  gchar *graphql_url;
  gchar *currency;
  gchar *country;
  gint   timeout_seconds;
} GattribBomConfig;

/* Lifecycle ------------------------------------------------------------- */

/*! \brief Build a config from the environment, falling back to defaults.
 *
 *  A config with no credentials is still valid and simply cannot make
 *  authenticated calls, which is what lets the UI report "not configured"
 *  instead of failing obscurely. */
GattribBomConfig *gattrib_bom_config_new (void);
void gattrib_bom_config_free (GattribBomConfig *config);

/*! \brief TRUE when the config carries a usable client id and secret. */
gboolean gattrib_bom_config_is_configured (const GattribBomConfig *config);

/*! \brief Create the Octopart (Nexar) supplier.
 *
 *  \param config     Credentials; copied, and may be NULL.
 *  \param transport  HTTP implementation; NULL selects the built-in one.
 *  \param user_data  Passed to \a transport.
 */
GattribBomSupplier *gattrib_bom_supplier_new_octopart (
    const GattribBomConfig *config,
    GattribBomTransport transport,
    gpointer user_data);

/*! \brief Create a supplier backed by canned data.  Needs no network.
 *
 *  Useful for exercising the UI and the tests; every query returns a small,
 *  deterministic result so assertions are stable. */
GattribBomSupplier *gattrib_bom_supplier_new_offline (void);

/*! \brief Free a supplier and its transport. */
void gattrib_bom_supplier_free (GattribBomSupplier *supplier);

/*! \brief The supplier's name, for messages. */
const gchar *gattrib_bom_supplier_get_name (GattribBomSupplier *supplier);

/* Data ------------------------------------------------------------------ */

void gattrib_bom_part_info_free (GattribBomPartInfo *info);

/*! \brief Look up one part.
 *
 *  \param supplier  Supplier to ask.
 *  \param mpn       Manufacturer part number to search for.
 *  \param error     Set when the call fails; a part that is merely absent
 *                   yields info->found == FALSE and no error.
 *  \return A newly-allocated result, or NULL on failure.
 */
GattribBomPartInfo *gattrib_bom_supplier_lookup (GattribBomSupplier *supplier,
                                                 const gchar *mpn,
                                                 GError **error);

/*! \brief Format the best offer as a compact price summary.
 *
 *  The result looks like "USD 1.2400 @100 (1000 in stock)" and is suitable
 *  for a spreadsheet cell.  Returns NULL when there are no offers.  Free
 *  with g_free(). */
gchar *gattrib_bom_format_best_offer (const GattribBomPartInfo *info);

#endif /* GATTRIB_BOM_H */
