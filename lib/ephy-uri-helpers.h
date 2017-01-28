/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2013 Bastien Nocera <hadess@hadess.net>
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

#ifndef EPHY_URI_HELPERS_H
#define EPHY_URI_HELPERS_H

#include <glib.h>

G_BEGIN_DECLS

char *ephy_remove_tracking_from_uri (const char *uri);
char *ephy_uri_decode (const char *uri);
char *ephy_uri_normalize (const char *uri);

G_END_DECLS

#endif /* EPHY_URI_HELPERS_H */

/* vim: set sw=2 ts=2 sts=2 et: */