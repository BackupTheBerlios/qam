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

#ifndef _VIEWMONITOR_H_
#define _VIEWMONITOR_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QList>
#include <QCloseEvent>
#include <QCursor>

#include "options.h"
#include "connectionentry.h"

class CViewMonitor : public QWidget 
{
	Q_OBJECT

  public:
	CViewMonitor(QWidget* parent = 0, Qt::WFlags f = 0);
	~CViewMonitor();

	void Update(QList<ConnectionEntry> & connections);

	void closeEvent(QCloseEvent *event);

  protected:
	void enterEvent(QEvent*) { setCursor(Qt::OpenHandCursor); };
	void leaveEvent(QEvent*) { unsetCursor(); };
	
	void mouseMoveEvent(QMouseEvent* e) { e->ignore(); };
	void mousePressEvent(QMouseEvent* e) { e->ignore(); };
	void mouseReleaseEvent(QMouseEvent* e) { e->ignore(); };
	void mouseDoubleClickEvent(QMouseEvent* e) {e->ignore(); };

  public slots:
	void onValuesChanged(QStringList& affected, void* causer);

  private:
	// helper classes to propagate mouse-events upward
	class CTreeWidget : public QTreeWidget
	{
	  protected:
		void mouseDoubleClickEvent(QMouseEvent* e) {e->ignore(); };
		void mouseMoveEvent(QMouseEvent* e) { e->ignore(); };
		void mousePressEvent(QMouseEvent* e) { m_preMoveCursor = cursor(); setCursor(Qt::ClosedHandCursor); e->ignore(); };
		void mouseReleaseEvent(QMouseEvent* e) { setCursor(m_preMoveCursor); e->ignore(); };
	  private:
		QCursor m_preMoveCursor;
	};
	class CTreeWidgetHeaderItem : public QTreeWidgetItem
	{
	   protected:
		void mouseMoveEvent(QMouseEvent* e) { e->ignore(); };
		void mousePressEvent(QMouseEvent* e) { e->ignore(); };
		void mouseReleaseEvent(QMouseEvent* e) { e->ignore(); };
	};

	struct _ColumnIndices {
		int ident;
		int direction;
		int eta;

		_ColumnIndices() : ident(0), direction(1), eta(2) {};
		_ColumnIndices(_ColumnIndices& c) : ident(c.ident), direction(c.direction), eta(c.eta) {};
		_ColumnIndices& operator=(_ColumnIndices& c)
		{
			ident = c.ident;
			direction = c.direction;
			eta = c.eta;
			return *this;
		};
	};
	typedef struct _ColumnIndices ColumnIndices;

	QVBoxLayout m_topLayout;
	CTreeWidget m_treeWidget;
	CTreeWidgetHeaderItem m_treeWidgetHeader;
	ColumnIndices m_columnIndices;

	void loadSettings();
	void writeSettings();
};

#endif // #ifndef _VIEWMONITOR_H_

