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
/*! \todo STILL NEED to clean up line lengths in aa and tr */
#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"
#include <gtk/gtk.h>

#define GLADE_HOOKUP_OBJECT(component, widget, name) \
  g_object_set_data(G_OBJECT(component), name, widget)


/***************** Start of Close Confirmation dialog box ************/

#define TYPE_CLOSE_CONFIRMATION_DIALOG            (close_confirmation_dialog_get_type ())
#define CLOSE_CONFIRMATION_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_CLOSE_CONFIRMATION_DIALOG, CloseConfirmationDialog))
#define CLOSE_CONFIRMATION_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_CLOSE_CONFIRMATION_DIALOG, CloseConfirmationDialogClass))
#define IS_CLOSE_CONFIRMATION_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_CLOSE_CONFIRMATION_DIALOG))
#define IS_CLOSE_CONFIRMATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_CLOSE_CONFIRMATION_DIALOG))
#define CLOSE_CONFIRMATION_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),TYPE_CLOSE_CONFIRMATION_DIALOG, CloseConfirmationDialogClass))


typedef struct _CloseConfirmationDialog      CloseConfirmationDialog;
typedef struct _CloseConfirmationDialogClass CloseConfirmationDialogClass;

struct _CloseConfirmationDialog
{
  GtkDialog parent;

  GschemToplevel *w_current;

  GtkListStore *store_unsaved_pages;
};

struct _CloseConfirmationDialogClass
{
  GtkDialogClass parent_class;
};


enum {
  PROP_GSCHEM_TOPLEVEL = 1,
  PROP_UNSAVED_PAGE,
  PROP_UNSAVED_PAGES,
  PROP_SELECTED_PAGES
};

enum {
  COLUMN_SAVE,
  COLUMN_PAGE,
  NUM_COLUMNS
};


static gpointer close_confirmation_dialog_parent_class = NULL;


static void close_confirmation_dialog_class_init (CloseConfirmationDialogClass *klass);
static void close_confirmation_dialog_init (CloseConfirmationDialog *self);
static void close_confirmation_dialog_set_property (GObject      *object,
                                                    guint         property_id,
                                                    const GValue *value,
                                                    GParamSpec   *pspec);
static void close_confirmation_dialog_get_property (GObject      *object,
                                                    guint         property_id,
                                                    GValue       *value,
                                                    GParamSpec   *pspec);
static GObject* close_confirmation_dialog_constructor (GType type,
                                                       guint n_construct_properties,
                                                       GObjectConstructParam *construct_params);

GList *close_confirmation_dialog_get_selected_pages (CloseConfirmationDialog *dialog);



GType
close_confirmation_dialog_get_type ()
{
  static GType close_confirmation_dialog_type = 0;

  if (!close_confirmation_dialog_type) {
    static const GTypeInfo close_confirmation_dialog_info = {
      sizeof(CloseConfirmationDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) close_confirmation_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(CloseConfirmationDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) close_confirmation_dialog_init,
    };

    close_confirmation_dialog_type =
      g_type_register_static (GTK_TYPE_DIALOG,
                              "CloseConfirmationDialog",
                              &close_confirmation_dialog_info, 0);
  }

  return close_confirmation_dialog_type;
}

static void
close_confirmation_dialog_class_init (CloseConfirmationDialogClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  close_confirmation_dialog_parent_class = g_type_class_peek_parent (klass);

  gobject_class->constructor  = close_confirmation_dialog_constructor;
  gobject_class->set_property = close_confirmation_dialog_set_property;
  gobject_class->get_property = close_confirmation_dialog_get_property;

  g_object_class_install_property (
    gobject_class, PROP_GSCHEM_TOPLEVEL,
    g_param_spec_pointer ("gschem-toplevel",
                          "",
                          "",
                          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
  g_object_class_install_property (
    gobject_class, PROP_UNSAVED_PAGE,
    g_param_spec_pointer ("unsaved-page",
                          "",
                          "",
                          G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));
  g_object_class_install_property (
    gobject_class, PROP_UNSAVED_PAGES,
    g_param_spec_pointer ("unsaved-pages",
                          "",
                          "",
                          G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));
  g_object_class_install_property (
    gobject_class, PROP_SELECTED_PAGES,
    g_param_spec_pointer ("selected-pages",
                          "",
                          "",
                          G_PARAM_READABLE));

}

static void
close_confirmation_dialog_init (CloseConfirmationDialog *self)
{
  /* create model for treeview and populate */
  self->store_unsaved_pages = gtk_list_store_new (NUM_COLUMNS,
                                                  G_TYPE_BOOLEAN,  /* save? */
                                                  G_TYPE_POINTER); /* page */

}

/*! \brief Returns the number of pages in the model.
 *  \par Function Description
 *  This function determines the number of pages with unsaved changes
 *  from the model.
 *
 *  \param [in] model The tree model.
 *  \returns The number of pages with unsaved changes.
 */
static gint
count_pages (GtkTreeModel *model)
{
  GtkTreeIter iter;
  gint n_pages;

  gtk_tree_model_get_iter_first (model, &iter);
  for (n_pages = 1;
       gtk_tree_model_iter_next (model, &iter);
       n_pages++);

  return n_pages;
}

/*! \brief Returns the name to use for the given page in the model.
 *  \par Function Description
 *  This function determines the text to be used to identify a
 *  specific page from the model of pages with unsaved changes.
 *
 *  If <B>piter</B> is NULL, the name for the first page of the model
 *  is returned. Otherwise, it returns the name for the page defined
 *  by the pointed iterator.
 *
 *  The returned value must be freed by caller.
 *
 *  \param [in] model The tree model.
 *  \param [in] piter A pointer on a GtkTreeIter of model or NULL.
 *  \returns The name for the page.
 */
static gchar*
get_page_name (GtkTreeModel *model, GtkTreeIter *piter)
{
  GtkTreeIter iter;
  PAGE *page;

  g_return_val_if_fail (GTK_IS_TREE_MODEL (model), NULL);

  if (piter == NULL) {
    gtk_tree_model_get_iter_first (model, &iter);
  } else {
    iter = *piter;
  }

  gtk_tree_model_get (model, &iter,
                      COLUMN_PAGE, &page,
                      -1);
  g_assert (page != NULL && page->page_filename != NULL);
  if (page->is_untitled)
    return g_strdup (_("Untitled page"));
  return g_path_get_basename (page->page_filename);
}

/*! \brief Sets the contents of the name cell in the treeview of dialog.
 *  \par Function Description
 *  This functions sets the cell of the treeview with the short name
 *  of the page obtained with <B>get_page_name()</B>.
 *
 *  \param [in] tree_column A GtkTreeColumn.
 *  \param [in] cell        The GtkCellRenderer that is being rendered by
 *                        tree_column.
 *  \param [in] tree_model  The GtkTreeModel being rendered.
 *  \param [in] iter        A GtkTreeIter of the current row rendered.
 *  \param [in] data        .
 */
static void
close_confirmation_dialog_set_page_name (GtkTreeViewColumn *tree_column,
                                         GtkCellRenderer   *cell,
                                         GtkTreeModel      *tree_model,
                                         GtkTreeIter       *iter,
                                         gpointer           data)
{
  gchar *page_name;

  page_name = get_page_name (tree_model, iter);
  g_object_set (cell,
                "text", page_name,
                NULL);
  g_free (page_name);

}

/*! \brief Callback function for the toggled signal of check box in treeview.
 *  \par Function Description
 *  This functions changes the value of the save column in the model
 *  for the affected row when user toggles the check box in the
 *  treeview.
 *
 *  \param [in] cell_renderer The GtkCellRendererToggle.
 *  \param [in] path          The GtkTreePath to the concerned row in model.
 *  \param [in] user_data     The dialog as user data.
 */
static void
close_confirmation_dialog_callback_renderer_toggled (GtkCellRendererToggle *cell_renderer,
                                                     gchar                 *path,
                                                     gpointer               user_data)
{
  CloseConfirmationDialog *dialog = CLOSE_CONFIRMATION_DIALOG (user_data);
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean save;

  model = GTK_TREE_MODEL (dialog->store_unsaved_pages);

  if (!gtk_tree_model_get_iter_from_string (model, &iter, path)) {
    return;
  }
  gtk_tree_model_get (model, &iter,
                      COLUMN_SAVE, &save,
                      -1);
  gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                      COLUMN_SAVE, (save != TRUE),
                      -1);

}

/*! \brief Adds a treeview to confirmation dialog for selecting of pages.
 *  \par Function Description
 *  This function adds a treeview and caption to display the content
 *  of the dialog model of pages with unsaved changes.
 *
 *  The treeview displays the page names with check boxes.
 *
 *  \param [in] dialog The dialog.
 *  \returns A pointer on the GtkVBox to add to dialog.
 */
static GtkWidget*
close_confirmation_dialog_build_page_list (CloseConfirmationDialog *dialog)
{
  GtkWidget *vbox, *scrolled_window, *treeview, *label;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  const gchar *text;

  /* place the treeview and its caption into their own box */
  vbox = GTK_WIDGET (g_object_new (GTK_TYPE_VBOX,
                                   /* GtkBox */
                                   "homogeneous", FALSE,
                                   "spacing",     8,
                                   NULL));

  /* the list of pages with changes */
  /*  - scrolled window as container for the treeview first */
  scrolled_window = GTK_WIDGET (g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                                              /* GtkScrolledWindow */
                                              "hscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                              "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                              "shadow-type",       GTK_SHADOW_IN,
                                              NULL));
  /*  - then the treeview */
  /* create model for treeview and populate */
  treeview = GTK_WIDGET (g_object_new (GTK_TYPE_TREE_VIEW,
                                       /* GtkTreeView */
                                       "enable-search",   FALSE,
                                       "headers-visible", FALSE,
                                       "model",           dialog->store_unsaved_pages,
                                       NULL));
  renderer = gtk_cell_renderer_toggle_new ();
  g_signal_connect (renderer, "toggled",
                    G_CALLBACK (
                      close_confirmation_dialog_callback_renderer_toggled),
                    dialog);
  column   = gtk_tree_view_column_new_with_attributes ("Save?",
                                                       renderer,
                                                       "active", COLUMN_SAVE,
                                                       NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  column = GTK_TREE_VIEW_COLUMN (
    g_object_new (GTK_TYPE_TREE_VIEW_COLUMN,
                  /* GtkTreeViewColumn */
                  "title", _("Name"),
                  NULL));
  gtk_tree_view_column_pack_start (column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func (column, renderer,
                                           close_confirmation_dialog_set_page_name,
                                           NULL, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);

  gtk_box_pack_end (GTK_BOX (vbox), scrolled_window,
                    TRUE, TRUE, 0);

  /* the caption label above the list of pages */
  label = GTK_WIDGET (g_object_new (GTK_TYPE_LABEL,
                                    /* GtkMisc */
                                    "xalign",          0.0,
                                    "yalign",          0.0,
                                    /* GtkLabel */
                                    "wrap",            TRUE,
                                    "mnemonic-widget", treeview,
                                    NULL));
  text = _("S_elect the files you want to save:");
  gtk_label_set_text_with_mnemonic (GTK_LABEL (label), text);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), treeview);
  gtk_box_pack_start (GTK_BOX (vbox), label,
                      FALSE, FALSE, 0);

  return vbox;
}

static GObject*
close_confirmation_dialog_constructor(GType type,
                                      guint n_construct_properties,
                                      GObjectConstructParam *construct_params)
{
  GObject *object;
  CloseConfirmationDialog *dialog;
  GtkWidget *hbox, *image, *vbox, *label;
  GtkTreeIter iter;
  gboolean ret, single_page;
  gchar *tmp, *str;
  const gchar *cstr;

  /* chain up to constructor of parent class */
  object = G_OBJECT_CLASS(close_confirmation_dialog_parent_class)->constructor(
    type, n_construct_properties, construct_params);
  dialog = CLOSE_CONFIRMATION_DIALOG(object);

  gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(dialog), TRUE);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);

  GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_box_set_spacing(GTK_BOX(content_area), 14);

  GtkWidget *action_area = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
  gtk_box_set_spacing(GTK_BOX(action_area), 6);
  gtk_container_set_border_width(GTK_CONTAINER(action_area), 5);

  /* check if there is one or more than one page with changes */
  ret = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(dialog->store_unsaved_pages), &iter);
  g_assert(ret);
  single_page = !gtk_tree_model_iter_next(GTK_TREE_MODEL(dialog->store_unsaved_pages), &iter);

  /* layout the dialog */
  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);

  image = gtk_image_new_from_icon_name("dialog-warning", GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

  /* primary label */
  if (single_page) {
    GtkTreeModel *model = GTK_TREE_MODEL(dialog->store_unsaved_pages);
    GtkTreeIter iter;
    PAGE *page;

    gtk_tree_model_get_iter_first(model, &iter);
    gtk_tree_model_get(model, &iter, COLUMN_PAGE, &page, -1);
    g_assert(page != NULL);

    x_window_set_current_page(dialog->w_current, page);

    if (page->is_untitled) {
      tmp = g_strdup(_("Save changes before closing?"));
    } else {
      gchar *page_name = g_path_get_basename(page->page_filename);
      tmp = g_strdup_printf(_("Save the changes to \"%s\" before closing?"), page_name);
      g_free(page_name);
    }
  } else {
    tmp = g_strdup_printf(
      _("There are %d files with unsaved changes.\nSave changes before closing?"),
      count_pages(GTK_TREE_MODEL(dialog->store_unsaved_pages)));
  }

  str = g_strconcat("<big><b>", tmp, "</b></big>", NULL);
  g_free(tmp);

  label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), str);
  // gtk_label_set_wrap(GTK_LABEL(label), TRUE);  ??? depracated in GTK 3?
  gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_START);
  gtk_label_set_selectable(GTK_LABEL(label), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
  g_free(str);

  if (!single_page) {
    gtk_box_pack_start(GTK_BOX(vbox),
                       close_confirmation_dialog_build_page_list(dialog),
                       FALSE, FALSE, 0);
  }

  /* secondary label */
  cstr = _("If you don't save, all your changes will be permanently lost.");
  label = gtk_label_new(cstr);
  gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_START);
  gtk_label_set_selectable(GTK_LABEL(label), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);

  /* add buttons */
  gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                         _("Close _without saving"), GTK_RESPONSE_NO,
                         _("_Cancel"),              GTK_RESPONSE_CANCEL,
                         _("_Save"),                GTK_RESPONSE_YES,
                         NULL);

  gtk_dialog_set_alternative_button_order(GTK_DIALOG(dialog),
                                          GTK_RESPONSE_YES,
                                          GTK_RESPONSE_NO,
                                          GTK_RESPONSE_CANCEL,
                                          -1);

  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_YES);

  gtk_widget_show_all(hbox);
  gtk_box_pack_start(GTK_BOX(content_area), hbox, FALSE, FALSE, 0);

  return object;
}

static void
close_confirmation_dialog_set_property (GObject      *object,
                                        guint         property_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  CloseConfirmationDialog *dialog = CLOSE_CONFIRMATION_DIALOG (object);
  GtkTreeIter iter;
  gpointer data;
  GList *p_current;

  switch(property_id) {
    case PROP_GSCHEM_TOPLEVEL:
      dialog->w_current = GSCHEM_TOPLEVEL (g_value_get_pointer (value));
      break;

    case PROP_UNSAVED_PAGE:
      data = g_value_get_pointer (value);
      if (data != NULL) {
        /* add single page to model */
        gtk_list_store_append (dialog->store_unsaved_pages,
                               &iter);
        gtk_list_store_set (dialog->store_unsaved_pages,
                            &iter,
                            COLUMN_SAVE, TRUE,
                            COLUMN_PAGE, data,
                            -1);
      }
      break;

    case PROP_UNSAVED_PAGES:
      data = g_value_get_pointer (value);
      /* add set of pages to model */
      for (p_current = (GList*)data;
           p_current != NULL;
           p_current = g_list_next (p_current)) {
        gtk_list_store_append (dialog->store_unsaved_pages,
                               &iter);
        gtk_list_store_set (dialog->store_unsaved_pages,
                            &iter,
                            COLUMN_SAVE, TRUE,
                            COLUMN_PAGE, p_current->data,
                            -1);
      }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }

}

static void
close_confirmation_dialog_get_property (GObject    *object,
                                        guint       property_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  CloseConfirmationDialog *dialog = CLOSE_CONFIRMATION_DIALOG (object);

  switch(property_id) {
    case PROP_GSCHEM_TOPLEVEL:
      g_value_set_pointer (value, dialog->w_current);
      break;

    case PROP_SELECTED_PAGES:
      g_value_set_pointer (
        value,
        close_confirmation_dialog_get_selected_pages (dialog));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }

}

/*! \brief Helps building a list of selected page to save.
 *  \par Function Description
 *  This is the <B>GtkTreeModelForeachFunc</B> for function
 *  <B>close_confirmation_dialog_get_selected_pages()</B>.
 *
 *  It builds from the tree model a list of PAGEs for which a save
 *  action has been requested. Each selected page is appended to the
 *  GList pointed by <B>data</B>
 *
 *  \param [in] model The tree model.
 *  \param [in] path  .
 *  \param [in] iter  .
 *  \param [in] data  A pointer on a GList* to fill.
 *  \returns FALSE to continue walking the tree.
 */
static gboolean
get_selected_pages (GtkTreeModel *model,
                    GtkTreePath  *path,
                    GtkTreeIter  *iter,
                    gpointer     data)
{
  PAGE *page;
  gboolean save;

  gtk_tree_model_get (model, iter,
                      COLUMN_SAVE, &save,
                      COLUMN_PAGE, &page,
                      -1);
  if (save) {
    g_assert (page != NULL);
    *(GList**)data = g_list_append (*(GList**)data, page);
  }

  return FALSE;
}

/*! \brief Returns a list of the selected pages with changes to save.
 *  \par Function Description
 *  This function returns the pages that the user has selected in the
 *  confirmation dialog.
 *
 *  The returned list must be freed.
 *
 *  \param [in] dialog The dialog.
 *  \returns A GList of selected PAGE* in dialog.
 */
GList*
close_confirmation_dialog_get_selected_pages (CloseConfirmationDialog *dialog)
{
  GList *selected = NULL;

  gtk_tree_model_foreach (GTK_TREE_MODEL (dialog->store_unsaved_pages),
                          (GtkTreeModelForeachFunc)get_selected_pages,
                          &selected);

  return selected;
}


/*! \brief Asks for confirmation before closing a changed page.
 *  \par Function Description
 *  This function asks the user to confirm its closing order for
 *  page <B>page</B> while it still has unsaved changes.
 *
 *  It displays a message dialog inviting the user to cancel the
 *  closing, or to discard the changes or to save the changes to a
 *  file.
 *
 *  \param [in] w_current The toplevel environment.
 *  \param [in] page      The page to close.
 *
 *  \return TRUE if okay to continue with closing page, FALSE
 *  otherwise.
 */
gboolean
x_dialog_close_changed_page (GschemToplevel *w_current, PAGE *page)
{
  GtkWidget *dialog;
  gint response_id;
  gboolean result = FALSE;

  g_return_val_if_fail (page != NULL && page->CHANGED, TRUE);

  dialog = GTK_WIDGET (g_object_new (TYPE_CLOSE_CONFIRMATION_DIALOG,
                                     "gschem-toplevel", w_current,
                                     "unsaved-page", page,
                                     NULL));
  /* set default response signal. This is usually triggered by the
     "Return" key */
  gtk_dialog_set_default_response(GTK_DIALOG(dialog),
                                  GTK_RESPONSE_YES);

  response_id = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

  switch (response_id) {
      case GTK_RESPONSE_NO:
        /* action selected: close without saving */
        /* close the page, discard changes */
        result = TRUE;
        break;


      case GTK_RESPONSE_YES:
        /* action selected: save */
        if (x_highlevel_save_page (w_current, page))
          result = TRUE;
        /* no, user has cancelled the save and page has changes */
        /* do not close page */
        break;

      case GTK_RESPONSE_CANCEL:
        /* action selected: cancel */
        /* fall through */
      default:
        /* Hit when the user breaks out of the dialog with the escape key
         * or otherwise destroys the dialog window without a proper response */
        /* nothing to do */
        break;
  }

  return result;
}

/*! \brief Asks for confirmation before closing a window.
 *  \par Function Description
 *  This function asks the user to confirm its closing order for
 *  the given window.
 *
 *  The user is given the possibility to save the pages that currently
 *  have unsaved changes, if any.
 *
 *  It returns TRUE if the user really accepts the close of the
 *  window. Otherwise the user has somehow cancelled and the window
 *  must not be closed.
 *
 *  \param [in] w_current The toplevel environment.
 *  \returns TRUE if the window can be closed, FALSE otherwise.
 */
gboolean
x_dialog_close_window (GschemToplevel *w_current)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  GList *iter;
  GtkWidget *dialog;
  PAGE *p_current;
  GList *unsaved_pages, *p_unsaved;
  gboolean ret = FALSE;

  for ( iter = geda_list_get_glist( toplevel->pages ), unsaved_pages = NULL;
        iter != NULL;
        iter = g_list_next( iter ) ) {

    p_current = (PAGE*)iter->data;

    if (p_current->CHANGED) {
      unsaved_pages = g_list_append (unsaved_pages, (gpointer)p_current);
    }
  }

  if (unsaved_pages == NULL) {
    /* no page with unsaved changes, close window */
    return TRUE;
  }

  dialog = GTK_WIDGET (g_object_new (TYPE_CLOSE_CONFIRMATION_DIALOG,
                                     "gschem-toplevel", w_current,
                                     "unsaved-pages", unsaved_pages,
                                     NULL));

  gtk_window_set_transient_for (GTK_WINDOW (dialog),
                                GTK_WINDOW (w_current->main_window));

  g_list_free (unsaved_pages);
  unsaved_pages = NULL;

  switch (gtk_dialog_run (GTK_DIALOG (dialog))) {
      case GTK_RESPONSE_NO:
        /* action selected: close without saving */
        /* discard changes, ok to close window */
        ret = TRUE;
        break;

      case GTK_RESPONSE_YES:
        /* action selected: save */
        g_object_get (dialog,
                      "selected-pages", &unsaved_pages,
                      NULL);
        ret = TRUE;
        break;

      case GTK_RESPONSE_CANCEL:
        /* action selected: cancel */
        /* fall through */
      default:
        /* Hit when the user breaks out of the dialog with the escape key
         * or otherwise destroys the dialog window without a proper response */
        ret = FALSE;
        break;
  }
  gtk_widget_destroy (dialog);

  /* if response was "save", save all selected pages */
  for (p_unsaved = unsaved_pages;
       p_unsaved != NULL;
       p_unsaved = g_list_next (p_unsaved)) {
    p_current = (PAGE *) p_unsaved->data;

    if (!x_highlevel_save_page (w_current, p_current))
      /* if user cancelled save, do not close window */
      ret = FALSE;
  }
  g_list_free (unsaved_pages);

  return ret;
}

/***************** End of Close Confirmation dialog box **************/
