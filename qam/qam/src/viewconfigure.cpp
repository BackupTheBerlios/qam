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

#include <QMessageBox>

#include "viewconfigure.h"
#include "options.h"
#include "monitor.h"

CViewConfigure::CViewConfigure(QWidget* parent, Qt::WFlags f) :
	QWidget(parent, f),
	
	m_topLayout(this),

	// window settings
	m_groupWindowSettings("Fenster"),
	m_gridWindowSettings(&m_groupWindowSettings),
	m_labelUpdateInterval("Aktualisierungsintervall"),
	m_spinUpdateInterval(),
	m_labelWindowTransparency("Fenstertransparenz"),
	m_spinWindowTransparency(),
	m_checkWindowAlwaysOnTop("Fenster immer zuoberst"),
	m_checkUseTabIcons("Tab-Icons statt Text"),
	
	// network settings
	m_groupNetworkSettings("Netzwerk"),
	m_gridNetworkSettings(&m_groupNetworkSettings),
	m_checkCheckForNewVersion("Auf Updates prüfen"),
	m_checkUseProxy("HTTP-Proxy verwenden"),
	m_labelProxy("Proxy:Port"),
	m_hboxProxyInput(),
	m_labelProxyUser("Proxy User"),
	m_editProxyUser(),
	m_labelProxyPassword("Proxy Passwort"),
	m_editProxyPassword(),

	// about
	m_groupAbout("Über"),
	m_layoutAbout(&m_groupAbout),
	m_labelVersion(QString("Version: %1.%2.%3").arg(CVersion::major()).arg(CVersion::minor()).arg(CVersion::revision())),
	m_editAbout(),

	// buttons
	m_widgetButtons(),
	m_layoutButtons(&m_widgetButtons),
	m_buttonAbout("Über"),
	m_buttonApply("Anwenden"),
	m_buttonReset("Zurücksetzen"),
	m_widgetButtonSpacer(),

	// ...
	m_bShowingAbout(false)
{
	m_topLayout.setMargin(2);

	// window settings group

	m_vboxScrollLayout.addWidget(&m_groupWindowSettings);
	m_groupWindowSettings.setFlat(true);
	m_gridWindowSettings.setMargin(0);
	m_gridWindowSettings.setRowMinimumHeight(0, 5);
	m_gridWindowSettings.addWidget(&m_labelUpdateInterval, 1, 0, 1, 2, Qt::AlignLeft);
	m_gridWindowSettings.addWidget(&m_spinUpdateInterval, 2, 0, 1, 2, Qt::AlignLeft);
		m_spinUpdateInterval.setRange(10, 300);
		m_spinUpdateInterval.setSingleStep(10);
		m_spinUpdateInterval.setSuffix(" sek");
	m_gridWindowSettings.addWidget(&m_labelWindowTransparency, 3, 0, 1, 2, Qt::AlignLeft);
	m_gridWindowSettings.addWidget(&m_spinWindowTransparency, 4, 0, 1, 2, Qt::AlignLeft);
		m_spinWindowTransparency.setRange(0, 80);
		m_spinWindowTransparency.setSingleStep(1);
		m_spinWindowTransparency.setSuffix(" %");
	QHBoxLayout* layoutCheckUseTabIcons = new QHBoxLayout();
	m_gridWindowSettings.addLayout(layoutCheckUseTabIcons, 5, 0, 1, 2);
		layoutCheckUseTabIcons->addWidget(&m_checkUseTabIcons, Qt::AlignLeft);
	QHBoxLayout* layoutCheckWindowOnTop = new QHBoxLayout();
	m_gridWindowSettings.addLayout(layoutCheckWindowOnTop, 6, 0, 1, 2);
	layoutCheckWindowOnTop->addWidget(&m_checkWindowAlwaysOnTop, Qt::AlignLeft);
		m_checkWindowAlwaysOnTop.setEnabled(false);
	layoutCheckWindowOnTop->addStretch(10);

	// network settings group
	
	m_vboxScrollLayout.addWidget(&m_groupNetworkSettings);
	m_groupNetworkSettings.setFlat(true);
	m_gridNetworkSettings.setMargin(0);
	m_gridNetworkSettings.setRowMinimumHeight(0, 5);
	QHBoxLayout* layoutCheckVersion = new QHBoxLayout();
	m_gridNetworkSettings.addLayout(layoutCheckVersion, 1, 0, 1, 2);
	layoutCheckVersion->addWidget(&m_checkCheckForNewVersion, Qt::AlignLeft);
	layoutCheckVersion->addStretch(10);
#if QT_VERSION >= 0x040100
	QHBoxLayout* layoutUseProxy = new QHBoxLayout();
	m_gridNetworkSettings.addLayout(layoutUseProxy, 2, 0, 1, 2);
	layoutUseProxy->addWidget(&m_checkUseProxy, Qt::AlignLeft);
	layoutUseProxy->addStretch(10);
	m_gridNetworkSettings.addWidget(&m_labelProxy, 3, 0, 1, 2, Qt::AlignLeft);
	m_gridNetworkSettings.addWidget(&m_editProxyHost, 4, 0, 1, 2, Qt::AlignLeft);
	m_gridNetworkSettings.addWidget(&m_labelProxyUser, 5, 0, 1, 2, Qt::AlignLeft);
	m_gridNetworkSettings.addWidget(&m_editProxyUser, 6, 0, 1, 2, Qt::AlignLeft);
	m_gridNetworkSettings.addWidget(&m_labelProxyPassword, 7, 0, 1, 2, Qt::AlignLeft);
	m_gridNetworkSettings.addWidget(&m_editProxyPassword, 8, 0, 1, 2, Qt::AlignLeft);
#endif // #if QT_VERSION >= 0x040100

	// add spacer
	
	m_vboxScrollLayout.addStretch(5);

	// set up scrollable area for settings

	m_scrollArea.setWidgetResizable(true);
	m_scrollArea.setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

	m_widgetScrollChild.setLayout(&m_vboxScrollLayout);
	m_scrollArea.setWidget(&m_widgetScrollChild);
	m_topLayout.addWidget(&m_scrollArea);

	// set up about

	m_topLayout.addWidget(&m_groupAbout, 20);
	m_groupAbout.hide();
	m_layoutAbout.addWidget(&m_labelVersion);
	m_layoutAbout.addSpacing(5);
	m_layoutAbout.addWidget(&m_editAbout);
	m_layoutAbout.setMargin(3);
	m_layoutAbout.setSpacing(0);
		QString about = "copyright (C) 2007 by gf@nichtcool.de\n\n"
						"Dieses Programm ist freie Software. Sie können es unter den Bedingungen der GNU General Public License, wie von der Free Software Foundation veröffentlicht, weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz.\n\n"
						"Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License.\n\n"
						"Sie sollten ein Exemplar der GNU General Public License zusammen mit diesem Programm erhalten haben. Falls nicht, schreiben Sie an die Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA.";
		m_editAbout.setText(about);
		m_editAbout.setReadOnly(true);

	// add spacer before buttons

	//m_spacer.setMinimumSize(0,0);
	//m_topLayout.addWidget(&m_spacer);

	// set up button group

	m_topLayout.addWidget(&m_widgetButtons);
	m_widgetButtons.setLayout(&m_layoutButtons);
	m_layoutButtons.setMargin(5);
	m_layoutButtons.setSpacing(2);
	m_layoutButtons.addWidget(&m_buttonAbout);
		m_layoutButtons.setAlignment(&m_buttonReset, Qt::AlignLeft);
	m_layoutButtons.addWidget(&m_widgetButtonSpacer, 100);
		m_layoutButtons.setAlignment(&m_widgetButtonSpacer, Qt::AlignLeft);
		m_widgetButtonSpacer.setMinimumSize(0,0);
	m_layoutButtons.addWidget(&m_buttonApply);
		m_layoutButtons.setAlignment(&m_buttonApply, Qt::AlignRight);
		m_buttonApply.setEnabled(false);
	m_layoutButtons.addWidget(&m_buttonReset);
		m_layoutButtons.setAlignment(&m_buttonReset, Qt::AlignRight);
		m_buttonReset.setEnabled(false);

	// set stretch factors


	// load settings

	loadSettings();

	// set up events

	connect(&m_buttonAbout, SIGNAL(clicked(bool)),
				this, SLOT(onButtonAboutClicked(bool)));
	connect(&m_buttonApply, SIGNAL(clicked(bool)),
				this, SLOT(onButtonApplyClicked(bool)));
	connect(&m_buttonReset, SIGNAL(clicked(bool)),
				this, SLOT(onButtonResetClicked(bool)));
	connect(&m_spinUpdateInterval, SIGNAL(valueChanged(int)),
				this, SLOT(onSpinButtonvalueChanged(int)));
	connect(&m_spinWindowTransparency, SIGNAL(valueChanged(int)),
				this, SLOT(onSpinButtonvalueChanged(int)));
	connect(&m_checkUseTabIcons, SIGNAL(stateChanged(int)),
				this, SLOT(onCheckBoxStateChanged(int)));
	connect(&m_checkWindowAlwaysOnTop, SIGNAL(stateChanged(int)),
				this, SLOT(onCheckBoxStateChanged(int)));

	connect(&m_checkCheckForNewVersion, SIGNAL(stateChanged(int)),
				this, SLOT(onCheckBoxStateChanged(int)));
#if QT_VERSION >= 0x040100
	connect(&m_checkUseProxy, SIGNAL(stateChanged(int)),
				this, SLOT(onCheckBoxStateChanged(int)));
	connect(&m_editProxyHost, SIGNAL(textEdited(const QString&)),
				this, SLOT(onEditTextChanged(const QString&)));
	connect(&m_editProxyUser, SIGNAL(textEdited(const QString&)),
				this, SLOT(onEditTextChanged(const QString&)));
	connect(&m_editProxyPassword, SIGNAL(textEdited(const QString&)),
				this, SLOT(onEditTextChanged(const QString&)));
#endif // #if QT_VERSION >= 0x040100
}

CViewConfigure::~CViewConfigure()
{

}

void CViewConfigure::closeEvent(QCloseEvent *event)
{	
	writeSettings();
	event->accept();
}

void CViewConfigure::loadSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("ConfigurePage");
		// ...
    settings.endGroup();
	settings.beginGroup("MainWindow");
		m_spinUpdateInterval.setValue(settings.value("updateInterval", 10).toInt());
		double fVal = settings.value("windowOpacity", 80).toDouble();
		int nVal = (100 - (int)(fVal * 100));
		m_spinWindowTransparency.setValue(nVal);
		m_checkUseTabIcons.setCheckState(settings.value("useTabIcons", true).toBool() ? Qt::Checked : Qt::Unchecked);
#if 0
		m_checkWindowAlwaysOnTop.setCheckState(settings.value("alwaysOnTop", true).toBool() ? Qt::Checked : Qt::Unchecked);
#endif
    settings.endGroup();

	settings.beginGroup("Network");
		m_checkCheckForNewVersion.setChecked(settings.value("checkForNewVersion", false).toBool() ? Qt::Checked : Qt::Unchecked);
#if QT_VERSION >= 0x040100
		bool useProxy = settings.value("useProxy", false).toBool();
		m_checkUseProxy.setChecked(useProxy ? Qt::Checked : Qt::Unchecked);
		QString proxy = settings.value("proxyHost", "hostname").toString();
		proxy += ":" + settings.value("proxyPort", 80).toString();
		m_editProxyHost.setText(proxy);
		m_editProxyUser.setText(settings.value("proxyUser", "").toString());
		m_editProxyPassword.setText(settings.value("proxyPassword", "").toString());
		m_editProxyPassword.setEchoMode(QLineEdit::Password);

		switchProxySettingsVisible(useProxy);
#endif // #if QT_VERSION >= 0x040100
	settings.endGroup();
}

void CViewConfigure::writeSettings()
{
	COptions& settings = COptions::getInstance();
	settings.beginGroup("ConfigurePage");
		// ...
    settings.endGroup();
	settings.beginGroup("MainWindow");
		settings.setValue("updateInterval", m_spinUpdateInterval.value());
		double val = (double)(100 - m_spinWindowTransparency.value()) / 100;
		settings.setValue("windowOpacity", val);
		settings.setValue("useTabIcons", (m_checkUseTabIcons.checkState() == Qt::Checked) ? true : false);
#if 0
		settings.setValue("alwaysOnTop", m_checkWindowAlwaysOnTop.checkState() == Qt::Checked ? true : false);
#endif
    settings.endGroup();

	settings.beginGroup("Network");
		settings.setValue("checkForNewVersion", (m_checkCheckForNewVersion.checkState() == Qt::Checked) ? true : false);
#if QT_VERSION >= 0x040100
		settings.setValue("useProxy", m_checkUseProxy.checkState() == Qt::Checked ? true : false);
		QString proxyHost = m_editProxyHost.text().section(':', 0, 0);
		QString proxyPort = m_editProxyHost.text().section(':', 1, 1);
		settings.setValue("proxyHost", proxyHost);
		settings.setValue("proxyPort", proxyPort);
		settings.setValue("proxyUser", m_editProxyUser.text());
		settings.setValue("proxyPassword", m_editProxyPassword.text());
#endif
	settings.endGroup();

	QStringList list("ConfigurePage");
	list += "MainWindow";
	list += "Network";
	settings.adviseOfChanges(list, (void*)this);
}

void CViewConfigure::onValuesChanged(QStringList& affected, void* causer)
{
	if((causer != (void*)this) && (affected.contains("ConfigurePage") || affected.contains("MainWindow") || affected.contains("Network"))) {
		loadSettings();
	}
}

void CViewConfigure::onButtonApplyClicked(bool)
{
	if(m_checkUseProxy.checkState() == Qt::Checked) {
		if(!proxyInputValid()) {
			QMessageBox::critical(this, tr("Proxy ungültig"), tr("Die Angabe für den Proxy ist ungültig."), QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}
	}

	writeSettings();
	m_buttonApply.setEnabled(false);
	m_buttonReset.setEnabled(false);
	emit settingsApplied();
}

void CViewConfigure::onButtonResetClicked(bool)
{
	loadSettings();
	m_buttonApply.setEnabled(false);
	m_buttonReset.setEnabled(false);
}

void CViewConfigure::onComboBoxTextChanged(const QString&)
{
	m_buttonApply.setEnabled(true);
	m_buttonReset.setEnabled(true);
}

void CViewConfigure::onSpinButtonvalueChanged(int)
{
	m_buttonApply.setEnabled(true);
	m_buttonReset.setEnabled(true);
}

void CViewConfigure::onEditTextChanged(const QString&)
{
	m_buttonApply.setEnabled(true);
	m_buttonReset.setEnabled(true);
}

void CViewConfigure::onCheckBoxStateChanged(int)
{
	m_buttonApply.setEnabled(true);
	m_buttonReset.setEnabled(true);

	switchProxySettingsVisible(m_checkUseProxy.checkState() == Qt::Checked ? true : false);
}

void CViewConfigure::onButtonAboutClicked(bool)
{
	bool s = m_bShowingAbout;

	m_scrollArea.setVisible(s);
	m_groupAbout.setVisible(!s);

	m_bShowingAbout = !s;
}

void CViewConfigure::switchProxySettingsVisible(bool show)
{
	m_labelProxy.setVisible(show);
	m_labelProxyUser.setVisible(show);
	m_labelProxyPassword.setVisible(show);
	m_editProxyHost.setVisible(show);
	m_editProxyUser.setVisible(show);
	m_editProxyPassword.setVisible(show);
}

bool CViewConfigure::proxyInputValid() const
{
	const QString h = m_editProxyHost.text().section(':', 0, 0);
	const QString p = m_editProxyHost.text().section(':', 1, 1);

	if((h.length() < 1) || (h.length() > 255))
		return false;
	if((p.length() < 1) || (p.length() > 5))
		return false;

	int i;

	for(i=0;i<p.length();i++) {
		if(!p[i].isDigit())
			return false;
	}

	const QStringList hs = h.split('.');

	if(hs.size() < 1) {
		return false;
	}

	for(i=0;i<hs.size();i++) {
		const QString& e = hs[i];
		int n;
		
		if((e.length() < 1) || (e.length() > 63)) 
			return false;

		if(!e[0].isLetter())
			return false;
		
		for(n=0;n<e.length();n++) {
			const char c = e[n].toAscii();
			const QChar& q = e[n];
			
			if(c == 0)
				return false;
			if( ! ((c == '-') || q.isDigit() || q.isLetter()) )
				return false;
		}
		
		if(!(e[n-1].isDigit() || e[n-1].isLetter()))
			return false;
	}

	return true;	
}