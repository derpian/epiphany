/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  Copyright © 2002-2004 Marco Pesenti Gritti
 *  Copyright © 2003, 2004 Christian Persch
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "ephy-bookmarks.h"

#include "ephy-bookmark-properties.h"
#include "ephy-bookmarks-export.h"
#include "ephy-bookmarks-import.h"
#include "ephy-bookmarks-type-builtins.h"
#include "ephy-debug.h"
#include "ephy-embed-shell.h"
#include "ephy-file-helpers.h"
#include "ephy-history-service.h"
#include "ephy-node-common.h"
#include "ephy-prefs.h"
#include "ephy-profile-utils.h"
#include "ephy-settings.h"
#include "ephy-shell.h"
#include "ephy-signal-accumulator.h"
#include "ephy-tree-model-node.h"


#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <string.h>

#define EPHY_BOOKMARKS_XML_ROOT    "ephy_bookmarks"
#define EPHY_BOOKMARKS_XML_VERSION "1.03"
#define BOOKMARKS_SAVE_DELAY 3 /* seconds */
#define UPDATE_URI_DATA_KEY "updated-uri"

static const char zeroconf_protos[3][6] =
{
  "http",
  "https",
  "ftp"
};

struct _EphyBookmarks {
  GObject parent_instance;

  gboolean init_defaults;
  gboolean dirty;
  guint save_timeout_id;
  char *xml_file;
  char *rdf_file;
  EphyNodeDb *db;
  EphyNode *bookmarks;
  EphyNode *keywords;
  EphyNode *notcategorized;
  EphyNode *smartbookmarks;
  EphyNode *lower_fav;
  double lower_score;

  /* Local sites */
  EphyNode *local;
  GHashTable *resolve_handles;
};

static const char *default_topics [] =
{
  N_("Entertainment"),
  N_("News"),
  N_("Shopping"),
  N_("Sports"),
  N_("Travel"),
  N_("Work")
};

/* Signals */
enum {
  TREE_CHANGED,
  LAST_SIGNAL
};

static guint ephy_bookmarks_signals[LAST_SIGNAL];

static void ephy_bookmarks_finalize (GObject *object);
static void ephy_local_bookmarks_start_client (EphyBookmarks *bookmarks);

G_DEFINE_TYPE (EphyBookmarks, ephy_bookmarks, G_TYPE_OBJECT)

static void
ephy_bookmarks_init_defaults (EphyBookmarks *eb)
{
  guint i;

  for (i = 0; i < G_N_ELEMENTS (default_topics); i++) {
    ephy_bookmarks_add_keyword (eb, _(default_topics[i]));
  }

  ephy_bookmarks_import_rdf (eb, DATADIR "/default-bookmarks.rdf");
}

static void
ephy_bookmarks_class_init (EphyBookmarksClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = ephy_bookmarks_finalize;

  ephy_bookmarks_signals[TREE_CHANGED] =
    g_signal_new ("tree-changed",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL,
                  G_TYPE_NONE,
                  0);
}

static gboolean
save_filter (EphyNode      *node,
             EphyBookmarks *bookmarks)
{
  return node != bookmarks->bookmarks &&
         node != bookmarks->notcategorized &&
         node != bookmarks->local;
}

static gboolean
save_filter_local (EphyNode      *node,
                   EphyBookmarks *bookmarks)
{
  return !ephy_node_has_child (bookmarks->local, node);
}

static void
ephy_bookmarks_save (EphyBookmarks *eb)
{
  LOG ("Saving bookmarks");

  ephy_node_db_write_to_xml_safe
    (eb->db,
    (xmlChar *)eb->xml_file,
    (xmlChar *)EPHY_BOOKMARKS_XML_ROOT,
    (xmlChar *)EPHY_BOOKMARKS_XML_VERSION,
    (xmlChar *)"Do not rely on this file, it's only for internal use. Use bookmarks.rdf instead.",
    eb->keywords, (EphyNodeFilterFunc)save_filter, eb,
    eb->bookmarks, (EphyNodeFilterFunc)save_filter_local, eb,
    NULL);

  /* Export bookmarks in rdf */
  ephy_bookmarks_export_rdf (eb, eb->rdf_file);
}

static gboolean
save_bookmarks_delayed (EphyBookmarks *bookmarks)
{
  ephy_bookmarks_save (bookmarks);
  bookmarks->dirty = FALSE;
  bookmarks->save_timeout_id = 0;

  return FALSE;
}

static void
ephy_bookmarks_save_delayed (EphyBookmarks *bookmarks, int delay)
{
  if (!bookmarks->dirty) {
    bookmarks->dirty = TRUE;

    if (delay > 0) {
      bookmarks->save_timeout_id =
        g_timeout_add_seconds (BOOKMARKS_SAVE_DELAY,
                               (GSourceFunc)save_bookmarks_delayed,
                               bookmarks);
      g_source_set_name_by_id (bookmarks->save_timeout_id, "[epiphany] save_bookmarks_delayed");
    } else {
      bookmarks->save_timeout_id =
        g_idle_add ((GSourceFunc)save_bookmarks_delayed,
                    bookmarks);
    }
  }
}

static void
icon_updated_cb (WebKitFaviconDatabase *favicon_database,
                 const char            *address,
                 const char            *icon,
                 EphyBookmarks         *eb)
{
  ephy_bookmarks_set_icon (eb, address, icon);
}

static void
ephy_setup_history_notifiers (EphyBookmarks *eb)
{
  WebKitFaviconDatabase *favicon_database;
  EphyEmbedShell *shell = ephy_embed_shell_get_default ();

  favicon_database = webkit_web_context_get_favicon_database (ephy_embed_shell_get_web_context (shell));
  g_signal_connect (favicon_database, "favicon-changed",
                    G_CALLBACK (icon_updated_cb), eb);
}

static void
update_bookmark_keywords (EphyBookmarks *eb, EphyNode *bookmark)
{
  GPtrArray *children;
  guint i;
  GString *list;
  const char *title;
  char *normalized_keywords, *case_normalized_keywords;

  list = g_string_new (NULL);

  children = ephy_node_get_children (eb->keywords);
  for (i = 0; i < children->len; i++) {
    EphyNode *kid;

    kid = g_ptr_array_index (children, i);

    if (kid != eb->notcategorized &&
        kid != eb->bookmarks &&
        kid != eb->local &&
        ephy_node_has_child (kid, bookmark)) {
      const char *topic;
      topic = ephy_node_get_property_string
                (kid, EPHY_NODE_KEYWORD_PROP_NAME);
      g_string_append (list, topic);
      g_string_append (list, " ");
    }
  }

  title = ephy_node_get_property_string
            (bookmark, EPHY_NODE_BMK_PROP_TITLE);
  g_string_append (list, " ");
  g_string_append (list, title);

  normalized_keywords = g_utf8_normalize (list->str, -1, G_NORMALIZE_ALL);
  case_normalized_keywords = g_utf8_casefold (normalized_keywords, -1);

  ephy_node_set_property_string (bookmark, EPHY_NODE_BMK_PROP_KEYWORDS,
                                 case_normalized_keywords);

  g_string_free (list, TRUE);
  g_free (normalized_keywords);
  g_free (case_normalized_keywords);
}

static void
bookmarks_changed_cb (EphyNode      *node,
                      EphyNode      *child,
                      guint          property_id,
                      EphyBookmarks *eb)
{
  if (property_id == EPHY_NODE_BMK_PROP_TITLE) {
    update_bookmark_keywords (eb, child);
  }

  ephy_bookmarks_save_delayed (eb, BOOKMARKS_SAVE_DELAY);
}

static void
bookmarks_removed_cb (EphyNode      *node,
                      EphyNode      *child,
                      guint          old_index,
                      EphyBookmarks *eb)
{
  ephy_bookmarks_save_delayed (eb, BOOKMARKS_SAVE_DELAY);
}

static gboolean
bookmark_is_categorized (EphyBookmarks *eb, EphyNode *bookmark)
{
  GPtrArray *children;
  guint i;

  children = ephy_node_get_children (eb->keywords);
  for (i = 0; i < children->len; i++) {
    EphyNode *kid;

    kid = g_ptr_array_index (children, i);

    if (kid != eb->notcategorized &&
        kid != eb->bookmarks &&
        kid != eb->local &&
        ephy_node_has_child (kid, bookmark)) {
      return TRUE;
    }
  }

  return FALSE;
}

static void
topics_removed_cb (EphyNode      *node,
                   EphyNode      *child,
                   guint          old_index,
                   EphyBookmarks *eb)
{
  GPtrArray *children;
  guint i;

  children = ephy_node_get_children (child);
  for (i = 0; i < children->len; i++) {
    EphyNode *kid;

    kid = g_ptr_array_index (children, i);

    if (!bookmark_is_categorized (eb, kid) &&
        !ephy_node_has_child (eb->notcategorized, kid)) {
      ephy_node_add_child
        (eb->notcategorized, kid);
    }

    update_bookmark_keywords (eb, kid);
  }
}

static void
fix_hierarchy_topic (EphyBookmarks *eb,
                     EphyNode      *topic)
{
  GPtrArray *children;
  EphyNode *bookmark;
  const char *name;
  char **split;
  guint i, j;

  children = ephy_node_get_children (topic);
  name = ephy_node_get_property_string (topic, EPHY_NODE_KEYWORD_PROP_NAME);
  split = g_strsplit (name, "->", -1);

  for (i = 0; split[i]; i++) {
    if (split[i][0] == '\0') continue;

    topic = ephy_bookmarks_find_keyword (eb, split[i], FALSE);
    if (topic == NULL) {
      topic = ephy_bookmarks_add_keyword (eb, split[i]);
    }
    for (j = 0; j < children->len; j++) {
      bookmark = g_ptr_array_index (children, j);
      ephy_bookmarks_set_keyword (eb, topic, bookmark);
    }
  }

  g_strfreev (split);
}

static void
fix_hierarchy (EphyBookmarks *eb)
{
  GPtrArray *topics;
  EphyNode *topic;
  const char *name;
  int i;

  topics = ephy_node_get_children (eb->keywords);
  for (i = (int)topics->len - 1; i >= 0; i--) {
    topic = (EphyNode *)g_ptr_array_index (topics, i);
    name = ephy_node_get_property_string
             (topic, EPHY_NODE_KEYWORD_PROP_NAME);
    if (strstr (name, "->") != NULL) {
      fix_hierarchy_topic (eb, topic);
      ephy_node_remove_child (eb->keywords, topic);
    }
  }
}

static void
backup_file (const char *original_filename, const char *extension)
{
  char *template, *backup_filename;
  int result = 0;

  if (g_file_test (original_filename, G_FILE_TEST_EXISTS) == FALSE) {
    return;
  }

  template = g_strconcat (original_filename, ".backup-XXXXXX", NULL);
  backup_filename = ephy_file_tmp_filename (template, extension);

  if (backup_filename != NULL) {
    result = rename (original_filename, backup_filename);
  }

  if (result >= 0) {
    g_message ("Your old bookmarks file was backed up as \"%s\".\n",
               backup_filename);
  } else {
    g_warning ("Backup failed! Your old bookmarks file was lost.\n");
  }

  g_free (template);
  g_free (backup_filename);
}

/* End of copied code */

static char *
get_id_for_response (const char *type,
                     const char *domain,
                     const char *name)
{
  /* FIXME: limit length! */
  return g_strdup_printf ("%s\1%s\1%s",
                          type,
                          domain,
                          name);
}

typedef struct {
  EphyBookmarks *bookmarks;
  EphyNode *node;
  char *name;
  char *type;
  char *domain;
} ResolveData;

static void
resolve_data_free (ResolveData *data)
{
  g_free (data->type);
  g_free (data->name);
  g_free (data->domain);
  g_slice_free (ResolveData, data);
}

static void
ephy_local_bookmarks_start_client (EphyBookmarks *bookmarks)
{
}

static void
ephy_local_bookmarks_init (EphyBookmarks *bookmarks)
{
  bookmarks->resolve_handles = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                      g_free,
                                                      (GDestroyNotify)resolve_data_free);
  ephy_local_bookmarks_start_client (bookmarks);
}

static void
ephy_local_bookmarks_stop (EphyBookmarks *bookmarks)
{
  guint i;

  if (bookmarks->resolve_handles != NULL) {
    g_hash_table_destroy (bookmarks->resolve_handles);
    bookmarks->resolve_handles = NULL;
  }

  if (bookmarks->local != NULL) {
    ephy_node_unref (bookmarks->local);
    bookmarks->local = NULL;
  }

}

static void
ephy_bookmarks_init (EphyBookmarks *eb)
{
  EphyNodeDb *db;

  /* Translators: this topic contains all bookmarks */
  const char *bk_all = C_("bookmarks", "All");

  /* Translators: this topic contains the not categorized
     bookmarks */
  const char *bk_not_categorized = C_("bookmarks", "Not Categorized");

  /* Translators: this is an automatic topic containing local
   * websites bookmarks autodiscovered with zeroconf. */
  const char *bk_local_sites = C_("bookmarks", "Nearby Sites");

  db = ephy_node_db_new (EPHY_NODE_DB_BOOKMARKS);
  eb->db = db;

  eb->xml_file = g_build_filename (ephy_dot_dir (),
                                   EPHY_BOOKMARKS_FILE,
                                   NULL);
  eb->rdf_file = g_build_filename (ephy_dot_dir (),
                                   EPHY_BOOKMARKS_FILE_RDF,
                                   NULL);

  /* Bookmarks */
  eb->bookmarks = ephy_node_new_with_id (db, BOOKMARKS_NODE_ID);

  ephy_node_set_property_string (eb->bookmarks,
                                 EPHY_NODE_KEYWORD_PROP_NAME,
                                 bk_all);
  ephy_node_signal_connect_object (eb->bookmarks,
                                   EPHY_NODE_CHILD_REMOVED,
                                   (EphyNodeCallback)bookmarks_removed_cb,
                                   G_OBJECT (eb));
  ephy_node_signal_connect_object (eb->bookmarks,
                                   EPHY_NODE_CHILD_CHANGED,
                                   (EphyNodeCallback)bookmarks_changed_cb,
                                   G_OBJECT (eb));

  /* Keywords */
  eb->keywords = ephy_node_new_with_id (db, KEYWORDS_NODE_ID);
  ephy_node_set_property_int (eb->bookmarks,
                              EPHY_NODE_KEYWORD_PROP_PRIORITY,
                              EPHY_NODE_ALL_PRIORITY);

  ephy_node_signal_connect_object (eb->keywords,
                                   EPHY_NODE_CHILD_REMOVED,
                                   (EphyNodeCallback)topics_removed_cb,
                                   G_OBJECT (eb));

  ephy_node_add_child (eb->keywords,
                       eb->bookmarks);

  /* Not categorized */
  eb->notcategorized = ephy_node_new_with_id (db, BMKS_NOTCATEGORIZED_NODE_ID);


  ephy_node_set_property_string (eb->notcategorized,
                                 EPHY_NODE_KEYWORD_PROP_NAME,
                                 bk_not_categorized);

  ephy_node_set_property_int (eb->notcategorized,
                              EPHY_NODE_KEYWORD_PROP_PRIORITY,
                              EPHY_NODE_SPECIAL_PRIORITY);

  ephy_node_add_child (eb->keywords, eb->notcategorized);

  /* Local Websites */
  eb->local = ephy_node_new_with_id (db, BMKS_LOCAL_NODE_ID);

  /* don't allow drags to this topic */
  ephy_node_set_is_drag_dest (eb->local, FALSE);


  ephy_node_set_property_string (eb->local,
                                 EPHY_NODE_KEYWORD_PROP_NAME,
                                 bk_local_sites);
  ephy_node_set_property_int (eb->local,
                              EPHY_NODE_KEYWORD_PROP_PRIORITY,
                              EPHY_NODE_SPECIAL_PRIORITY);

  ephy_node_add_child (eb->keywords, eb->local);
  ephy_local_bookmarks_init (eb);

  /* Smart bookmarks */
  eb->smartbookmarks = ephy_node_new_with_id (db, SMARTBOOKMARKS_NODE_ID);

  if (g_file_test (eb->xml_file, G_FILE_TEST_EXISTS) == FALSE
      && g_file_test (eb->rdf_file, G_FILE_TEST_EXISTS) == FALSE) {
    eb->init_defaults = TRUE;
  } else if (ephy_node_db_load_from_file (eb->db, eb->xml_file,
                                          (xmlChar *)EPHY_BOOKMARKS_XML_ROOT,
                                          (xmlChar *)EPHY_BOOKMARKS_XML_VERSION) == FALSE) {
    /* save the corrupted files so the user can late try to
     * manually recover them. See bug #128308.
     */

    g_warning ("Could not read bookmarks file \"%s\", trying to "
               "re-import bookmarks from \"%s\"\n",
               eb->xml_file, eb->rdf_file);

    backup_file (eb->xml_file, "xml");

    if (ephy_bookmarks_import_rdf (eb, eb->rdf_file) == FALSE) {
      backup_file (eb->rdf_file, "rdf");

      eb->init_defaults = TRUE;
    }
  }

  if (eb->init_defaults) {
    ephy_bookmarks_init_defaults (eb);
  }

  fix_hierarchy (eb);

  g_settings_bind (EPHY_SETTINGS_LOCKDOWN,
                   EPHY_PREFS_LOCKDOWN_BOOKMARK_EDITING,
                   eb->db, "immutable",
                   G_SETTINGS_BIND_GET);

  ephy_setup_history_notifiers (eb);
}

static void
ephy_bookmarks_finalize (GObject *object)
{
  EphyBookmarks *eb = EPHY_BOOKMARKS (object);

  if (eb->save_timeout_id != 0) {
    g_source_remove (eb->save_timeout_id);
  }

  ephy_bookmarks_save (eb);

  ephy_local_bookmarks_stop (eb);

  ephy_node_unref (eb->bookmarks);
  ephy_node_unref (eb->keywords);
  ephy_node_unref (eb->notcategorized);
  ephy_node_unref (eb->smartbookmarks);

  g_object_unref (eb->db);

  g_free (eb->xml_file);
  g_free (eb->rdf_file);

  LOG ("Bookmarks finalized");

  G_OBJECT_CLASS (ephy_bookmarks_parent_class)->finalize (object);
}

EphyBookmarks *
ephy_bookmarks_new (void)
{
  return EPHY_BOOKMARKS (g_object_new (EPHY_TYPE_BOOKMARKS, NULL));
}

static void
update_has_smart_address (EphyBookmarks *bookmarks, EphyNode *bmk, const char *address)
{
  EphyNode *smart_bmks;
  gboolean smart = FALSE, with_options = FALSE;

  smart_bmks = bookmarks->smartbookmarks;

  if (address) {
    smart = strstr (address, "%s") != NULL;
    with_options = strstr (address, "%s%{") != NULL;
  }

  /* if we have a smart bookmark with width specification,
   * remove from smart bookmarks first to force an update
   * in the toolbar
   */
  if (smart && with_options) {
    if (ephy_node_has_child (smart_bmks, bmk)) {
      ephy_node_remove_child (smart_bmks, bmk);
    }
    ephy_node_add_child (smart_bmks, bmk);
  } else if (smart) {
    if (!ephy_node_has_child (smart_bmks, bmk)) {
      ephy_node_add_child (smart_bmks, bmk);
    }
  } else {
    if (ephy_node_has_child (smart_bmks, bmk)) {
      ephy_node_remove_child (smart_bmks, bmk);
    }
  }
}

EphyNode *
ephy_bookmarks_add (EphyBookmarks *eb,
                    const char    *title,
                    const char    *url)
{
  EphyNode *bm;
  WebKitFaviconDatabase *favicon_database;
  EphyEmbedShell *shell = ephy_embed_shell_get_default ();

  bm = ephy_node_new (eb->db);

  if (bm == NULL) return NULL;

  if (url == NULL) return NULL;
  ephy_node_set_property_string (bm, EPHY_NODE_BMK_PROP_LOCATION, url);

  if (title == NULL || title[0] == '\0') {
    title = _("Untitled");
  }
  ephy_node_set_property_string (bm, EPHY_NODE_BMK_PROP_TITLE, title);

  favicon_database = webkit_web_context_get_favicon_database (ephy_embed_shell_get_web_context (shell));
  if (favicon_database != NULL) {
    char *icon = webkit_favicon_database_get_favicon_uri (favicon_database, url);
    if (icon != NULL) {
      ephy_node_set_property_string
        (bm, EPHY_NODE_BMK_PROP_ICON, icon);
      g_free (icon);
    }
  }

  update_has_smart_address (eb, bm, url);
  update_bookmark_keywords (eb, bm);

  ephy_node_add_child (eb->bookmarks, bm);
  ephy_node_add_child (eb->notcategorized, bm);

  ephy_bookmarks_save_delayed (eb, 0);

  return bm;
}

void
ephy_bookmarks_set_address (EphyBookmarks *eb,
                            EphyNode      *bookmark,
                            const char    *address)
{
  ephy_node_set_property_string (bookmark, EPHY_NODE_BMK_PROP_LOCATION,
                                 address);

  update_has_smart_address (eb, bookmark, address);
}

EphyNode *
ephy_bookmarks_find_bookmark (EphyBookmarks *eb,
                              const char    *url)
{
  GPtrArray *children;
  guint i;

  g_return_val_if_fail (EPHY_IS_BOOKMARKS (eb), NULL);
  g_return_val_if_fail (eb->bookmarks != NULL, NULL);
  g_return_val_if_fail (url != NULL, NULL);

  children = ephy_node_get_children (eb->bookmarks);
  for (i = 0; i < children->len; i++) {
    EphyNode *kid;
    const char *location;

    kid = g_ptr_array_index (children, i);
    location = ephy_node_get_property_string
                 (kid, EPHY_NODE_BMK_PROP_LOCATION);

    if (location != NULL && strcmp (url, location) == 0) {
      return kid;
    }
  }

  return NULL;
}

static gboolean
is_similar (const char *url1, const char *url2)
{
  int i, j;

  for (i = 0; url1[i]; i++)
    if (url1[i] == '#' || url1[i] == '?')
      break;
  while (i > 0 && url1[i] == '/')
    i--;

  for (j = 0; url2[j]; j++)
    if (url2[j] == '#' || url2[j] == '?')
      break;
  while (j > 0 && url2[j] == '/')
    j--;

  if (i != j) return FALSE;
  if (strncmp (url1, url2, i) != 0) return FALSE;
  return TRUE;
}

gint
ephy_bookmarks_get_similar (EphyBookmarks *eb,
                            EphyNode      *bookmark,
                            GPtrArray     *identical,
                            GPtrArray     *similar)
{
  GPtrArray *children;
  const char *url;
  guint i, result;

  g_return_val_if_fail (EPHY_IS_BOOKMARKS (eb), -1);
  g_return_val_if_fail (eb->bookmarks != NULL, -1);
  g_return_val_if_fail (bookmark != NULL, -1);

  url = ephy_node_get_property_string
          (bookmark, EPHY_NODE_BMK_PROP_LOCATION);

  g_return_val_if_fail (url != NULL, -1);

  result = 0;

  children = ephy_node_get_children (eb->bookmarks);
  for (i = 0; i < children->len; i++) {
    EphyNode *kid;
    const char *location;

    kid = g_ptr_array_index (children, i);
    if (kid == bookmark) {
      continue;
    }

    location = ephy_node_get_property_string
                 (kid, EPHY_NODE_BMK_PROP_LOCATION);

    if (location != NULL) {
      if (identical != NULL && strcmp (url, location) == 0) {
        g_ptr_array_add (identical, kid);
        result++;
      } else if (is_similar (url, location)) {
        if (similar != NULL) {
          g_ptr_array_add (similar, kid);
        }
        result++;
      }
    }
  }

  return result;
}

void
ephy_bookmarks_set_icon (EphyBookmarks *eb,
                         const char    *url,
                         const char    *icon)
{
  EphyNode *node;

  g_return_if_fail (icon != NULL);

  node = ephy_bookmarks_find_bookmark (eb, url);
  if (node == NULL) return;

  ephy_node_set_property_string (node, EPHY_NODE_BMK_PROP_ICON, icon);
}


void
ephy_bookmarks_set_usericon (EphyBookmarks *eb,
                             const char    *url,
                             const char    *icon)
{
  EphyNode *node;

  g_return_if_fail (icon != NULL);

  node = ephy_bookmarks_find_bookmark (eb, url);
  if (node == NULL) return;

  ephy_node_set_property_string (node, EPHY_NODE_BMK_PROP_USERICON,
                                 icon);
}


/* name must end with '=' */
static char *
get_option (char       *start,
            const char *name,
            char      **optionsend)
{
  char *end, *p;

  *optionsend = start;

  if (start == NULL || start[0] != '%' || start[1] != '{') return NULL;
  start += 2;

  end = strstr (start, "}");
  if (end == NULL) return NULL;
  *optionsend = end + 1;

  start = strstr (start, name);
  if (start == NULL || start > end) return NULL;
  start += strlen (name);

  /* Find end of option, either ',' or '}' */
  end = strstr (start, ",");
  if (end == NULL || end >= *optionsend) end = *optionsend - 1;

  /* limit option length and sanity-check it */
  if (end - start > 32) return NULL;
  for (p = start; p < end; ++p) {
    if (!g_ascii_isalnum (*p)) return NULL;
  }

  return g_strndup (start, end - start);
}

char *
ephy_bookmarks_resolve_address (EphyBookmarks *eb,
                                const char    *address,
                                const char    *content)
{
  GString *result;
  char *pos, *oldpos, *arg, *escaped_arg, *encoding, *optionsend;

  g_return_val_if_fail (EPHY_IS_BOOKMARKS (eb), NULL);
  g_return_val_if_fail (address != NULL, NULL);

  if (content == NULL) content = "";

  result = g_string_new_len (NULL, strlen (content) + strlen (address));

  /* substitute %s's */
  oldpos = (char *)address;
  while ((pos = strstr (oldpos, "%s")) != NULL) {
    g_string_append_len (result, oldpos, pos - oldpos);
    pos += 2;

    encoding = get_option (pos, "encoding=", &optionsend);
    if (encoding != NULL) {
      GError *error = NULL;

      arg = g_convert (content, strlen (content), encoding,
                       "UTF-8", NULL, NULL, &error);
      if (error != NULL) {
        g_warning ("Error when converting arg to encoding '%s': %s\n",
                   encoding, error->message);
        g_error_free (error);
      } else {
        escaped_arg = g_uri_escape_string (arg, NULL, TRUE);
        g_string_append (result, escaped_arg);
        g_free (escaped_arg);
        g_free (arg);
      }

      g_free (encoding);
    } else {
      arg = g_uri_escape_string (content, NULL, TRUE);
      g_string_append (result, arg);
      g_free (arg);
    }

    oldpos = optionsend;
  }
  g_string_append (result, oldpos);

  return g_string_free (result, FALSE);
}

guint
ephy_bookmarks_get_smart_bookmark_width (EphyNode *bookmark)
{
  const char *url;
  char *option, *end, *number;
  guint width;

  url = ephy_node_get_property_string (bookmark, EPHY_NODE_BMK_PROP_LOCATION);
  if (url == NULL) return 0;

  /* this takes the first %s, but that's okay since we only support one text entry box */
  option = strstr (url, "%s%{");
  if (option == NULL) return 0;
  option += 2;

  number = get_option (option, "width=", &end);
  if (number == NULL) return 0;

  width = (guint)g_ascii_strtoull (number, NULL, 10);
  g_free (number);

  return CLAMP (width, 1, 64);
}

EphyNode *
ephy_bookmarks_add_keyword (EphyBookmarks *eb,
                            const char    *name)
{
  EphyNode *key;

  key = ephy_node_new (eb->db);

  if (key == NULL) return NULL;

  ephy_node_set_property_string (key, EPHY_NODE_KEYWORD_PROP_NAME,
                                 name);
  ephy_node_set_property_int (key, EPHY_NODE_KEYWORD_PROP_PRIORITY,
                              EPHY_NODE_NORMAL_PRIORITY);

  ephy_node_add_child (eb->keywords, key);

  return key;
}

void
ephy_bookmarks_remove_keyword (EphyBookmarks *eb,
                               EphyNode      *keyword)
{
  ephy_node_remove_child (eb->keywords, keyword);
}

char *
ephy_bookmarks_get_topic_uri (EphyBookmarks *eb,
                              EphyNode      *node)
{
  char *uri;

  if (ephy_bookmarks_get_bookmarks (eb) == node) {
    uri = g_strdup ("topic://Special/All");
  } else if (ephy_bookmarks_get_not_categorized (eb) == node) {
    uri = g_strdup ("topic://Special/NotCategorized");
  } else if (ephy_bookmarks_get_local (eb) == node) {
    /* Note: do not change to "Nearby" because of existing custom toolbars */
    uri = g_strdup ("topic://Special/Local");
  } else {
    const char *name;

    name = ephy_node_get_property_string
             (node, EPHY_NODE_KEYWORD_PROP_NAME);

    uri = g_strdup_printf ("topic://%s", name);
  }

  return uri;
}

EphyNode *
ephy_bookmarks_find_keyword (EphyBookmarks *eb,
                             const char    *name,
                             gboolean       partial_match)
{
  EphyNode *node;
  GPtrArray *children;
  guint i;
  const char *topic_name;

  g_return_val_if_fail (name != NULL, NULL);

  topic_name = name;

  if (g_utf8_strlen (name, -1) == 0) {
    LOG ("Empty name, no keyword matches.");
    return NULL;
  }

  if (strcmp (name, "topic://Special/All") == 0) {
    return ephy_bookmarks_get_bookmarks (eb);
  } else if (strcmp (name, "topic://Special/NotCategorized") == 0) {
    return ephy_bookmarks_get_not_categorized (eb);
  } else if (strcmp (name, "topic://Special/Local") == 0) {
    return ephy_bookmarks_get_local (eb);
  } else if (g_str_has_prefix (name, "topic://")) {
    topic_name += strlen ("topic://");
  }

  children = ephy_node_get_children (eb->keywords);
  node = NULL;
  for (i = 0; i < children->len; i++) {
    EphyNode *kid;
    const char *key;

    kid = g_ptr_array_index (children, i);
    key = ephy_node_get_property_string (kid, EPHY_NODE_KEYWORD_PROP_NAME);

    if ((partial_match && g_str_has_prefix (key, topic_name) > 0) ||
        (!partial_match && strcmp (key, topic_name) == 0)) {
      node = kid;
    }
  }

  return node;
}

gboolean
ephy_bookmarks_has_keyword (EphyBookmarks *eb,
                            EphyNode      *keyword,
                            EphyNode      *bookmark)
{
  return ephy_node_has_child (keyword, bookmark);
}

void
ephy_bookmarks_set_keyword (EphyBookmarks *eb,
                            EphyNode      *keyword,
                            EphyNode      *bookmark)
{
  if (ephy_node_has_child (keyword, bookmark)) return;

  ephy_node_add_child (keyword, bookmark);

  if (ephy_node_has_child (eb->notcategorized, bookmark)) {
    LOG ("Remove from categorized bookmarks");
    ephy_node_remove_child
      (eb->notcategorized, bookmark);
  }

  update_bookmark_keywords (eb, bookmark);

  g_signal_emit (G_OBJECT (eb), ephy_bookmarks_signals[TREE_CHANGED], 0);
}

void
ephy_bookmarks_unset_keyword (EphyBookmarks *eb,
                              EphyNode      *keyword,
                              EphyNode      *bookmark)
{
  if (!ephy_node_has_child (keyword, bookmark)) return;

  ephy_node_remove_child (keyword, bookmark);

  if (!bookmark_is_categorized (eb, bookmark) &&
      !ephy_node_has_child (eb->notcategorized, bookmark)) {
    LOG ("Add to not categorized bookmarks");
    ephy_node_add_child
      (eb->notcategorized, bookmark);
  }

  update_bookmark_keywords (eb, bookmark);

  g_signal_emit (G_OBJECT (eb), ephy_bookmarks_signals[TREE_CHANGED], 0);
}

/**
 * ephy_bookmarks_get_smart_bookmarks:
 *
 * Return value: (transfer none):
 **/
EphyNode *
ephy_bookmarks_get_smart_bookmarks (EphyBookmarks *eb)
{
  return eb->smartbookmarks;
}

/**
 * ephy_bookmarks_get_keywords:
 *
 * Return value: (transfer none):
 **/
EphyNode *
ephy_bookmarks_get_keywords (EphyBookmarks *eb)
{
  return eb->keywords;
}

/**
 * ephy_bookmarks_get_bookmarks:
 *
 * Return value: (transfer none):
 **/
EphyNode *
ephy_bookmarks_get_bookmarks (EphyBookmarks *eb)
{
  return eb->bookmarks;
}

/**
 * ephy_bookmarks_get_local:
 *
 * Return value: (transfer none):
 **/
EphyNode *
ephy_bookmarks_get_local (EphyBookmarks *eb)
{
  return eb->local;
}

/**
 * ephy_bookmarks_get_not_categorized:
 *
 * Return value: (transfer none):
 **/
EphyNode *
ephy_bookmarks_get_not_categorized (EphyBookmarks *eb)
{
  return eb->notcategorized;
}

/**
 * ephy_bookmarks_get_from_id:
 *
 * Return value: (transfer none):
 **/
EphyNode *
ephy_bookmarks_get_from_id (EphyBookmarks *eb, long id)
{
  return ephy_node_db_get_node_from_id (eb->db, id);
}

int
ephy_bookmarks_compare_topics (gconstpointer a, gconstpointer b)
{
  EphyNode *node_a = (EphyNode *)a;
  EphyNode *node_b = (EphyNode *)b;
  const char *title1, *title2;
  int priority1, priority2;

  priority1 = ephy_node_get_property_int (node_a, EPHY_NODE_KEYWORD_PROP_PRIORITY);
  priority2 = ephy_node_get_property_int (node_b, EPHY_NODE_KEYWORD_PROP_PRIORITY);

  if (priority1 > priority2) return 1;
  if (priority1 < priority2) return -1;

  title1 = ephy_node_get_property_string (node_a, EPHY_NODE_KEYWORD_PROP_NAME);
  title2 = ephy_node_get_property_string (node_b, EPHY_NODE_KEYWORD_PROP_NAME);

  if (title1 == title2) return 0;
  if (title1 == NULL) return -1;
  if (title2 == NULL) return 1;
  return g_utf8_collate (title1, title2);
}

int
ephy_bookmarks_compare_topic_pointers (gconstpointer a, gconstpointer b)
{
  EphyNode *node_a = *(EphyNode **)a;
  EphyNode *node_b = *(EphyNode **)b;

  return ephy_bookmarks_compare_topics (node_a, node_b);
}

int
ephy_bookmarks_compare_bookmarks (gconstpointer a, gconstpointer b)
{
  EphyNode *node_a = (EphyNode *)a;
  EphyNode *node_b = (EphyNode *)b;
  const char *title1, *title2;

  title1 = ephy_node_get_property_string (node_a, EPHY_NODE_BMK_PROP_TITLE);
  title2 = ephy_node_get_property_string (node_b, EPHY_NODE_BMK_PROP_TITLE);

  if (title1 == title2) return 0;
  if (title1 == NULL) return -1;
  if (title2 == NULL) return 1;
  return g_utf8_collate (title1, title2);
}

int
ephy_bookmarks_compare_bookmark_pointers (gconstpointer a, gconstpointer b)
{
  EphyNode *node_a = *(EphyNode **)a;
  EphyNode *node_b = *(EphyNode **)b;

  return ephy_bookmarks_compare_bookmarks (node_a, node_b);
}
