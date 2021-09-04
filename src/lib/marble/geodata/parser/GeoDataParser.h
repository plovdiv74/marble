/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MARBLE_GEODATAPARSER_H
#define MARBLE_GEODATAPARSER_H

#include "geodata_export.h"
#include "GeoParser.h"

namespace Marble
{

class GeoDocument;
class GeoDataDocument;

enum GeoDataSourceType {
    GeoData_UNKNOWN = -1,
    GeoData_KML    = 1,
    GeoData_GeoRSS = 2
};

class GEODATA_EXPORT GeoDataParser : public GeoParser
{
public:
    explicit GeoDataParser(GeoDataSourceType source);
    ~GeoDataParser() override;

private:
    bool isValidElement(const QString& tagName) const override;
    bool isValidRootElement() override;

    GeoDocument* createDocument() const override;
};

// Global helper function for the tag handlers
GEODATA_EXPORT GeoDataDocument* geoDataDoc(GeoParser& parser);

}

#endif
