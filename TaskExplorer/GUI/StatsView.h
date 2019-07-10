#pragma once
#include <qwidget.h>
#include "../Common/TreeViewEx.h"
#include "../Common/TreeWidgetEx.h"
#include "../Common/PanelView.h"
#include "..\..\API\ProcessInfo.h"
#ifdef WIN32
#include "../../API/Windows/WinProcess.h"
#endif

class CStatsView : public CPanelView
{
	Q_OBJECT
public:
	enum EView {
		eSystem = 0,
		eProcess = 1,
		eJob = 2
	};

	CStatsView(EView eView, QWidget *parent = 0);
	virtual ~CStatsView();

public slots:

	//void					OnMenu(const QPoint &point);

	void					ShowProcess(const CProcessPtr& pProcess);
	void					ShowSystem();
#ifdef WIN32
	void					ShowJob(const CWinJobPtr& pCurJob);
#endif

protected:
	//virtual void				OnMenu(const QPoint& Point);
	virtual QTreeView*			GetView()	{ return m_pStatsList; }
	virtual QAbstractItemModel* GetModel()	{ return m_pStatsList->model(); }

	void					ShowIoStats(const SProcStats& Stats);

private:
	bool					m_MonitorsETW;
	EView					m_eView;

	void					SetupTree();

	enum EStackColumns
	{
		eName = 0,
		eCount,
		eSize,
		eRate,
		eDelta,
		ePeak,
		eLimit,
		eColCount
	};

	QVBoxLayout*			m_pMainLayout;

	QTreeWidgetEx*			m_pStatsList;		//	Name	Count	Size	Rate	Delta	Peak	Limit
	
	QTreeWidgetItem*		m_pCPU;				
	QTreeWidgetItem*		m_pCycles;					//	###						###
	QTreeWidgetItem*		m_pKernelTime;				//	###						###
	QTreeWidgetItem*		m_pUserTime;				//	###						###
	QTreeWidgetItem*		m_pTotalTime;				//	###						###
	QTreeWidgetItem*		m_pContextSwitches;			//	###						###

	QTreeWidgetItem*		m_pMemory;
	QTreeWidgetItem*		m_pPrivateBytes;			//			###				###
	QTreeWidgetItem*		m_pVirtualSize;				//			###						###
	QTreeWidgetItem*		m_pPageFaults;				//	###						###
	QTreeWidgetItem*		m_pCommitCharge;			//			###						###		###
	QTreeWidgetItem*		m_pWorkingSet;				//			###
	QTreeWidgetItem*		m_pPrivateWS;				//			###
	QTreeWidgetItem*		m_pSharedWS;				//			###								###
	QTreeWidgetItem*		m_pPagedPool;				//			###						###
	QTreeWidgetItem*		m_pNonPagedPool;			//			###						###




	QTreeWidgetItem*		m_pIO;
	QTreeWidgetItem*		m_pIOReads;					//	###		###		###		###
	QTreeWidgetItem*		m_pIOWrites;				//	###		###		###		###
	QTreeWidgetItem*		m_pIOOther;					//	###		###		###		###
	QTreeWidgetItem*		m_pMMapIOReads;				//	###		###		###		###
	QTreeWidgetItem*		m_pMMapIOWrites;			//	###		###		###		###
	QTreeWidgetItem*		m_pDiskReads;				//	###		###		###		###
	QTreeWidgetItem*		m_pDiskWrites;				//	###		###		###		###
	QTreeWidgetItem*		m_pNetSends;				//	###		###		###		###
	QTreeWidgetItem*		m_pNetReceives;				//	###		###		###		###


	QTreeWidgetItem*		m_pOther;
	QTreeWidgetItem*		m_pProcesses;				//	###	
	QTreeWidgetItem*		m_pThreads;					//	###	
	QTreeWidgetItem*		m_pHandles;					//	###								###
	QTreeWidgetItem*		m_pGdiObjects;				//	###
	QTreeWidgetItem*		m_pUserObjects;				//	###
	QTreeWidgetItem*		m_pWndObjects;				//	###
	

	//QMenu*					m_pMenu;

};
