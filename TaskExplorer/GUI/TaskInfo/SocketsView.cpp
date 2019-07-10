#include "stdafx.h"
#include "../TaskExplorer.h"
#include "SocketsView.h"
#include "../../Common/Common.h"
#ifdef WIN32
#include "../../API/Windows/WinSocket.h"		
#endif


CSocketsView::CSocketsView(bool bAll, QWidget *parent)
	:CPanelView(parent)
{
	m_pMainLayout = new QHBoxLayout();
	m_pMainLayout->setMargin(0);
	this->setLayout(m_pMainLayout);

	m_pSocketModel = new CSocketModel();
	
	m_pSortProxy = new CSortFilterProxyModel(false, this);
	m_pSortProxy->setSortRole(Qt::EditRole);
    m_pSortProxy->setSourceModel(m_pSocketModel);
	m_pSortProxy->setDynamicSortFilter(true);


	// Socket List
	m_pSocketList = new QTreeViewEx();
	m_pSocketList->setItemDelegate(new CStyledGridItemDelegate(m_pSocketList->fontMetrics().height() + 3, this));

	m_pSocketList->setModel(m_pSortProxy);

	m_pSocketList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pSocketList->setSortingEnabled(true);

	if (!bAll)
	{
		m_pSocketModel->SetProcessFilter(CProcessPtr());

		//Hide unneded columns
		m_pSocketList->setColumnFixed(CSocketModel::eProcess, true);
		m_pSocketList->setColumnHidden(CSocketModel::eProcess, true);
#ifdef WIN32
		m_pSocketList->setColumnFixed(CSocketModel::eOwner, true);
		m_pSocketList->setColumnHidden(CSocketModel::eOwner, true);
#endif
	}

	m_pSocketList->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pSocketList, SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(OnMenu(const QPoint &)));

	m_pMainLayout->addWidget(m_pSocketList);
	// 

	connect(theAPI, SIGNAL(SocketListUpdated(QSet<quint64>, QSet<quint64>, QSet<quint64>)), this, SLOT(OnSocketListUpdated(QSet<quint64>, QSet<quint64>, QSet<quint64>)));

	//m_pMenu = new QMenu();
	m_pClose = m_pMenu->addAction(tr("Close"), this, SLOT(OnClose()));
	AddPanelItemsToMenu();

	setObjectName(parent->objectName());
	m_pSocketList->header()->restoreState(theConf->GetBlob(objectName() + "/SocketsView_Columns"));
}


CSocketsView::~CSocketsView()
{
	theConf->SetBlob(objectName() + "/SocketsView_Columns", m_pSocketList->header()->saveState());
}

void CSocketsView::ShowProcess(const CProcessPtr& pProcess)
{
	m_pSocketModel->SetProcessFilter(pProcess);

	OnSocketListUpdated(QSet<quint64>(),QSet<quint64>(),QSet<quint64>());
}

void CSocketsView::Refresh()
{
	// noting
}

void CSocketsView::OnSocketListUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed)
{
	QMultiMap<quint64, CSocketPtr> SocketList = theAPI->GetSocketList();

	m_pSocketModel->Sync(SocketList);
}

void CSocketsView::OnMenu(const QPoint &point)
{
	QModelIndex Index = m_pSocketList->currentIndex();
	
	m_pClose->setEnabled(Index.isValid());
	
	CPanelView::OnMenu(point);
}

void CSocketsView::OnClose()
{
	if(QMessageBox("TaskExplorer", tr("Do you want to close the selected socket(s)"), QMessageBox::Question, QMessageBox::Yes, QMessageBox::No | QMessageBox::Default | QMessageBox::Escape, QMessageBox::NoButton).exec() != QMessageBox::Yes)
		return;

	QList<STATUS> Errors;
	foreach(const QModelIndex& Index, m_pSocketList->selectedRows())
	{
		QModelIndex ModelIndex = m_pSortProxy->mapToSource(Index);
		CSocketPtr pSocket = m_pSocketModel->GetSocket(ModelIndex);
		if (!pSocket.isNull())
		{
			STATUS Status = pSocket->Close();
				
			if(Status.IsError())
				Errors.append(Status);
		}
	}

	CTaskExplorer::CheckErrors(Errors);
}
