/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_KML_KMLCOLORTAGHANDLER_H
#define MARBLE_KML_KMLCOLORTAGHANDLER_H

#include "GeoTagHandler.h"

#include <QColor>

namespace Marble
{
namespace kml
{

class KmlcolorTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;

    static QColor parseColor( const QString &colorString );
};

}
}

#endif
