/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set sw=2 ts=2 sts=2 et: */
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

#ifndef EPHY_SECURITY_LEVELS_H
#define EPHY_SECURITY_LEVELS_H

#include "glib.h"

G_BEGIN_DECLS

/* TODO: EPHY_SECURITY_LEVEL_WEAK_SECURITY should be implemented for e.g. certs
 * that use SHA1 or RSA 1024, connections that use RC4, connections that required
 * protocol version fallback, servers that did not send the safe renegotiation
 * extension, servers that picked weak DH primes, whatever else turns out to be bad
 * next year, etc. etc. */

typedef enum
{
  EPHY_SECURITY_LEVEL_TO_BE_DETERMINED,
  EPHY_SECURITY_LEVEL_NO_SECURITY,
  EPHY_SECURITY_LEVEL_UNACCEPTABLE_CERTIFICATE,
  EPHY_SECURITY_LEVEL_MIXED_CONTENT,
  EPHY_SECURITY_LEVEL_STRONG_SECURITY,
  EPHY_SECURITY_LEVEL_LOCAL_PAGE
} EphySecurityLevel;

const char *ephy_security_level_to_icon_name (EphySecurityLevel level);

G_END_DECLS

#endif
