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

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <QSettings>

class COptions : public QSettings
{
	Q_OBJECT

  public:
	static COptions & getInstance();

	void adviseOfChanges(QStringList& changedGroups, void* causer);

  signals:
	void valuesChanged(QStringList& affectedGroups, void* causer);

  private:
	COptions() {};
};

#endif // #ifndef _OPTIONS_H_

