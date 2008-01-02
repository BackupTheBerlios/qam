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

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <QObject>
#include <QString>
#include <QHttp>
#include <QTimer>

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QMenu>

#include "connectionentry.h"
#include "viewmonitor.h"
#include "viewsetmonitor.h"
#include "viewconfigure.h"
#include "footer.h"

typedef unsigned int uint;

class CVersion
{
  public:
	static uint major() {return m_major;};
	static uint minor() {return m_minor;};
	static uint revision() {return m_revision;};
	static void version(uint& major, uint& minor, uint& revision) { major = m_major; minor = m_minor; revision = m_revision; };
  protected:
	static const uint m_major = 1, m_minor = 2, m_revision = 0;
  private:
	CVersion();
	~CVersion();
};

class CMonitor : public QFrame
{
	Q_OBJECT

  public:

	CMonitor(QWidget* parent = 0, Qt::WFlags = 0);
	~CMonitor();

  protected:
	void closeEvent(QCloseEvent *event);
	void mouseMoveEvent(QMouseEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent* e);
	void enterEvent(QEvent*) { setCursor(Qt::OpenHandCursor); };
	void leaveEvent(QEvent*) { unsetCursor(); };

  public slots:
	void onValuesChanged(QStringList& affected, void* causer);
	void onSysTrayMenuShowClicked();
	void onSysTrayMenuHideClicked();
	void onSysTrayMenuCloseClicked();
	void onSysTrayIconActivated(QSystemTrayIcon::ActivationReason);

  private slots:
	void onMonitorSettingsApplied();
	void onConfigureSettingsApplied();

	void onCurrentTabChanged(int index);

	void onResponseHeaderReceived(const QHttpResponseHeader &responseHeader);
	void onHttpRequestFinished(int requestId, bool error);
	void onTimer();

  private:
	class CTabWidget : public QTabWidget
	{
	  protected:
		void enterEvent(QEvent*) { setCursor(Qt::ArrowCursor); };
		void leaveEvent(QEvent*) { setCursor(Qt::ArrowCursor); };
	};

	struct _PageIndex {
		int monitor;
		int setMonitor;
		int configure;
		int close;
	};
	typedef struct _PageIndex PageIndex;

	struct _SystemTray {
		QAction* showAction;
		QAction* hideAction;
		QAction* closeAction;
		QMenu* trayMenu;

		QSystemTrayIcon* trayIcon;
	};
	typedef struct _SystemTray SystemTray;

	QVBoxLayout m_topLayout;
	CTabWidget	m_tabWidget;
		PageIndex m_pageIndex;
		CViewMonitor m_pageMonitor;
		CViewSetMonitor m_pageSetMonitor;
		CViewConfigure m_pageConfigure;
		QWidget m_pageClose;
	CFooter m_footer;

	SystemTray m_systemTray;

	int m_nUpdateInterval; // in seconds
	QTimer m_timer;

	QString m_location;
	QString m_station;
	QString m_eta;

	QHttp m_Http;
	int m_nUpgradeRequestId;
	bool m_useProxy;
	QString m_proxyHost;
	uint m_proxyPort;
	QString m_proxyUser;
	QString m_proxyPassword;

	QPoint m_pointMouseToWindowDiff;
	QCursor m_preMoveCursor;

	void startRetrievingConnectionData();
	bool getNewConnections();

	void onUpgradeRequestFinished();

	void loadSettings();
	void writeSettings();

	void setupSystray();
};

#endif // #ifndef _MONITOR_H_

