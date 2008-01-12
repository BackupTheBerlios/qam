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

#include "footer.h"

#ifdef _DEBUG
#include <iostream>
#endif

CFooter::CFooter(QWidget* toResize, QWidget* parent, Qt::WFlags f) : 
	QWidget(parent, f),
	
	m_layoutTop(this),

	m_resizerLeft(toResize, CResizer::Left),
	m_resizerMiddle(toResize, CResizer::Middle),
	m_resizerRight(toResize, CResizer::Right),

	m_widgetToResize(toResize)
{
	setMaximumSize(QWIDGETSIZE_MAX, 6);
	setMinimumSize(120, 6);

	m_layoutTop.setMargin(0);
	m_layoutTop.setSpacing(0);

	m_layoutTop.addWidget(&m_resizerLeft);
	m_layoutTop.addWidget(&m_resizerMiddle);
	m_layoutTop.addWidget(&m_resizerRight);
	m_layoutTop.setStretchFactor(&m_resizerLeft, 10);
	m_layoutTop.setStretchFactor(&m_resizerMiddle, 45);
	m_layoutTop.setStretchFactor(&m_resizerRight, 10);
}

void CFooter::closeEvent(QCloseEvent*)
{
	
}


