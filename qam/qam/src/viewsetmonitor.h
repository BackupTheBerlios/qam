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

#ifndef _VIEWSETMONITOR_H_
#define _VIEWSETMONITOR_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QCloseEvent>
#include <QCursor>

class CViewSetMonitor : public QWidget 
{
	Q_OBJECT

  public slots:
	void onValuesChanged(QStringList& affected, void* causer);

  public:
	CViewSetMonitor(QWidget* parent = 0, Qt::WFlags f = 0);
	~CViewSetMonitor();

	void closeEvent(QCloseEvent *event);
  
  signals:
	void settingsApplied();

  protected:
	void mouseMoveEvent(QMouseEvent* e) { e->ignore(); };
	void mousePressEvent(QMouseEvent* e) { m_preMoveCursor = cursor(); setCursor(Qt::ClosedHandCursor); e->ignore(); };
	void mouseReleaseEvent(QMouseEvent* e) { setCursor(m_preMoveCursor); e->ignore(); };
	void enterEvent(QEvent*) { setCursor(Qt::OpenHandCursor); };
	void leaveEvent(QEvent*) { unsetCursor(); };
	void showEvent(QShowEvent*);

  private slots:
	void onButtonApplyClicked(bool);
	void onButtonResetClicked(bool);
	void onComboBoxTextChanged(const QString& text);
	void onSpinButtonvalueChanged(int i); 
	void onCheckBoxStateChanged(int state);
	void onComboStationReturnPressed();
	void onComboStationTextEdited(const QString&);

  private:
	class CGroupBox : public QGroupBox
	{
	  public:
		CGroupBox(const QString& title, QWidget* parent = 0) : QGroupBox(title, parent) {};
	  protected:
		void enterEvent(QEvent*) { setCursor(Qt::ArrowCursor); };
		void leaveEvent(QEvent*) { unsetCursor(); };
	};
	class CPushButton : public QPushButton
	{
	  public:
		CPushButton(const QString& text, QWidget* parent = 0) : QPushButton(text, parent) {};

	  protected:
		void enterEvent(QEvent*) { m_preMoveCursor = cursor(); setCursor(Qt::ArrowCursor);};
		void leaveEvent(QEvent*) { setCursor(m_preMoveCursor); };
	  private:
		QCursor m_preMoveCursor;
	};

	QVBoxLayout m_topLayout;
	
	CGroupBox m_groupCommonSettings;
		QGridLayout m_gridCommonSettings;
		QLabel m_labelLocation;
		QComboBox m_comboLocation;
		QLabel m_labelStation;
		QComboBox m_comboStation;
		QLabel m_labelTimeOffset;
		QSpinBox m_spinTimeOffset;

	QWidget m_widgetButtons;
		QHBoxLayout m_layoutButtons;
		CPushButton m_buttonApply;
		CPushButton m_buttonReset;
		QWidget m_widgetButtonSpacer;

	QCursor m_preMoveCursor;

	bool loadStationNames();
	void loadSettings();
	void writeSettings();
};

#endif // #ifndef _VIEWSETMONITOR_H_

