/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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

#ifndef _EPHY_WEB_OVERVIEW_MODEL_H
#define _EPHY_WEB_OVERVIEW_MODEL_H

#include <glib-object.h>

G_BEGIN_DECLS

#define EPHY_TYPE_WEB_OVERVIEW_MODEL (ephy_web_overview_model_get_type())

G_DECLARE_FINAL_TYPE (EphyWebOverviewModel, ephy_web_overview_model, EPHY, WEB_OVERVIEW_MODEL, GObject)

EphyWebOverviewModel *ephy_web_overview_model_new               (void);
void                  ephy_web_overview_model_set_urls          (EphyWebOverviewModel *model,
                                                                 GList                *urls);
GList                *ephy_web_overview_model_get_urls          (EphyWebOverviewModel *model);
void                  ephy_web_overview_model_set_url_thumbnail (EphyWebOverviewModel *model,
                                                                 const char           *url,
                                                                 const char           *path);
const char           *ephy_web_overview_model_get_url_thumbnail (EphyWebOverviewModel *model,
                                                                 const char           *url);
void                  ephy_web_overview_model_set_url_title     (EphyWebOverviewModel *model,
                                                                 const char           *url,
                                                                 const char           *title);
void                  ephy_web_overview_model_delete_url        (EphyWebOverviewModel *model,
                                                                 const char           *url);
void                  ephy_web_overview_model_delete_host       (EphyWebOverviewModel *model,
                                                                 const char           *host);
void                  ephy_web_overview_model_clear             (EphyWebOverviewModel *model);


typedef struct _EphyWebOverviewModelItem EphyWebOverviewModelItem;
struct _EphyWebOverviewModelItem
{
  char *url;
  char *title;
};

EphyWebOverviewModelItem *ephy_web_overview_model_item_new  (const char               *url,
                                                             const char               *title);
void                      ephy_web_overview_model_item_free (EphyWebOverviewModelItem *item);

G_END_DECLS

#endif /* _EPHY_WEB_OVERVIEW_MODEL_H */
