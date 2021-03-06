/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
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

#include "config.h"

#include "ephy-embed-container.h"
#include "ephy-embed-type-builtins.h"

G_DEFINE_INTERFACE (EphyEmbedContainer, ephy_embed_container, G_TYPE_OBJECT);

static void
ephy_embed_container_default_init (EphyEmbedContainerInterface *iface)
{
  g_object_interface_install_property (iface,
                                       g_param_spec_boolean ("is-popup", NULL, NULL,
                                                             FALSE,
                                                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_interface_install_property (iface,
                                       g_param_spec_object ("active-child", NULL, NULL,
                                                            GTK_TYPE_WIDGET /* Can't use an interface type here */,
                                                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

/**
 * ephy_embed_container_add_child:
 * @container: an #EphyEmbedContainer
 * @child: an #EphyEmbed
 * @position: the position in @container's
 * @set_active: whether to set @embed as the active child of @container
 * after insertion
 *
 * Inserts @child into @container.
 *
 * Return value: @child's new position inside @container.
 **/
gint
ephy_embed_container_add_child (EphyEmbedContainer *container,
                                EphyEmbed          *child,
                                gint                position,
                                gboolean            set_active)
{
  EphyEmbedContainerInterface *iface;

  g_return_val_if_fail (EPHY_IS_EMBED_CONTAINER (container), -1);
  g_return_val_if_fail (EPHY_IS_EMBED (child), -1);

  iface = EPHY_EMBED_CONTAINER_GET_IFACE (container);
  return iface->add_child (container, child, position, set_active);
}

/**
 * ephy_embed_container_set_active_child:
 * @container: an #EphyEmbedContainer
 * @child: an #EphyEmbed inside @container
 *
 * Sets @child as @container's active child.
 **/
void
ephy_embed_container_set_active_child (EphyEmbedContainer *container,
                                       EphyEmbed          *child)
{
  EphyEmbedContainerInterface *iface;

  g_return_if_fail (EPHY_IS_EMBED_CONTAINER (container));
  g_return_if_fail (EPHY_IS_EMBED (child));

  iface = EPHY_EMBED_CONTAINER_GET_IFACE (container);

  iface->set_active_child (container, child);
}

/**
 * ephy_embed_container_remove_child:
 * @container: an #EphyEmbedContainer
 * @child: an #EphyEmbed
 *
 * Removes @child from @container.
 **/
void
ephy_embed_container_remove_child (EphyEmbedContainer *container,
                                   EphyEmbed          *child)
{
  EphyEmbedContainerInterface *iface;

  g_return_if_fail (EPHY_IS_EMBED_CONTAINER (container));
  g_return_if_fail (EPHY_IS_EMBED (child));

  iface = EPHY_EMBED_CONTAINER_GET_IFACE (container);

  iface->remove_child (container, child);
}

/**
 * ephy_embed_container_get_active_child:
 * @container: an #EphyEmbedContainer
 *
 * Returns @container's active #EphyEmbed.
 *
 * Return value: (transfer none): @container's active child
 **/
EphyEmbed *
ephy_embed_container_get_active_child (EphyEmbedContainer *container)
{
  EphyEmbedContainerInterface *iface;

  g_return_val_if_fail (EPHY_IS_EMBED_CONTAINER (container), NULL);

  iface = EPHY_EMBED_CONTAINER_GET_IFACE (container);
  return iface->get_active_child (container);
}

/**
 * ephy_embed_container_get_children:
 * @container: a #EphyEmbedContainer
 *
 * Returns the list of #EphyEmbed:s in the container.
 *
 * Return value: (element-type EphyEmbed) (transfer container):
 *               a newly-allocated list of #EphyEmbed:s
 */
GList *
ephy_embed_container_get_children (EphyEmbedContainer *container)
{
  EphyEmbedContainerInterface *iface;

  g_return_val_if_fail (EPHY_IS_EMBED_CONTAINER (container), NULL);

  iface = EPHY_EMBED_CONTAINER_GET_IFACE (container);
  return iface->get_children (container);
}

/**
 * ephy_embed_container_get_is_popup:
 * @container: an #EphyEmbedContainer
 *
 * Returns whether this embed container is a popup.
 *
 * Return value: %TRUE if it is a popup
 **/
gboolean
ephy_embed_container_get_is_popup (EphyEmbedContainer *container)
{
  EphyEmbedContainerInterface *iface;

  g_return_val_if_fail (EPHY_IS_EMBED_CONTAINER (container), FALSE);

  iface = EPHY_EMBED_CONTAINER_GET_IFACE (container);
  return iface->get_is_popup (container);
}


