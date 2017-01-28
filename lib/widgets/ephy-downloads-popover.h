/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2015 Igalia S.L.
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
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EPHY_DOWNLOADS_POPOVER_H
#define EPHY_DOWNLOADS_POPOVER_H

#include <gtk/gtk.h>

#include "ephy-download.h"

G_BEGIN_DECLS

#define EPHY_TYPE_DOWNLOADS_POPOVER (ephy_downloads_popover_get_type())

G_DECLARE_FINAL_TYPE (EphyDownloadsPopover, ephy_downloads_popover, EPHY, DOWNLOADS_POPOVER, GtkPopover)

GtkWidget *ephy_downloads_popover_new      (GtkWidget *relative_to);

G_END_DECLS

#endif
