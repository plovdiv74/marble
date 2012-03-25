//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
// Copyright 2008      Inge Wallin    <inge@lysator.liu.se>
//

// Self
#include "RenderPlugin.h"

// Marble
#include "DialogConfigurationInterface.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"

// Qt
#include <QtGui/QAction>
#include <QtGui/QStandardItem>


namespace Marble
{

class RenderPluginPrivate
{
  public:
    RenderPluginPrivate( const MarbleModel *marbleModel )
        : m_marbleModel( marbleModel ),
          m_action(0),
          m_item(0),
          m_enabled(true),
          m_visible(true)
    {
    }

    ~RenderPluginPrivate()
    {
    }

    // const: RenderPlugins should only read the model, not modify it
    const MarbleModel  *const m_marbleModel;
    QAction            *m_action;
    QStandardItem      *m_item;

    bool                m_enabled;
    bool                m_visible;
};


RenderPlugin::RenderPlugin( const MarbleModel *marbleModel )
    : d( new RenderPluginPrivate( marbleModel ) )
{
    d->m_action = new QAction( this );
    connect( d->m_action, SIGNAL( toggled( bool ) ),
	     this,        SLOT( setVisible( bool ) ) );

    d->m_item = new QStandardItem();
}

RenderPlugin::~RenderPlugin()
{
    delete d->m_item;
    delete d;

}

const MarbleModel* RenderPlugin::marbleModel() const
{
    return d->m_marbleModel;
}

QAction* RenderPlugin::action() const
{
    d->m_action->setCheckable( true );
    d->m_action->setChecked( visible() );
    d->m_action->setIcon( icon() );
    d->m_action->setText( guiString() );
    d->m_action->setToolTip( description() );
    return d->m_action;
}

QList<QActionGroup*>* RenderPlugin::actionGroups() const
{
    return 0;
}

QList<QActionGroup*>* RenderPlugin::toolbarActionGroups() const
{
    return 0;
}

QStandardItem* RenderPlugin::item()
{
    d->m_item->setIcon( icon() );
    d->m_item->setText( name() );
    d->m_item->setEditable( false );
    d->m_item->setCheckable( true );
    d->m_item->setCheckState( enabled() ?  Qt::Checked : Qt::Unchecked  );
    d->m_item->setToolTip( description() );
    d->m_item->setFlags( d->m_item->flags() & ~Qt::ItemIsSelectable );

    // Custom data
    d->m_item->setData( nameId(), RenderPlugin::NameId );
    d->m_item->setData( (bool) qobject_cast<DialogConfigurationInterface *>( this ), RenderPlugin::ConfigurationDialogAvailable );
    d->m_item->setData( backendTypes(), RenderPlugin::BackendTypes );

    return d->m_item;
}

void RenderPlugin::applyItemState()
{
    setEnabled( d->m_item->checkState() == Qt::Checked );
}

void RenderPlugin::retrieveItemState()
{
    d->m_item->setCheckState( enabled() ?  Qt::Checked : Qt::Unchecked  );
}

void RenderPlugin::setEnabled( bool enabled )
{
    if ( enabled == d->m_enabled )
        return;

    d->m_enabled = enabled;
    d->m_action->setVisible( enabled );

    d->m_item->setCheckState( enabled ?  Qt::Checked : Qt::Unchecked  );

    emit enabledChanged( enabled );
}

void RenderPlugin::setVisible( bool visible )
{
    if ( visible == d->m_visible )
        return;

    d->m_visible = visible;
    d->m_action->setChecked( visible );

    emit visibilityChanged( nameId(), visible );
}

bool RenderPlugin::enabled() const
{
    return d->m_enabled;
}

bool RenderPlugin::visible() const
{
    return d->m_visible;
}

QHash<QString,QVariant> RenderPlugin::settings() const
{
    return QHash<QString,QVariant>();
}

void RenderPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    Q_UNUSED( settings );
}

RenderPlugin::RenderType RenderPlugin::renderType() const
{
    return Unknown;
}

bool RenderPlugin::eventFilter( QObject *, QEvent * )
{
    return false;
}

void RenderPlugin::restoreDefaultSettings()
{
    setSettings( QHash<QString,QVariant>() );
}

} // namespace Marble

#include "RenderPlugin.moc"
