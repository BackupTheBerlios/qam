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

#include <QFile>
#include <QTextStream>
#include <QLineEdit>
#include <QCompleter>

#include "viewsetmonitor.h"
#include "options.h"

#ifdef _DEBUG
#include <iostream>
#endif

CViewSetMonitor::CViewSetMonitor(QWidget* parent, Qt::WFlags f) :
	QWidget(parent, f),
	m_topLayout(this),

	m_groupCommonSettings("Wichtiges"),
	m_gridCommonSettings(&m_groupCommonSettings),
	m_labelLocation("Ort"),
	m_comboLocation(),
	m_labelStation("Haltestelle"),
	m_comboStation(),
	m_labelTimeOffset("Abfahrt in"),
	m_spinTimeOffset(),

	m_widgetButtons(),
	m_layoutButtons(&m_widgetButtons),
	m_buttonApply("Anwenden"),
	m_buttonReset("Zurücksetzen"),
	m_widgetButtonSpacer()
{
	m_topLayout.addWidget(&m_groupCommonSettings);
	m_topLayout.setMargin(2);
	m_gridCommonSettings.addWidget(&m_labelLocation, 0, 0, Qt::AlignVCenter | Qt::AlignLeft);
	m_gridCommonSettings.addWidget(&m_comboLocation, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
		m_comboLocation.setEditable(true);
	m_gridCommonSettings.addWidget(&m_labelStation, 1, 0, Qt::AlignVCenter | Qt::AlignLeft);
	m_gridCommonSettings.addWidget(&m_comboStation, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
		m_comboStation.setEditable(true);
		m_comboStation.setAutoCompletion(true);
		m_comboStation.setDuplicatesEnabled(false);
		m_comboStation.setMaxVisibleItems(10);
		QCompleter* c = m_comboStation.completer();
		c->setCompletionMode(QCompleter::InlineCompletion /* QCompleter::PopupCompletion */);
		c->setModelSorting(QCompleter::UnsortedModel);
		c->setCaseSensitivity(Qt::CaseInsensitive);
	m_gridCommonSettings.addWidget(&m_labelTimeOffset, 2, 0, Qt::AlignVCenter | Qt::AlignLeft);
	m_gridCommonSettings.addWidget(&m_spinTimeOffset,  2, 1, Qt::AlignVCenter | Qt::AlignLeft);
		m_spinTimeOffset.setRange(0, 60);
		m_spinTimeOffset.setSingleStep(1);
		m_spinTimeOffset.setSuffix(" min");
	m_gridCommonSettings.setColumnStretch(2, 10);
	
	// insert spacer
	m_topLayout.addStretch(100);

	// set up button group

	m_topLayout.addWidget(&m_widgetButtons);
	m_widgetButtons.setLayout(&m_layoutButtons);
	m_layoutButtons.setMargin(5);
	m_layoutButtons.setSpacing(6);
	m_layoutButtons.addWidget(&m_widgetButtonSpacer, 100);
		m_layoutButtons.setAlignment(&m_widgetButtonSpacer, Qt::AlignLeft);
	m_layoutButtons.addWidget(&m_buttonApply, 10);
		m_layoutButtons.setAlignment(&m_buttonApply, Qt::AlignRight);
		m_buttonApply.setEnabled(false);
	m_layoutButtons.addWidget(&m_buttonReset, 10);
		m_layoutButtons.setAlignment(&m_buttonReset, Qt::AlignRight);
		m_buttonReset.setEnabled(false);

	// load settings

	loadSettings();

	// try loading valid station names from file

	loadStationNames();

	// set up events

	connect(&m_buttonApply, SIGNAL(clicked(bool)),
				this, SLOT(onButtonApplyClicked(bool)));
	connect(&m_buttonReset, SIGNAL(clicked(bool)),
				this, SLOT(onButtonResetClicked(bool)));
	connect(&m_spinTimeOffset, SIGNAL(valueChanged(int)),
				this, SLOT(onSpinButtonvalueChanged(int)));
	connect(&m_comboLocation, SIGNAL(editTextChanged(const QString&)),
				this, SLOT(onComboBoxTextChanged(const QString&)));
	connect(&m_comboStation, SIGNAL(editTextChanged(const QString&)),
				this, SLOT(onComboBoxTextChanged(const QString&)));
#if 0
	QLineEdit* e = m_comboStation.lineEdit();
	if(e) {
		connect(e, SIGNAL(returnPressed()),
				this, SLOT(onComboStationReturnPressed()));
		connect(e, SIGNAL(textEdited(const QString&)),
				this, SLOT(onComboStationTextEdited(const QString&)));
	}
#endif
}

CViewSetMonitor::~CViewSetMonitor()
{

}


bool CViewSetMonitor::loadStationNames()
{
	QFile file(":/haltestellen.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

	QTextStream stream(&file);
	//stream.setCodec("UTF-8");
	stream.setAutoDetectUnicode(true);

    QString qstr;
	QChar ch;

	while(!stream.atEnd()) {
		qstr = stream.readLine();
		m_comboStation.addItem(qstr.simplified());
	}

	return true;
}

void CViewSetMonitor::closeEvent(QCloseEvent *event)
{	
	writeSettings();
	event->accept();
}

void CViewSetMonitor::loadSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("SetMonitorPage");
		// ...
    settings.endGroup();
	settings.beginGroup("MonitorPage");
		
    settings.endGroup();
	settings.beginGroup("Monitor");
		m_comboLocation.insertItem(0, settings.value("location", "Dresden").toString());
		m_comboStation.insertItem(0, settings.value("station", "Albertplatz").toString());
		m_spinTimeOffset.setValue(settings.value("eta", 3).toInt());
	settings.endGroup();
}

void CViewSetMonitor::writeSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("SetMonitorPage");
		// ...
    settings.endGroup();
	settings.beginGroup("MonitorPage");
		// ...
    settings.endGroup();
	settings.beginGroup("Monitor");
		settings.setValue("location", m_comboLocation.currentText());
		settings.setValue("station", m_comboStation.currentText());
		settings.setValue("eta", m_spinTimeOffset.value());
	settings.endGroup();

	QStringList list("SetMonitorPage");
	list += "MonitorPage";
	list += "Monitor";
	settings.adviseOfChanges(list, (void*)this);
}

void CViewSetMonitor::onValuesChanged(QStringList& affected, void* causer)
{
	if((causer != (void*)this) && (affected.contains("SetMonitorPage") || affected.contains("MonitorPage") || affected.contains("Monitor"))) {
		loadSettings();
	}
}

void CViewSetMonitor::onButtonApplyClicked(bool)
{
	if(!m_buttonApply.isEnabled())
		return;
	writeSettings();
	m_buttonApply.setEnabled(false);
	m_buttonReset.setEnabled(false);
	emit settingsApplied();
}

void CViewSetMonitor::onButtonResetClicked(bool)
{
	if(!m_buttonReset.isEnabled())
		return;
	loadSettings();
	m_buttonApply.setEnabled(false);
	m_buttonReset.setEnabled(false);
}

void CViewSetMonitor::onComboBoxTextChanged(const QString&)
{
	m_buttonApply.setEnabled(true);
	m_buttonReset.setEnabled(true);
}

void CViewSetMonitor::onSpinButtonvalueChanged(int)
{
	m_buttonApply.setEnabled(true);
	m_buttonReset.setEnabled(true);
}

void CViewSetMonitor::onCheckBoxStateChanged(int)
{
	m_buttonApply.setEnabled(true);
	m_buttonReset.setEnabled(true);
}

void CViewSetMonitor::showEvent(QShowEvent*)
{
	m_comboStation.setFocus(Qt::OtherFocusReason);
	QLineEdit* e = m_comboStation.lineEdit();
	if(e) {
		e->selectAll();
	}
}

void CViewSetMonitor::onComboStationReturnPressed()
{
#ifdef _DEBUG
	std::cout << "return pressed... \n";	
#endif
}

void CViewSetMonitor::onComboStationTextEdited(const QString&)
{
	QCompleter* c = m_comboStation.completer();
	QLineEdit* l = m_comboStation.lineEdit();

	QString completion = c->currentCompletion();
	QString prefix = c->completionPrefix();
	
	int start = prefix.length() - 1;
	if(start < 0)
		start = 0;
	int end = completion.length() - 1;
	if(end < 0)
		end = 0;
	l->setText(completion);
	l->setSelection(start, end);
}

