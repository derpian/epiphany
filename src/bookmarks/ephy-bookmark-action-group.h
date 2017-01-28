/*
 *  Copyright © 2005 Peter Harvey
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

#ifndef EPHY_BOOKMARK_ACTION_GROUP_H
#define EPHY_BOOKMARK_ACTION_GROUP_H

#include "ephy-link-action.h"
#include "ephy-node.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkActionGroup * ephy_bookmark_group_new       (EphyNode *node);

G_END_DECLS

#endif
