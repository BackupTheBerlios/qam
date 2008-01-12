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

#include "monitor.h"

int main(int argc, char **argv) {
	QCoreApplication::setOrganizationName("nichtcool");
	QCoreApplication::setOrganizationDomain("nichtcool.de");
	QCoreApplication::setApplicationName("qam");

	QApplication app(argc, argv);

	Qt::WindowFlags flags = 0;
	flags |= Qt::Tool;
	//flags |= Qt::SplashScreen;
	flags |= Qt::WindowStaysOnTopHint;
	flags |= Qt::FramelessWindowHint;
	//flags |= Qt::WindowMinimizeButtonHint;
	//flags &= ~Qt::WindowContextHelpButtonHint;
	//flags &= ~Qt::WindowMaximizeButtonHint;
	//flags |= Qt::WindowTitleHint;

	CMonitor mon(0, flags);
	mon.setAttribute(Qt::WA_QuitOnClose);
	mon.setWindowTitle("qam");
	mon.show();

	return app.exec();
}


