#pragma once
#include <qwidget.h>
#include "../Common/SplitTreeView.h"
#include "../Common/HistoryGraph.h"
#include "..\API\ProcessInfo.h"
#include "TaskView.h"

class CProcessModel;
class QSortFilterProxyModel;

class CProcessTree : public CTaskView
{
	Q_OBJECT
public:
	CProcessTree(QWidget *parent = 0);
	virtual ~CProcessTree();

signals:
	void					ProcessClicked(const CProcessPtr& pProcess);

private slots:
	void					OnTreeEnabled(bool bEnabled);

	void					OnProcessListUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);

	void					OnUpdateHistory();

	//void					OnClicked(const QModelIndex& Index);
	void					OnDoubleClicked(const QModelIndex& Index);
	void					OnCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
	
	void					OnHeaderMenu(const QPoint& Point);
	void					OnHeaderMenu();

	//void					OnMenu(const QPoint& Point);

	void					OnCrashDump();
	void					OnProcessAction();
	void					OnRunAsThis();

protected:
	virtual QList<CTaskPtr>		GetSellectedTasks();

	virtual void				OnMenu(const QPoint& Point);
	virtual QTreeView*			GetView() 				{ return m_pProcessList->GetView(); }
	virtual QAbstractItemModel* GetModel()				{ return m_pSortProxy; }
	//virtual QAbstractItemModel* GetModel()				{ return m_pHandleModel; }
	//virtual QModelIndex			MapToSource(const QModelIndex& Model) { return m_pSortProxy->mapToSource(Model); }

private:
	void					AddHeaderSubMenu(QMenu* m_pHeaderMenu, const QString& Label, int from, int to);

	QHBoxLayout*			m_pMainLayout;

	CProcessModel*			m_pProcessModel;
	QSortFilterProxyModel*	m_pSortProxy;
	CSplitTreeView*			m_pProcessList;
	QMap<quint64, QPair<QPointer<CHistoryGraph>, QPersistentModelIndex> > m_CPU_History;
	QMap<quint64, QPair<QPointer<CHistoryGraph>, QPersistentModelIndex> > m_MEM_History;
	QMap<quint64, QPair<QPointer<CHistoryGraph>, QPersistentModelIndex> > m_IO_History;
	QMap<quint64, QPair<QPointer<CHistoryGraph>, QPersistentModelIndex> > m_NET_History;

	QMenu*					m_pHeaderMenu;
	QMap<QCheckBox*,int>	m_Columns;
	bool					m_ExpandAll;

	//QMenu*					m_pMenu;

	//QAction*				m_pTerminateTree;
	QAction*				m_pCreateDump;
	QAction*				m_pDebug; // []
#ifdef WIN32
	QAction*				m_pVirtualization; // []
	//QAction*				m_pWindows;
	//QAction*				m_pGDI_Handles;
	QAction*				m_pCritical; // []
	QAction*				m_pReduceWS;
	//QAction*				m_pUnloadModules;
	//QAction*				m_pWatchWS;
#endif
	QAction*				m_pRunAsThis;
};
