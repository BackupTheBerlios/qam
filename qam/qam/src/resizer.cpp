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

#include "resizer.h"

CResizer::CResizer(QWidget* toResize, ResizeDirection direction) :
	m_widgetToResize(toResize),
	m_resizeDirection(direction),
	m_lastMousePosX(-1),
	m_lastMousePosY(-1),
	m_buttonDown(false)
{
	switch(m_resizeDirection) {
		case Left:
			setCursor(Qt::SizeBDiagCursor);
			break;
		case Middle:
			setCursor(Qt::SizeVerCursor);
			break;
		case Right:
			setCursor(Qt::SizeFDiagCursor);
			break;
	}
}

void CResizer::onPressed()
{
	m_buttonDown = true;
	m_lastMousePosX = -1;
	m_lastMousePosY = -1;
	grabMouse();
}

void CResizer::onReleased()
{
	m_buttonDown = false;
	releaseMouse();
}

void CResizer::mousePressEvent(QMouseEvent* e)
{
	e->accept();
	QRect r = m_widgetToResize->geometry();
	m_mouseOffSetX = e->globalX() - r.x();
	m_mouseOffSetY = e->globalY() - r.y();
	m_mouseOffSetR = r.right() - e->globalX();
	m_mouseOffSetB = r.bottom() - e->globalY();

	QSize s = m_widgetToResize->minimumSize();
	m_minimumTargetWidth = s.width();
	m_minimumTargetHeight = s.height();

	//grabMouse();
}

void CResizer::mouseReleaseEvent(QMouseEvent* e)
{
	e->accept();
	//releaseMouse();
}

void CResizer::leaveEvent(QEvent* e)
{
	e->accept();
	releaseMouse();
}

void CResizer::mouseMoveEvent(QMouseEvent* e)
{
	e->accept();

	int mX = e->globalX();
	int mY = e->globalY();

	QRect r = m_widgetToResize->geometry();
	int wX = r.x();
	int wY = r.y();
	int wW = r.width();
	//int wH = r.height();

	int nX=0, nY=0, nW=0, nH=0;

	switch(m_resizeDirection)
	{
		case Left:
		{
			nX = mX - m_mouseOffSetX;
			nW = wW + (wX - nX);

			nY = wY;
			nH = (mY - wY) + m_mouseOffSetB;

			if(m_minimumTargetWidth > nW) {
				nW = m_minimumTargetWidth;
				nX = wX;
			}
			if(m_minimumTargetHeight > nH) {
				nH = m_minimumTargetHeight;
			}

			break;
		}
		case Middle:
			nX = wX;
			nW = wW;

			nY = wY;
			nH = (mY - wY) + m_mouseOffSetB;

			if(m_minimumTargetHeight > nH) {
				nH = m_minimumTargetHeight;
			}

			break;
		case Right:
			nX = wX;
			nW = (mX - wX) + m_mouseOffSetR;

			nY = wY;
			nH = (mY - wY) + m_mouseOffSetB;

			if(m_minimumTargetWidth > nW) {
				nW = m_minimumTargetWidth;
				nX = wX;
			}
			if(m_minimumTargetHeight > nH) {
				nH = m_minimumTargetHeight;
			}

			break;
	}

	m_widgetToResize->setGeometry(nX, nY, nW, nH);
}


