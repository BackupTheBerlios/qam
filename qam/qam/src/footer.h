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

#ifndef _FOOTER_H_
#define _FOOTER_H_

#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>

#include "resizer.h"

class CFooter : public QWidget
{
	Q_OBJECT

  public:
	CFooter(QWidget* toResize, QWidget* parent = 0, Qt::WFlags f = 0);

	void closeEvent(QCloseEvent *event);

  private:
	enum ResizeDirection { Left, Middle, Right };

	class CHBoxLayout : public QHBoxLayout
	{
	  public:
		CHBoxLayout(QWidget* parent) : QHBoxLayout(parent) {};
		QSize minimumSize() const {return QSize(100, 10);};
		QSize maximumSize() const {return QSize(100, 10);};
	  protected:
		void mouseMoveEvent(QMouseEvent* e) { e->ignore(); };
		void mousePressEvent(QMouseEvent* e) { e->ignore(); };
		void mouseReleaseEvent(QMouseEvent* e) { e->ignore(); };
	};

	CHBoxLayout m_layoutTop;
	CResizer m_resizerLeft;
	CResizer m_resizerMiddle;
	CResizer m_resizerRight;

	QWidget* m_widgetToResize;
};

#endif // #ifndef _FOOTER_H_

