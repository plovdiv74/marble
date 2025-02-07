// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef TILEQUEUE_H
#define TILEQUEUE_H

#include "TileId.h"

#include <QSharedMemory>

namespace Marble {

QDataStream& operator<<(QDataStream&, const TileId& tile);
QDataStream& operator>>(QDataStream&, TileId& tile);

class TileQueue
{

public:
    TileQueue();
    ~TileQueue();

    bool read(QSet<TileId> &tileQueue);
    bool write(const QSet<TileId> &tileQueue);

private:
    void release();

    Q_DISABLE_COPY(TileQueue)

    QSharedMemory m_sharedMemory;
};

}

#endif
