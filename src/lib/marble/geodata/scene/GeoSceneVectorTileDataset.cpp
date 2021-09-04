/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "GeoSceneVectorTileDataset.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneVectorTileDataset::GeoSceneVectorTileDataset( const QString& name )
    : GeoSceneTileDataset( name ){
}

const char* GeoSceneVectorTileDataset::nodeType() const
{
    return GeoSceneTypes::GeoSceneVectorTileType;
}

}

