/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2000-2003 Marco Pesenti Gritti
 *  Copyright © 2004 Christian Persch
 *  Copyright © 2009 Igalia S.L.
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

#ifndef EPHY_EMBED_EVENT_H
#define EPHY_EMBED_EVENT_H

#include <webkit2/webkit2.h>

G_BEGIN_DECLS

#define EPHY_TYPE_EMBED_EVENT (ephy_embed_event_get_type ())

G_DECLARE_FINAL_TYPE (EphyEmbedEvent, ephy_embed_event, EPHY, EMBED_EVENT, GObject)

EphyEmbedEvent *     ephy_embed_event_new                 (GdkEventButton      *event,
                                                           WebKitHitTestResult *hit_test_result);
guint                ephy_embed_event_get_context         (EphyEmbedEvent      *event);
guint                ephy_embed_event_get_button          (EphyEmbedEvent      *event);
guint                ephy_embed_event_get_modifier        (EphyEmbedEvent      *event);
void                 ephy_embed_event_get_coords          (EphyEmbedEvent      *event,
                                                           guint               *x,
                                                           guint               *y);
void                 ephy_embed_event_get_property        (EphyEmbedEvent      *event,
                                                           const char          *name,
                                                           GValue              *value);
gboolean             ephy_embed_event_has_property        (EphyEmbedEvent      *event,
                                                           const char          *name);
WebKitHitTestResult *ephy_embed_event_get_hit_test_result (EphyEmbedEvent      *event);

G_END_DECLS

#endif
