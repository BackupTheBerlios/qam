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

#include <QLabel>

#include "viewmonitor.h"

CViewMonitor::CViewMonitor(QWidget* parent, Qt::WFlags f) :
	QWidget(parent, f),
	m_topLayout(this),

	m_columnIndices()
{
	m_topLayout.setSpacing(0);
	m_topLayout.setMargin(0);
	m_topLayout.addWidget(&m_treeWidget);
	m_treeWidget.setColumnCount(3);
	m_treeWidget.setHeaderItem(&m_treeWidgetHeader);
	m_treeWidget.setRootIsDecorated(false);
	m_treeWidget.setSortingEnabled(false);
	m_treeWidget.setSelectionMode(QAbstractItemView::NoSelection);
	m_treeWidget.setAlternatingRowColors(true);

	loadSettings();

	// load this only on start up
#if 0
	COptions& settings = COptions::getInstance();
	settings.beginGroup("MonitorPage");
		m_columnIndices.ident = settings.value("column-index-ident", 0).toInt();
		m_columnIndices.direction = settings.value("column-index-direction", 1).toInt();
		m_columnIndices.eta = settings.value("column-index-eta", 2).toInt();
    settings.endGroup();
#endif

	// set up column header names
	QTreeWidgetItem* i = m_treeWidget.headerItem();
	i->setText(m_columnIndices.ident, "Linie");
	i->setText(m_columnIndices.direction, "Ziel");
	i->setText(m_columnIndices.eta, "in Min.");

	for(int i=0;i<10;i++) {
		QTreeWidgetItem* item = new QTreeWidgetItem(&m_treeWidget);
		item->setText(0, "");
		item->setText(1, "");
		item->setText(2, "");
		item->setTextAlignment(m_columnIndices.ident, Qt::AlignRight);
		m_treeWidget.addTopLevelItem(item);
	}
}

CViewMonitor::~CViewMonitor()
{
}

void CViewMonitor::Update(QList<ConnectionEntry> & connections)
{
	int i, n = connections.size();
	QString qstrItem;

	m_treeWidget.clear();

	for(i=0; i<n; i++) {
		const ConnectionEntry & c = connections.at(i);
		QTreeWidgetItem* item = new QTreeWidgetItem(&m_treeWidget);
		item->setText(m_columnIndices.ident, c.ident);
		item->setText(m_columnIndices.direction, c.direction);
		item->setText(m_columnIndices.eta, c.eta);

		item->setTextAlignment(m_columnIndices.ident, Qt::AlignRight);

		m_treeWidget.addTopLevelItem(item);
	}

	if(i<10) {
		for(;i<10;i++) {
			QTreeWidgetItem* item = new QTreeWidgetItem(&m_treeWidget);
			item->setText(0, "");
			item->setText(1, "");
			item->setText(2, "");
		}
	}

	m_treeWidget.resizeColumnToContents(0);
	m_treeWidget.resizeColumnToContents(1);
	m_treeWidget.resizeColumnToContents(2);
}

void CViewMonitor::closeEvent(QCloseEvent *event)
{	
	writeSettings();

#if 0
	COptions& settings = COptions::getInstance();
	settings.beginGroup("MonitorPage");
		settings.setValue("column-index-ident", m_columnIndices.ident);
		settings.setValue("column-index-direction", m_columnIndices.direction);
		settings.setValue("column-index-eta", m_columnIndices.eta);
    settings.endGroup();
#endif

	event->accept();
}

void CViewMonitor::loadSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("MonitorPage");
		// ...
    settings.endGroup();
}

void CViewMonitor::writeSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("MonitorPage");
		// ...
    settings.endGroup();

	QStringList list("MonitorPage");
	settings.adviseOfChanges(list, (void*)this);
}

void CViewMonitor::onValuesChanged(QStringList& affected, void* causer)
{
	if((causer != (void*)this) && affected.contains("MonitorPage")) {
		loadSettings();
	}
}


