#pragma once
#include <qwidget.h>
#include "../../Common/TreeWidgetEx.h"
#include "../../Common/TreeViewEx.h"
#include "../../Common/PanelView.h"
#include "../../Common/SettingsWidgets.h"
#include "..\..\API\WndInfo.h"
#include "../../API/ProcessInfo.h"

class CWindowModel;
class QSortFilterProxyModel;


class CWindowsView : public CPanelView
{
	Q_OBJECT
public:
	CWindowsView(QWidget *parent = 0);
	virtual ~CWindowsView();

public slots:
	void					ShowProcess(const CProcessPtr& pProcess);
	void					Refresh();

private slots:
	void					OnWindowsUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);
	void					OnItemSelected(const QModelIndex &current);

	//void					OnMenu(const QPoint &point);
	void					OnWindowAction();

protected:
	virtual void				OnMenu(const QPoint& Point);

	virtual QTreeView*			GetView() 				{ return m_pWindowList; }
	virtual QAbstractItemModel* GetModel()				{ return m_pSortProxy; }
	//virtual QAbstractItemModel* GetModel()				{ return m_pSocketModel; }
	//virtual QModelIndex			MapToSource(const QModelIndex& Model) { return m_pSortProxy->mapToSource(Model); }

	CProcessPtr				m_pCurProcess;

private:

	QHBoxLayout*			m_pMainLayout;

	QTreeViewEx*			m_pWindowList;
	CWindowModel*			m_pWindowModel;
	QSortFilterProxyModel*	m_pSortProxy;

	//QMenu*					m_pMenu;
	QAction*				m_pBringToFront;
	QAction*				m_pHighlight;

	QAction*				m_pRestore;
	QAction*				m_pMinimize;
	QAction*				m_pMaximize;
	QAction*				m_pClose;
	
	QAction*				m_pVisible;
	QAction*				m_pEnabled;
	QSpinBox*				m_pOpacity;
	QAction*				m_pOnTop;

	QSplitter*				m_pSplitter;

	CPanelWidget<QTreeWidgetEx>* m_pWindowDetails;

	bool					m_LockValue;
};

