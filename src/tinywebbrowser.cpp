#include "tinywebbrowser.h"
#include "httpfetchfile.h"
#include "katlasdirs.h"

#include <QFileInfo>
#include <QUrl>
#include <QDebug>
#include <QTextFrame>

TinyWebBrowser::TinyWebBrowser( QWidget *parent ) : QTextBrowser( parent ) {
	m_pFetchFile = new HttpFetchFile(this);
	connect( m_pFetchFile, SIGNAL( downloadDone( QString, bool ) ), this, SLOT( slotDownloadFinished( QString, bool ) ) );
	connect( m_pFetchFile, SIGNAL( statusMessage( QString ) ),
		SIGNAL( statusMessage( QString) ) );		

	QStringList searchPaths;
	searchPaths << KAtlasDirs::localDir() + "/cache/" << KAtlasDirs::systemDir() + "/cache/";
	setSearchPaths( searchPaths );
}

QVariant TinyWebBrowser::loadResource ( int type, const QUrl & name ){
/*	if ( type != QTextDocument::HtmlResource && !m_urlList.contains(name) )
	{
		qDebug() << QString("Scheduling %1 for download.").arg(QFileInfo(name.path()).fileName());
		m_pFetchFile->downloadFile( name );
	}
*/
//	if ( type == QTextDocument::HtmlResource )
//	{
//	}

	if ( type == QTextDocument::ImageResource )
// && m_urlList.contains(name))
	{
		QPixmap emptyPixmap(1,1);
		emptyPixmap.fill(Qt::transparent);
//		m_urlList.append(name);
		return emptyPixmap;
	}

//	m_urlList.append(name);
	return QTextBrowser::loadResource( type, name );
}

void TinyWebBrowser::setSource( QUrl url )
{
	m_source = QFileInfo(url.path()).fileName();
	m_pFetchFile->downloadFile( url );
}

void TinyWebBrowser::slotDownloadFinished( QString filename, bool ){
	if ( filename == m_source )
	{
		QTextBrowser::setSource( filename );
		QTextFrameFormat format = document()->rootFrame()->frameFormat();
		format.setMargin(12);
		document()->rootFrame()->setFrameFormat( format );
	}
	else
	{
		qDebug("Reload");
//		reload();
//		m_urlList.removeAll();
	}
}
#include "tinywebbrowser.moc"
