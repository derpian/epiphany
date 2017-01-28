/*
 *  Copyright © 2007 Xan Lopez
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

#ifndef EPHY_EMBED_CONTAINER_H
#define EPHY_EMBED_CONTAINER_H

#include "ephy-embed.h"
#include "ephy-web-view.h"

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define EPHY_TYPE_EMBED_CONTAINER (ephy_embed_container_get_type ())

G_DECLARE_INTERFACE (EphyEmbedContainer, ephy_embed_container, EPHY, EMBED_CONTAINER, GObject)

struct _EphyEmbedContainerInterface
{
  GTypeInterface parent_iface;

  gint (* add_child)               (EphyEmbedContainer *container,
                                    EphyEmbed *child,
                                    gint position,
                                    gboolean set_active);

  void (* set_active_child)        (EphyEmbedContainer *container,
                                    EphyEmbed *child);

  void (* remove_child)            (EphyEmbedContainer *container,
                                    EphyEmbed *child);

  EphyEmbed * (* get_active_child) (EphyEmbedContainer *container);

  GList * (* get_children)         (EphyEmbedContainer *container);

  gboolean (* get_is_popup)        (EphyEmbedContainer *container);
};

gint              ephy_embed_container_add_child        (EphyEmbedContainer *container,
                                                         EphyEmbed          *child,
                                                         gint                position,
                                                         gboolean            set_active);
void              ephy_embed_container_set_active_child (EphyEmbedContainer *container,
                                                         EphyEmbed          *child);
void              ephy_embed_container_remove_child     (EphyEmbedContainer *container,
                                                         EphyEmbed          *child);
EphyEmbed *       ephy_embed_container_get_active_child (EphyEmbedContainer *container);
GList *           ephy_embed_container_get_children     (EphyEmbedContainer *container);
gboolean          ephy_embed_container_get_is_popup     (EphyEmbedContainer *container);

G_END_DECLS

#endif
