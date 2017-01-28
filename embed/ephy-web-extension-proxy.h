/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2014 Igalia S.L.
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

#ifndef EPHY_WEB_EXTENSION_PROXY_H
#define EPHY_WEB_EXTENSION_PROXY_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define EPHY_TYPE_WEB_EXTENSION_PROXY (ephy_web_extension_proxy_get_type ())

G_DECLARE_FINAL_TYPE (EphyWebExtensionProxy, ephy_web_extension_proxy, EPHY, WEB_EXTENSION_PROXY, GObject)

EphyWebExtensionProxy *ephy_web_extension_proxy_new                                       (GDBusConnection       *connection);
void                   ephy_web_extension_proxy_form_auth_data_save_confirmation_response (EphyWebExtensionProxy *web_extension,
                                                                                           guint                  request_id,
                                                                                           gboolean               response);
void                   ephy_web_extension_proxy_web_page_has_modified_forms               (EphyWebExtensionProxy *web_extension,
                                                                                           guint64                page_id,
                                                                                           GCancellable          *cancellable,
                                                                                           GAsyncReadyCallback    callback,
                                                                                           gpointer               user_data);
gboolean               ephy_web_extension_proxy_web_page_has_modified_forms_finish        (EphyWebExtensionProxy *web_extension,
                                                                                           GAsyncResult          *result,
                                                                                           GError               **error);
void                   ephy_web_extension_proxy_get_best_web_app_icon                     (EphyWebExtensionProxy *web_extension,
                                                                                           guint64                page_id,
                                                                                           const char            *base_uri,
                                                                                           GCancellable          *cancellable,
                                                                                           GAsyncReadyCallback    callback,
                                                                                           gpointer               user_data);
gboolean               ephy_web_extension_proxy_get_best_web_app_icon_finish              (EphyWebExtensionProxy *web_extension,
                                                                                           GAsyncResult          *result,
                                                                                           char                 **icon_uri,
                                                                                           char                 **icon_color,
                                                                                           GError               **error);
void                   ephy_web_extension_proxy_get_web_app_title                         (EphyWebExtensionProxy *web_extension,
                                                                                           guint64                page_id,
                                                                                           GCancellable          *cancellable,
                                                                                           GAsyncReadyCallback    callback,
                                                                                           gpointer               user_data);
char                  *ephy_web_extension_proxy_get_web_app_title_finish                  (EphyWebExtensionProxy *web_extension,
                                                                                           GAsyncResult          *result,
                                                                                           GError               **error);
void                   ephy_web_extension_proxy_history_set_urls                          (EphyWebExtensionProxy *web_extension,
                                                                                           GList                 *urls);
void                   ephy_web_extension_proxy_history_set_url_thumbnail                 (EphyWebExtensionProxy *web_extension,
                                                                                           const char            *url,
                                                                                           const char            *path);
void                   ephy_web_extension_proxy_history_set_url_title                     (EphyWebExtensionProxy *web_extension,
                                                                                           const char            *url,
                                                                                           const char            *title);
void                   ephy_web_extension_proxy_history_delete_url                        (EphyWebExtensionProxy *web_extension,
                                                                                           const char            *url);
void                   ephy_web_extension_proxy_history_delete_host                       (EphyWebExtensionProxy *web_extension,
                                                                                           const char            *host);
void                   ephy_web_extension_proxy_history_clear                             (EphyWebExtensionProxy *web_extension);

G_END_DECLS

#endif /* !EPHY_WEB_EXTENSION_PROXY_H */
