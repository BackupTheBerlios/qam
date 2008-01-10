/******************************************************************************
 * 	qam - a qt 'abfahrtsmonitor
 *	Copyright (C) 2007  gf@nichtcool.de
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; version 2
 *	of the License.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *	USA.
 *****************************************************************************/

#include "monitor.h"

#include <QMessageBox>
#include <QByteArray>
#include <QTextStream>
#include <QApplication>
#include <QUrl>

// QNetworkProxy was only added in qt 4.1
#if QT_VERSION >= 0x040100
	#include <QNetworkProxy>
#endif

#include "options.h"

//#ifdef _DEBUG
	#include <iostream>
	#include <QImageReader>
//#endif


CMonitor::CMonitor(QWidget* parent, Qt::WFlags f) :	
	//QWidget(parent, f),
	QFrame(parent, f),
	m_topLayout(this),

	m_Http(this),
	m_nUpgradeRequestId(0),
	m_nUpdateInterval(10),
	m_useProxy(false),
	m_proxyHost(),
	m_proxyPort(80),
	m_proxyUser(),
	m_proxyPassword(),
	
	m_location("Dresden"),
	m_station("Albertplatz"),
	m_eta("3"),
	m_pointMouseToWindowDiff(0, 0),
	m_footer(this)
{
	// init widgets

	setWindowIcon(QIcon(":/pic/station_256x256.xpm"));

	setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	setLineWidth(1);
	setMidLineWidth(0);
	
	m_topLayout.setMargin(0);
	m_topLayout.setSpacing(0);
	m_topLayout.addWidget(&m_tabWidget);
	m_topLayout.addWidget(&m_footer);

	// set tab-pages

	m_tabWidget.setTabPosition(QTabWidget::South);
	m_tabWidget.setUsesScrollButtons(false);
	m_pageIndex.monitor = m_tabWidget.addTab(&m_pageMonitor, "");
		m_tabWidget.setTabIcon(m_pageIndex.monitor, QIcon(":/pic/station_256x256.xpm"));
		m_tabWidget.setTabToolTip(m_pageIndex.setMonitor, "Abfahrtsanzeige");
	m_pageIndex.setMonitor = m_tabWidget.addTab(&m_pageSetMonitor, "");
		m_tabWidget.setTabIcon(m_pageIndex.setMonitor, QIcon(":/pic/changestation_256x256.xpm"));
		m_tabWidget.setTabToolTip(m_pageIndex.setMonitor, "Anzeige ändern");
	m_pageIndex.configure = m_tabWidget.addTab(&m_pageConfigure, "");
		m_tabWidget.setTabToolTip(m_pageIndex.configure, "Einstellungen");
		m_tabWidget.setTabIcon(m_pageIndex.configure, QIcon(":/pic/config_256x256.xpm"));
	m_pageIndex.close = m_tabWidget.addTab(&m_pageClose, "");
		m_tabWidget.setTabIcon(m_pageIndex.close, QIcon(":/pic/close_256x256.xpm"));
		m_tabWidget.setTabToolTip(m_pageIndex.close, "Schließen");

	// set minimum size

	setMinimumSize(200, 200);

	// load main window state

	loadSettings();

	COptions& settings = COptions::getInstance();
	settings.beginGroup("MainWindow");
		resize(settings.value("size", QSize(300, 200)).toSize());
		move(settings.value("pos", QPoint(200, 200)).toPoint());
	settings.endGroup();

	// setup systray and generic context menu

	setupSystray();
	setupContextMenu();

	// set up event handlers

	connect(&m_Http, SIGNAL(requestFinished(int, bool)),
				this, SLOT(onHttpRequestFinished(int, bool)));
	connect(&m_Http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
                this, SLOT(onResponseHeaderReceived(const QHttpResponseHeader &)));
	connect(&COptions::getInstance(), SIGNAL(valuesChanged(QStringList&, void*)),
				this, SLOT(onValuesChanged(QStringList&, void*)));
	connect(&m_tabWidget, SIGNAL(currentChanged(int)),
				this, SLOT(onCurrentTabChanged(int)));
	connect(&m_pageSetMonitor, SIGNAL(settingsApplied()),
				this, SLOT(onMonitorSettingsApplied()));
	connect(&m_pageConfigure, SIGNAL(settingsApplied()),
				this, SLOT(onConfigureSettingsApplied()));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));

	// set up application wide http-proxy

#if QT_VERSION >= 0x040100
	if(m_useProxy) {
		QNetworkProxy* proxy = new QNetworkProxy();
	// QNetworkProxy::HttpProxy was only added in qt 4.3
	#if QT_VERSION >= 0x040300
		proxy->setType(QNetworkProxy::HttpProxy);
	#else
		proxy->setType(QNetworkProxy::DefaultProxy);
	#endif
		proxy->setHostName(m_proxyHost);
		proxy->setPort(m_proxyPort);
		if(m_proxyUser.length() > 0)
			proxy->setUser(m_proxyUser);
		if(m_proxyPassword.length() > 0)
			proxy->setPassword(m_proxyPassword);

		QNetworkProxy::setApplicationProxy(*proxy);
	}
#endif // #if QT_VERSION >= 0x040100

	// check for upgrades or just start retrieving connection data

	if(settings.value("StartupBehaviour/CheckForUpgrades", true).toBool()) {
		m_Http.setHost("nichtcool.de", 80);
		m_nUpgradeRequestId = m_Http.get("/dl/dev/qam/currentVersion");
	} else {
		startRetrievingConnectionData();
		m_timer.start(m_nUpdateInterval * 1000);
	}
}

CMonitor::~CMonitor()
{

}

void CMonitor::startRetrievingConnectionData()
{
	m_nUpgradeRequestId = 0;

#if QT_VERSION >= 0x040100
	if(m_useProxy) {
		QNetworkProxy* proxy = new QNetworkProxy();
	// QNetworkProxy::HttpProxy was only added in qt 4.3
	#if QT_VERSION >= 0x040300
		proxy->setType(QNetworkProxy::HttpProxy);
	#else
		proxy->setType(QNetworkProxy::DefaultProxy);
	#endif
		proxy->setHostName(m_proxyHost);
		proxy->setPort(m_proxyPort);
		if(m_proxyUser.length() > 0)
			proxy->setUser(m_proxyUser);
		if(m_proxyPassword.length() > 0)
			proxy->setPassword(m_proxyPassword);

		QNetworkProxy::setApplicationProxy(*proxy);
	}	
#endif // #if QT_VERSION >= 0x040100

	getNewConnections();
}

bool CMonitor::getNewConnections()
{
	QString qstrUrl;
	qstrUrl = "/abfahrtsmonitor/Abfahrten.do?";
	qstrUrl += "ort="; qstrUrl += QUrl::toPercentEncoding(m_location);
	qstrUrl += "&hst="; qstrUrl += QUrl::toPercentEncoding(m_station);
	qstrUrl += "&vz="; qstrUrl += QUrl::toPercentEncoding(m_eta);
	//qstrUrl += "&vm=";

	m_Http.setHost("widgets.vvo-online.de", 80);
	m_Http.get(qstrUrl);

	return true;
}

void CMonitor::onResponseHeaderReceived(const QHttpResponseHeader &responseHeader)
{
#ifdef _DEBUG
	std::cout << "CMonitor::onResponseHeaderReceived()" << "statuscode: " << responseHeader.statusCode() << "\n";
#endif

    if (responseHeader.statusCode() != 200) {
		if(m_nUpgradeRequestId != 0) {
			m_nUpgradeRequestId = 0;
			startRetrievingConnectionData();
			return;
		}

        switch(QMessageBox::critical(this, tr("Verbindungsfehler"), tr("Verbindungsfehler: %1.\nTrotzdem weiterversuchen?").arg(responseHeader.reasonPhrase()), QMessageBox::Yes, QMessageBox::No))
		{
			case QMessageBox::No:
				m_timer.stop();
				m_Http.clearPendingRequests();
				m_Http.close();
		}

        return;
    }
}

void CMonitor::onHttpRequestFinished(int /* requestId */, bool error)
{
	if(error)
		return;

	if(m_nUpgradeRequestId != 0) {
		onUpgradeRequestFinished();
		return;
	}

#ifdef _DEBUG
	std::cout << "CMonitor::onHttpRequestFinished()\n";
#endif

	try // silently skip this on any error
	{

		// extract body

		QByteArray res = m_Http.readAll();
		if(res.length() == 0)
			return;
		QTextStream stream(res);
		stream.setCodec("UTF-8");
		stream.setAutoDetectUnicode(true);

		QString qstr;
		QChar ch;

		while(!stream.atEnd()) {
			stream >> ch;
			qstr += ch;
		}

		// body empty?

		if(qstr.length() == 0)
			return;

		qstr.replace("&quot;", "\"");

		// decode html-escaped characters

		QString qstrTmp;
		int nTmp;
		int pos = qstr.indexOf("&#");
		int end;

		while(pos != -1) {
			end = pos + 1;
			
			while(qstr[end] != ';')
				end++;

			if(end < (pos + 3))
				break;

			qstrTmp = qstr.mid(pos + 2, end - pos - 2);

			nTmp = qstrTmp.toInt();
			QChar qchar(nTmp);
			qstr.replace(pos, end - pos + 1, qchar);
			
			pos = qstr.indexOf("&#");
		}

		// extract connection data

		QList<ConnectionEntry> connections;
		QStringList strList;

		qstr.remove(0, 2);
		qstr.remove(qstr.length() - 2, 2);
		strList = qstr.split("],[", QString::SkipEmptyParts);
		if(strList.isEmpty()) {
			// ... error
			return;
		}

		QString conn;
		QString connEl;
		QStringList strList2;
		ConnectionEntry entry;

		while(!strList.isEmpty()) {
			conn = strList.takeFirst();
			conn.remove(0, 1); // remove leading '"'
			conn.remove(conn.length() - 1, 1); // and trailing '"'
			strList2 = conn.split("\",\""); // split n","richtung","n into [n][richtung][n]
			
			if(strList2.size() != 3)
				continue;

			entry.ident = strList2.at(0);
			entry.direction = strList2.at(1);
			entry.eta = strList2.at(2);
			
			connections.append(entry);
		}

		m_pageMonitor.Update(connections);
	} catch (...) {
	
	}
}

void CMonitor::onUpgradeRequestFinished()
{
	QString doc = m_Http.readAll();

	if(doc.length() == 0) {
		if(!m_Http.hasPendingRequests()) {
			startRetrievingConnectionData();
			m_timer.start(m_nUpdateInterval * 1000);
		}
		return;
	}

	QString line = doc.section("\n", 0, 0);

	if(line.length() == 0) {
		if(!m_Http.hasPendingRequests())
			startRetrievingConnectionData();
		return;
	}

	uint major = 0, minor = 0, revision = 0;
	bool ok1, ok2, ok3;

	major = line.section('.', 0, 0).toUInt(&ok1);
	minor = line.section('.', 1, 1).toUInt(&ok2);
	revision = line.section('.', 2, 2).toUInt(&ok3);

	if(!ok1 || !ok2 || !ok3) {
		if(!m_Http.hasPendingRequests())
			startRetrievingConnectionData();
		return;
	}

	QString s1 = QString("%1%2%3")
								.arg(major, (int)2, (int)16, QChar('0'))
								.arg(minor, (int)2, (int)16, QChar('0'))
								.arg(revision, (int)2, (int)16, QChar('0'));
	uint netVersion = s1.toUInt(&ok1, 16);
	QString s2 = QString("%1%2%3")
								.arg(CVersion::major(), (int)2, (int)16, QChar('0'))
								.arg(CVersion::minor(), (int)2, (int)16, QChar('0'))
								.arg(CVersion::revision(), (int)2, (int)16, QChar('0'));
	uint thisVersion = s2.toUInt(&ok2, 16);

	if((!ok1 || !ok2 || (netVersion <= thisVersion)) && !m_Http.hasPendingRequests())
	{
		startRetrievingConnectionData();
		m_timer.start(m_nUpdateInterval * 1000);
		return;
	}

	QString message =	QString("Eine neue Version (%1.%2.%3) ist verfügbar...")
							.arg(major)
							.arg(minor)
							.arg(revision);

	// if info on version changes is present extract it and append it to the message

	QStringList list = doc.split("\n");
	doc = "";
	QStringList::iterator i;
	for(i = list.begin(); i != list.end(); ++i) {
		*i = i->simplified();
		if(i->length() > 1) {
			doc += *i;
		} else {
			break;
		}
	}

	QStringList versionInfo = doc.split("*");
	versionInfo.removeFirst();

	for(i = versionInfo.begin(); i != versionInfo.end(); ++i) {
		*i = i->simplified();
	}
	
	if(versionInfo.size() > 0) {
		message += "\n\nWichtige Änderungen:";
		for(i = versionInfo.begin(); i != versionInfo.end(); ++i) {
			message += "\n * ";
			message += *i;
		}
	}

	QMessageBox::information
		(this, 
		"Neue Version verfügbar", 
		message,
		QMessageBox::Ok,
		QMessageBox::NoButton);

	startRetrievingConnectionData();
	m_timer.start(m_nUpdateInterval * 1000);
}

void CMonitor::onTimer()
{
	if(m_Http.currentId() == 0) {
		getNewConnections();
	}

	if(m_timer.interval() != m_nUpdateInterval) {
		m_timer.setInterval(m_nUpdateInterval * 1000);
	}
}

void CMonitor::closeEvent(QCloseEvent *event)
{	
	m_timer.stop();

	m_systemTray.trayIcon->hide();

	writeSettings();
	// these should be only written on shutdown ;)
	COptions& settings = COptions::getInstance();
	settings.beginGroup("MainWindow");
		settings.setValue("size", size());
		settings.setValue("pos", pos());
	settings.endGroup();

	event->accept();
}

void CMonitor::loadSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("Monitor");
		m_location = settings.value("location", "Dresden").toString();
		m_station = settings.value("station", "Albertplatz").toString();
		m_eta = settings.value("eta", "3").toString();
	settings.endGroup();

	settings.beginGroup("MainWindow");
		m_nUpdateInterval = settings.value("updateInterval", 10).toInt();
		setWindowOpacity(settings.value("windowOpacity", 0.8).toDouble());
		Qt::WindowFlags flags = windowFlags();
#if 0
		if(settings.value("alwaysOnTop", true).toBool())
			flags |= Qt::WindowStaysOnTopHint;
		else
			flags ^= Qt::WindowStaysOnTopHint;
		setWindowFlags(flags);
#endif
    settings.endGroup();

	settings.beginGroup("Network");
		m_useProxy = settings.value("useProxy", false).toBool();
		m_proxyHost = settings.value("proxyHost", "").toString();
		m_proxyPort = settings.value("proxyPort", 80).toUInt();
		m_proxyUser = settings.value("proxyUser", "").toString();
		m_proxyPassword = settings.value("proxyPassword", "").toString();

		if(m_useProxy) {
			m_useProxy =	(m_proxyHost.length() > 0) && 
							(m_proxyPort <= 65535);
		}
#if QT_VERSION >= 0x040100
		if(m_useProxy) {
			QNetworkProxy* proxy = new QNetworkProxy();
		// QNetworkProxy::HttpProxy was only added in qt 4.3
		#if QT_VERSION >= 0x040300
			proxy->setType(QNetworkProxy::HttpProxy);
		#else
			proxy->setType(QNetworkProxy::DefaultProxy);
		#endif
			proxy->setHostName(m_proxyHost);
			proxy->setPort(m_proxyPort);
			if(m_proxyUser.length() > 0)
				proxy->setUser(m_proxyUser);
			if(m_proxyPassword.length() > 0)
				proxy->setPassword(m_proxyPassword);

			QNetworkProxy::setApplicationProxy(*proxy);
		}
#endif // #if QT_VERSION >= 0x040100
	settings.endGroup();
}

void CMonitor::writeSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("Monitor");
		settings.setValue("location", m_location);
		settings.setValue("station", m_station);
		settings.setValue("eta", m_eta);
	settings.endGroup();
	settings.beginGroup("MainWindow");
		settings.setValue("updateInterval", m_nUpdateInterval);
		settings.setValue("windowOpacity", windowOpacity());
#if 0
		Qt::WindowFlags flags = windowFlags();
		settings.setValue("alwaysOnTop", (flags & Qt::WindowStaysOnTopHint) ? true : false);
#endif
    settings.endGroup();

	QStringList list("MainWindow");
	list += "Monitor";
	settings.adviseOfChanges(list, (void*)this);
}

void CMonitor::onValuesChanged(QStringList& affected, void* causer)
{
	if((causer != (void*)this) && (affected.contains("MainWindow") || affected.contains("Monitor") || affected.contains("Network"))) {
		m_timer.stop();
		m_Http.close();
		m_Http.clearPendingRequests();
		
		loadSettings();

		startRetrievingConnectionData();
		m_timer.start(m_nUpdateInterval * 1000);
	}
}

void CMonitor::mousePressEvent(QMouseEvent* e)
{
	m_pointMouseToWindowDiff.setX(e->globalX() - x());
	m_pointMouseToWindowDiff.setY(e->globalY() - y());
	m_preMoveCursor = cursor();
	setCursor(Qt::ClosedHandCursor);
}

void CMonitor::mouseReleaseEvent(QMouseEvent* e)
{
	m_pointMouseToWindowDiff.setX(e->globalX() - x());
	m_pointMouseToWindowDiff.setY(e->globalY() - y());
	setCursor(m_preMoveCursor);
}

void CMonitor::mouseMoveEvent(QMouseEvent* e)
{
	move(e->globalPos() - m_pointMouseToWindowDiff);
}

void CMonitor::onCurrentTabChanged(int index)
{
	if(index == m_pageIndex.close) {
		quit();
	}
}

void CMonitor::mouseDoubleClickEvent(QMouseEvent* e)
{
	if(m_tabWidget.currentIndex() == m_pageIndex.monitor) {
		m_tabWidget.setCurrentIndex(m_pageIndex.setMonitor);
		e->accept();
	}
}

void CMonitor::onMonitorSettingsApplied()
{
	m_tabWidget.setCurrentIndex(m_pageIndex.monitor);
}

void CMonitor::onConfigureSettingsApplied()
{
	m_tabWidget.setCurrentIndex(m_pageIndex.monitor);
}

void CMonitor::setupSystray()
{
	SystemTray& s = m_systemTray;
	s.showAction = new QAction(tr("Zeigen"), this);
	s.showAction->setIcon(QIcon(":/pic/menu_maximize_32x32.xpm"));
	s.hideAction = new QAction(tr("Verstecken"), this);
	s.hideAction->setIcon(QIcon(":/pic/menu_minimize_32x32.xpm"));
	s.closeAction = new QAction(tr("Beenden"), this);
	s.closeAction->setIcon(QIcon(":/pic/menu_close_32x32.xpm"));
	s.trayMenu = new QMenu();
	s.trayMenu->addAction(s.showAction);
	s.trayMenu->addAction(s.hideAction);
	s.trayMenu->addSeparator();
	s.trayMenu->addAction(s.closeAction);

	connect(s.showAction, SIGNAL(triggered()),
			this, SLOT(onSysTrayMenuShowClicked()));
	connect(s.hideAction, SIGNAL(triggered()),
			this, SLOT(onSysTrayMenuHideClicked()));
	connect(s.closeAction, SIGNAL(triggered()),
			this, SLOT(onSysTrayMenuCloseClicked()));
	//connect(s.trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    //         this, SLOT(onSysTrayIconActivated(QSystemTrayIcon::ActivationReason)));

	s.trayIcon = new QSystemTrayIcon();
	s.trayIcon->setContextMenu(s.trayMenu);
	s.trayIcon->setIcon(QIcon(":/pic/station_256x256.xpm"));
	s.trayIcon->show();
}

void CMonitor::onSysTrayMenuShowClicked()
{
	restore();
}

void CMonitor::onSysTrayMenuHideClicked()
{
	minimize();
}

void CMonitor::onSysTrayMenuCloseClicked()
{
	quit();
}

void CMonitor::onSysTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			setVisible(!isVisible());
			break;
		case QSystemTrayIcon::MiddleClick:
			break;
		default:
			;
	}
}

void CMonitor::minimize()
{
	m_timer.stop();
	m_Http.close();
	m_Http.clearPendingRequests();
	setVisible(false);
}

void CMonitor::restore()
{
	startRetrievingConnectionData();
	m_timer.start(m_nUpdateInterval * 1000);
	setVisible(true);
}

void CMonitor::setupContextMenu()
{
	ContextMenu& c = m_contextMenu;
	c.hideAction = new QAction(tr("Verstecken"), this);
	c.hideAction->setIcon(QIcon(":/pic/menu_minimize_32x32.xpm"));
	c.closeAction = new QAction(tr("Beenden"), this);
	c.closeAction->setIcon(QIcon(":/pic/menu_close_32x32.xpm"));
	c.contextMenu = new QMenu();
	c.contextMenu->addAction(c.hideAction);
	c.contextMenu->addSeparator();
	c.contextMenu->addAction(c.closeAction);

	connect(c.hideAction, SIGNAL(triggered()),
			this, SLOT(onContextMenuHideClicked()));
	connect(c.closeAction, SIGNAL(triggered()),
			this, SLOT(onContextMenuCloseClicked()));
}

void CMonitor::contextMenuEvent(QContextMenuEvent* e)
{
	e->accept();
	m_contextMenu.contextMenu->popup(e->globalPos());
}

void CMonitor::onContextMenuHideClicked()
{
	minimize();
}

void CMonitor::onContextMenuCloseClicked()
{
	quit();
}

void CMonitor::quit()
{
	// TODO: find out how to close QApplication in a way that it send close-events around
	// begin dirty hack
	QCloseEvent* e = new QCloseEvent();
	e->setAccepted(false);
	closeEvent(e);
	e->setAccepted(false);
	m_pageMonitor.closeEvent(e);
	e->setAccepted(false);
	m_pageSetMonitor.closeEvent(e);
	e->setAccepted(false);
	m_pageConfigure.closeEvent(e);
	//end dirty hack
	close();
	qApp->quit();
}