/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
    SPDX-FileCopyrightText: 2010 Thibaut Gridel <tgridel@free.fr>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
    SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#ifndef MARBLE_VECTORTILELAYER_H
#define MARBLE_VECTORTILELAYER_H

#include "LayerInterface.h"
#include <QObject>

#include "MarbleGlobal.h"

namespace Marble
{

class GeoPainter;
class GeoDataDocument;
class GeoSceneGroup;
class GeoSceneVectorTileDataset;
class GeoDataTreeModel;
class PluginManager;
class HttpDownloadManager;
class ViewportParams;
class TileId;

class VectorTileLayer : public QObject, public LayerInterface
{
    Q_OBJECT

public:
    VectorTileLayer(HttpDownloadManager *downloadManager,
                    const PluginManager *pluginManager,
                    GeoDataTreeModel *treeModel);

    ~VectorTileLayer() override;

    QStringList renderPosition() const override;

    RenderState renderState() const override;

    int tileZoomLevel() const;

    QString runtimeTrace() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport,
                const QString &renderPos = QLatin1String("NONE"),
                GeoSceneLayer *layer = nullptr) override;

    void reload();

Q_SIGNALS:
    void tileLevelChanged(int tileLevel);

public Q_SLOTS:
    void setMapTheme(const QVector<const GeoSceneVectorTileDataset *> &textures, const GeoSceneGroup *textureLayerSettings);

    void reset();

private:
    Q_PRIVATE_SLOT(d, void updateLayerSettings())
    Q_PRIVATE_SLOT(d, void updateTile(const TileId &tileId, GeoDataDocument* document))


private:
    class Private;
    Private *const d;

};

}

#endif // MARBLE_VECTORTILELAYER_H
