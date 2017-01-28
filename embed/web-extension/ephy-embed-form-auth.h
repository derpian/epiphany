/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2013 Igalia S.L.
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

#ifndef EPHY_EMBED_FORM_AUTH_H
#define EPHY_EMBED_FORM_AUTH_H

#include <glib-object.h>
#include <libsoup/soup.h>
#include <webkit2/webkit-web-extension.h>

G_BEGIN_DECLS

#define EPHY_TYPE_EMBED_FORM_AUTH (ephy_embed_form_auth_get_type ())

G_DECLARE_FINAL_TYPE (EphyEmbedFormAuth, ephy_embed_form_auth, EPHY, EMBED_FORM_AUTH, GObject)

EphyEmbedFormAuth *ephy_embed_form_auth_new               (WebKitWebPage     *web_page,
                                                           WebKitDOMNode     *username_node,
                                                           WebKitDOMNode     *password_node,
                                                           const char        *username);
WebKitDOMNode     *ephy_embed_form_auth_get_username_node (EphyEmbedFormAuth *form_auth);
WebKitDOMNode     *ephy_embed_form_auth_get_password_node (EphyEmbedFormAuth *form_auth);
SoupURI           *ephy_embed_form_auth_get_uri           (EphyEmbedFormAuth *form_auth);
guint64            ephy_embed_form_auth_get_page_id       (EphyEmbedFormAuth *form_auth);
const char        *ephy_embed_form_auth_get_username      (EphyEmbedFormAuth *form_auth);
WebKitDOMDocument *ephy_embed_form_auth_get_owner_document(EphyEmbedFormAuth *form_auth);

G_END_DECLS

#endif /* EPHY_EMBED_FORM_AUTH_H */
