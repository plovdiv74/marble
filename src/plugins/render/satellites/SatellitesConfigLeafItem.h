// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESCONFIGLEAFITEM_H
#define MARBLE_SATELLITESCONFIGLEAFITEM_H

#include "SatellitesConfigAbstractItem.h"

namespace Marble {

class SatellitesConfigLeafItem : public SatellitesConfigAbstractItem
{

public:
    explicit SatellitesConfigLeafItem( const QString &name,
                                       const QString &id );
    ~SatellitesConfigLeafItem() override;

    void loadSettings(const QHash<QString, QVariant> &settings) override;

    QVariant data( int column, int role ) const override;
    bool setData(int column, int role, const QVariant& data) override;

    bool isLeaf() const override;
    SatellitesConfigAbstractItem *childAt( int row ) const override;
    int indexOf( const SatellitesConfigAbstractItem *child ) const override;
    int childrenCount() const override;

    QString id() const;
    QString url() const;

private:
    QString m_id;
    QString m_url;
    bool m_isChecked;
    bool m_isOrbitDisplayed;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGLEAFITEM_H
