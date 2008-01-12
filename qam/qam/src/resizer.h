/******************************************************************************
 * 	qam - a qt based abfahrtsmonitor
 *	Copyright (C) 2008  gf@nichtcool.de
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

#ifndef _RESIZER_H_
#define _RESIZER_H_

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>

class CResizer : public QPushButton
{
	Q_OBJECT

  public:
	enum ResizeDirection { Left, Middle, Right };

    CResizer(QWidget* toResize, ResizeDirection direction);

	void mouseMoveEvent(QMouseEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void leaveEvent(QEvent*);

  private slots:
	void onPressed();
	void onReleased();

  private:
	QWidget* m_widgetToResize;
	int m_minimumTargetWidth;
	int m_minimumTargetHeight;

	int m_lastMousePosX;
	int m_lastMousePosY;
	bool m_buttonDown;

	int m_mouseOffSetX;
	int m_mouseOffSetY;
	int m_mouseOffSetR;
	int m_mouseOffSetB;

	ResizeDirection m_resizeDirection;
};

#endif // #ifndef _RESIZER_H_

