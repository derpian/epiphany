/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2012 Igalia S.L.
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
#include "ephy-snapshot-service.h"

#include "ephy-favicon-helpers.h"

#ifndef GNOME_DESKTOP_USE_UNSTABLE_API
#define GNOME_DESKTOP_USE_UNSTABLE_API
#endif
#include <webkit2/webkit2.h>

struct _EphySnapshotService {
  GObject parent_instance;
};

G_DEFINE_TYPE (EphySnapshotService, ephy_snapshot_service, G_TYPE_OBJECT)

enum {
  SNAPSHOT_SAVED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

typedef enum {
  SNAPSHOT_STALE,
  SNAPSHOT_FRESH
} EphySnapshotFreshness;

typedef struct {
  char *path;
  EphySnapshotFreshness freshness;
} SnapshotPathCachedData;

static void
snapshot_path_cached_data_free (SnapshotPathCachedData *data)
{
}

static void
ephy_snapshot_service_class_init (EphySnapshotServiceClass *klass)
{
}

static void
ephy_snapshot_service_init (EphySnapshotService *self)
{
}

static GdkPixbuf *
ephy_snapshot_service_prepare_snapshot (cairo_surface_t *surface,
                                        cairo_surface_t *favicon)
{
}

typedef struct {
  EphySnapshotService *service;
  GdkPixbuf *snapshot;
  WebKitWebView *web_view;
  time_t mtime;
  char *url;
} SnapshotAsyncData;

static SnapshotAsyncData *
snapshot_async_data_new (EphySnapshotService *service,
                         GdkPixbuf           *snapshot,
                         WebKitWebView       *web_view,
                         time_t               mtime,
                         const char          *url)
{
}

static SnapshotAsyncData *
snapshot_async_data_copy (SnapshotAsyncData *data)
{
}

static void
snapshot_async_data_free (SnapshotAsyncData *data)
{
}

typedef struct {
  GHashTable *cache;
  char *url;
  SnapshotPathCachedData *data;
} CacheData;

static gboolean
idle_cache_snapshot_path (gpointer user_data)
{
  CacheData *data = (CacheData *)user_data;
  g_hash_table_insert (data->cache, data->url, data->data);
  g_hash_table_unref (data->cache);
  g_free (data);

  return G_SOURCE_REMOVE;
}

static void
cache_snapshot_data_in_idle (EphySnapshotService  *service,
                             const char           *url,
                             const char           *path,
                             EphySnapshotFreshness freshness)
{
}

static gboolean
idle_emit_snapshot_saved (gpointer user_data)
{
}

static void
save_snapshot_thread (GTask               *task,
                      EphySnapshotService *service,
                      SnapshotAsyncData   *data,
                      GCancellable        *cancellable)
{
}

static void
ephy_snapshot_service_save_snapshot_async (EphySnapshotService *service,
                                           GdkPixbuf           *snapshot,
                                           const char          *url,
                                           time_t               mtime,
                                           GCancellable        *cancellable,
                                           GAsyncReadyCallback  callback,
                                           gpointer             user_data)
{
}

static char *
ephy_snapshot_service_save_snapshot_finish (EphySnapshotService *service,
                                            GAsyncResult        *result,
                                            GError             **error)
{
}


static void
snapshot_saved (EphySnapshotService *service,
                GAsyncResult        *result,
                GTask               *task)
{
}

static void
save_snapshot (cairo_surface_t *surface,
               GTask           *task)
{
}

static void
on_snapshot_ready (WebKitWebView *web_view,
                   GAsyncResult  *result,
                   GTask         *task)
{
}

static gboolean
retrieve_snapshot_from_web_view (GTask *task)
{
  return FALSE;
}

static void
webview_destroyed_cb (GtkWidget *web_view,
                      GTask     *task)
{
}

static void
webview_load_changed_cb (WebKitWebView  *web_view,
                         WebKitLoadEvent load_event,
                         GTask          *task)
{
}

static gboolean
webview_load_failed_cb (WebKitWebView  *web_view,
                        WebKitLoadEvent load_event,
                        const char      failing_uri,
                        GError         *error,
                        GTask          *task)
{
}

static gboolean
ephy_snapshot_service_take_from_webview (GTask *task)
{
}

GQuark
ephy_snapshot_service_error_quark (void)
{
  return g_quark_from_static_string ("ephy-snapshot-service-error-quark");
}

/**
 * ephy_snapshot_service_get_default:
 *
 * Gets the default instance of #EphySnapshotService.
 *
 * Returns: a #EphySnapshotService
 **/
EphySnapshotService *
ephy_snapshot_service_get_default (void)
{
  static EphySnapshotService *service = NULL;

  if (service == NULL)
    service = g_object_new (EPHY_TYPE_SNAPSHOT_SERVICE, NULL);

  return service;
}

const char *
ephy_snapshot_service_lookup_cached_snapshot_path (EphySnapshotService *service,
                                                   const char          *url)
{
  return NULL;
}

static EphySnapshotFreshness
ephy_snapshot_service_lookup_snapshot_freshness (EphySnapshotService *service,
                                                 const char          *url)
{
  return SNAPSHOT_STALE;
}

static void
get_snapshot_path_for_url_thread (GTask               *task,
                                  EphySnapshotService *service,
                                  SnapshotAsyncData   *data,
                                  GCancellable        *cancellable)
{
}

void
ephy_snapshot_service_get_snapshot_path_for_url_async (EphySnapshotService *service,
                                                       const char          *url,
                                                       time_t               mtime,
                                                       GCancellable        *cancellable,
                                                       GAsyncReadyCallback  callback,
                                                       gpointer             user_data)
{
}

char *
ephy_snapshot_service_get_snapshot_path_for_url_finish (EphySnapshotService *service,
                                                        GAsyncResult        *result,
                                                        GError             **error)
{
}

static void
got_snapshot_path_for_url (EphySnapshotService *service,
                           GAsyncResult        *result,
                           GTask               *task)
{
}

void
ephy_snapshot_service_get_snapshot_path_async (EphySnapshotService *service,
                                               WebKitWebView       *web_view,
                                               time_t               mtime,
                                               GCancellable        *cancellable,
                                               GAsyncReadyCallback  callback,
                                               gpointer             user_data)
{
}

char *
ephy_snapshot_service_get_snapshot_path_finish (EphySnapshotService *service,
                                                GAsyncResult        *result,
                                                GError             **error)
{
}
