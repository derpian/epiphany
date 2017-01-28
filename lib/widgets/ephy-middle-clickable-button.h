/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2011 Alexandre Mazari
 *  Copyright © 2011 Igalia S.L.
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

#ifndef __EPHY_MIDDLE_CLICKABLE_BUTTON_H__
#define __EPHY_MIDDLE_CLICKABLE_BUTTON_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EPHY_TYPE_MIDDLE_CLICKABLE_BUTTON  (ephy_middle_clickable_button_get_type ())

G_DECLARE_FINAL_TYPE (EphyMiddleClickableButton, ephy_middle_clickable_button, EPHY, MIDDLE_CLICKABLE_BUTTON, GtkButton)

GtkWidget *ephy_middle_clickable_button_new      (void);

G_END_DECLS

#endif
