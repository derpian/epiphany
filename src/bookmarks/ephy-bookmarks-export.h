/*
 *  Copyright © 2003 Marco Pesenti Gritti
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

#ifndef EPHY_BOOKMARKS_EXPORT_H
#define EPHY_BOOKMARKS_EXPORT_H

#include "ephy-bookmarks.h"

G_BEGIN_DECLS

void ephy_bookmarks_export_rdf (EphyBookmarks *bookmarks,
				const char *filename);

void ephy_bookmarks_export_mozilla (EphyBookmarks *bookmarks,
				    const char *filename);

G_END_DECLS

#endif
