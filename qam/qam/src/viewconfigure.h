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

#ifndef _VIEWCONFIGURE_H_
#define _VIEWCONFIGURE_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QCloseEvent>
#include <QTextEdit>
#include <QLineEdit>
#include <QCursor>
#include <QScrollArea>
#include <QValidator>

class CViewConfigure : public QWidget 
{
	Q_OBJECT

  signals:
	void settingsApplied();

  public slots:
	void onValuesChanged(QStringList& affected, void* causer);

  public:
	CViewConfigure(QWidget* parent = 0, Qt::WFlags f = 0);
	~CViewConfigure();

	void closeEvent(QCloseEvent *event);

  protected:
	void mouseMoveEvent(QMouseEvent* e) { e->ignore(); };
	void mousePressEvent(QMouseEvent* e) { m_preMoveCursor = cursor(); setCursor(Qt::ClosedHandCursor); e->ignore(); };
	void mouseReleaseEvent(QMouseEvent* e) { setCursor(m_preMoveCursor); e->ignore(); };
	void enterEvent(QEvent*) { setCursor(Qt::OpenHandCursor); };
	void leaveEvent(QEvent*) { unsetCursor(); };

  private slots:
	void onButtonAboutClicked(bool);
	void onButtonApplyClicked(bool);
	void onButtonResetClicked(bool);
	void onComboBoxTextChanged(const QString& text);
	void onSpinButtonvalueChanged(int i); 
	void onCheckBoxStateChanged(int state);
	void onEditTextChanged(const QString&);

  private:
	class CGroupBox : public QGroupBox
	{
	  public:
		CGroupBox(const QString& title, QWidget* parent = 0) : QGroupBox(title, parent) {};
	  protected:
		void enterEvent(QEvent*) { setCursor(Qt::ArrowCursor); };
		void leaveEvent(QEvent*) { unsetCursor(); };
	};
	class CScrollArea : public QScrollArea
	{
	  public:
		CScrollArea() : QScrollArea() {};
	  protected:
		void enterEvent(QEvent*) { m_preMoveCursor = cursor(); setCursor(Qt::ArrowCursor); };
		void leaveEvent(QEvent*) { setCursor(m_preMoveCursor); };
	  private:
		QCursor m_preMoveCursor;
	};
	class CPushButton : public QPushButton
	{
	  public:
		CPushButton(const QString& title, QWidget* parent = 0) : QPushButton(title, parent) { setMouseTracking(true); };
	  protected:
		void enterEvent(QEvent*) { m_preMoveCursor = cursor(); setCursor(Qt::ArrowCursor); };
		void leaveEvent(QEvent*) { setCursor(m_preMoveCursor); };
	  private:
		QCursor m_preMoveCursor;
	};

	QVBoxLayout m_topLayout;
	CScrollArea m_scrollArea;
	QWidget m_widgetScrollChild;
	QVBoxLayout m_vboxScrollLayout;
	
	// window settings
	CGroupBox m_groupWindowSettings;
		QGridLayout m_gridWindowSettings;
		QLabel m_labelUpdateInterval;
		QSpinBox m_spinUpdateInterval;
		QLabel m_labelWindowTransparency;
		QSpinBox m_spinWindowTransparency;
		QCheckBox m_checkWindowAlwaysOnTop;

	// network settings
	CGroupBox m_groupNetworkSettings;
		QGridLayout m_gridNetworkSettings;
		QCheckBox m_checkCheckForNewVersion;
		QCheckBox m_checkUseProxy;
		QLabel m_labelProxy;
		QHBoxLayout m_hboxProxyInput;
			QLineEdit m_editProxyHost;
		QLabel m_labelProxyUser;
		QLineEdit m_editProxyUser;
		QLabel m_labelProxyPassword;
		QLineEdit m_editProxyPassword;

	CGroupBox m_groupAbout;
		QVBoxLayout m_layoutAbout;
		QTextEdit m_labelAbout;

	QWidget m_spacer;

	QWidget m_widgetButtons;
		QHBoxLayout m_layoutButtons;
		CPushButton m_buttonAbout;
		CPushButton m_buttonApply;
		CPushButton m_buttonReset;
		QWidget m_widgetButtonSpacer;

	QCursor m_preMoveCursor;
	bool m_bShowingAbout;

	void loadSettings();
	void writeSettings();
	void switchProxySettingsVisible(bool show);
	bool proxyInputValid() const;
};

#endif // #ifndef _VIEWCONFIGURE_H_

