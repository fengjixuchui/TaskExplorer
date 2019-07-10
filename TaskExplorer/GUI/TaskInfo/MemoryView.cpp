#include "stdafx.h"
#include "../TaskExplorer.h"
#include "MemoryView.h"
#include "../../Common/Common.h"
#ifdef WIN32
#include "../../API/Windows/WinMemory.h"
#endif
#include "../MemoryEditor.h"

CMemoryView::CMemoryView(QWidget *parent)
	:CPanelView(parent)
{
	m_pMainLayout = new QVBoxLayout();
	m_pMainLayout->setMargin(0);
	this->setLayout(m_pMainLayout);

	m_pFilterWidget = new QWidget();
	m_pMainLayout->addWidget(m_pFilterWidget);

	m_pFilterLayout = new QHBoxLayout();
	m_pFilterLayout->setMargin(3);
	m_pFilterWidget->setLayout(m_pFilterLayout);

	m_pHideFree = new QCheckBox(tr("Hide Free"));
	m_pFilterLayout->addWidget(m_pHideFree);

	m_pRefresh = new QPushButton(tr("Refresh"));
	m_pFilterLayout->addWidget(m_pRefresh);

	m_pFilterLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_pHideFree->setChecked(theConf->GetBool("MemoryView/HideFree", true));

	connect(m_pHideFree, SIGNAL(stateChanged(int)), this, SLOT(UpdateFilter()));
	connect(m_pRefresh, SIGNAL(toggled(bool)), this, SLOT(Refresh()));


	m_pMemoryModel = new CMemoryModel();
	
	m_pSortProxy = new CMemoryFilterModel(false, this);
	m_pSortProxy->setSortRole(Qt::EditRole);
    m_pSortProxy->setSourceModel(m_pMemoryModel);
	m_pSortProxy->setDynamicSortFilter(true);


	// Memory List
	m_pMemoryList = new QTreeViewEx();
	m_pMemoryList->setItemDelegate(new CStyledGridItemDelegate(m_pMemoryList->fontMetrics().height() + 3, this));

	m_pMemoryList->setModel(m_pSortProxy);

	m_pMemoryList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pMemoryList->setSortingEnabled(true);

	m_pMemoryList->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pMemoryList, SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(OnMenu(const QPoint &)));

	connect(m_pMemoryList, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(OnDoubleClicked(const QModelIndex&)));

	m_pMainLayout->addWidget(m_pMemoryList);
	// 

	UpdateFilter();

	connect(theAPI, SIGNAL(MemoryListUpdated(QSet<quint64>, QSet<quint64>, QSet<quint64>)), this, SLOT(OnMemoryListUpdated(QSet<quint64>, QSet<quint64>, QSet<quint64>)));

	//m_pMenu = new QMenu();

	m_pMenuSave = m_pMenu->addAction(tr("Dump memory"), this, SLOT(OnSaveMemory()));
	m_pMenuProtect = m_pMenu->addAction(tr("Change protection"), this, SLOT(OnProtectMemory()));
	m_pMenuFree = m_pMenu->addAction(tr("Free"), this, SLOT(OnFreeMemory()));
	m_pMenuDecommit = m_pMenu->addAction(tr("Decommit"), this, SLOT(OnFreeMemory()));

	AddPanelItemsToMenu();

	setObjectName(parent->objectName());
	m_pMemoryList->header()->restoreState(theConf->GetBlob(objectName() + "/MemorysView_Columns"));
}


CMemoryView::~CMemoryView()
{
	theConf->SetBlob(objectName() + "/MemorysView_Columns", m_pMemoryList->header()->saveState());
}

void CMemoryView::ShowProcess(const CProcessPtr& pProcess)
{
	m_pCurProcess = pProcess;

	Refresh();

	m_pMemoryList->expandAll();
}

void CMemoryView::Refresh()
{
	if (!m_pCurProcess)
		return;

	QMultiMap<quint64, CMemoryPtr> MemoryList = m_pCurProcess->GetMemoryMap();

	m_pMemoryModel->Sync(MemoryList);
}

void CMemoryView::OnMenu(const QPoint &point)
{
	QModelIndex Index = m_pMemoryList->currentIndex();
	QModelIndex ModelIndex = m_pSortProxy->mapToSource(Index);
	CMemoryPtr pMemory = m_pMemoryModel->GetMemory(ModelIndex);

	int count = m_pMemoryList->selectedRows().count();

    if (count == 0 || (count == 1 && !pMemory->IsAllocationBase()))
    {
		m_pMenuSave->setEnabled(count == 1);
		m_pMenuProtect->setEnabled(count == 1 && !pMemory->IsFree());
		m_pMenuFree->setEnabled(count == 1 && !pMemory->IsFree());
		m_pMenuDecommit->setEnabled(count == 1 && !pMemory->IsFree() && !pMemory->IsMapped());
    }
    else
    {
		ulong numberOfAllocationBase = 0;
		foreach(const QModelIndex& Index, m_pMemoryList->selectedRows())
		{
			QModelIndex ModelIndex = m_pSortProxy->mapToSource(Index);
			CMemoryPtr pMemory = m_pMemoryModel->GetMemory(ModelIndex);

            if (pMemory->IsAllocationBase())
                numberOfAllocationBase++;
        }

        m_pMenuSave->setEnabled(numberOfAllocationBase == 0 || numberOfAllocationBase == count);
		m_pMenuProtect->setEnabled(false);
		m_pMenuFree->setEnabled(false);
		m_pMenuDecommit->setEnabled(false);
    }

	CPanelView::OnMenu(point);
}

void CMemoryView::OnDoubleClicked(const QModelIndex& Index)
{
	QModelIndex ModelIndex = m_pSortProxy->mapToSource(Index);
	CMemoryPtr pMemory = m_pMemoryModel->GetMemory(ModelIndex);
	if (!pMemory)
		return;

	QIODevice* pDevice = pMemory->MkDevice();
	if (!pDevice) {
		QMessageBox("TaskExplorer", tr("This memory region can not be edited"), QMessageBox::Warning, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton).exec();
		return;
	}

	CMemoryEditor* pEditor = new CMemoryEditor();
	pEditor->setWindowTitle(tr("Memory Editor: %1 (%2) 0x%3").arg(m_pCurProcess->GetName()).arg(m_pCurProcess->GetParentId()).arg(pMemory->GetBaseAddress(),0,16));
	pEditor->setDevice(pDevice, pMemory->GetBaseAddress());
	pEditor->show();
}

void CMemoryView::OnSaveMemory()
{
	QFile DumpFile;
	if (sender() == m_pMenuSave)
	{
		QString DumpPath = QFileDialog::getSaveFileName(this, tr("Dump memory"), "", tr("Dump files (*.dmp);;All files (*.*)"));
		if (DumpPath.isEmpty())
			return;
		DumpFile.setFileName(DumpPath);
		DumpFile.open(QFile::WriteOnly);
	}

	QList<STATUS> Errors;
	int Force = -1;
	foreach(const QModelIndex& Index, m_pMemoryList->selectedRows())
	{
		QModelIndex ModelIndex = m_pSortProxy->mapToSource(Index);
		CMemoryPtr pMemory = m_pMemoryModel->GetMemory(ModelIndex);

		if (!pMemory.isNull())
		{
			Errors.append(pMemory->DumpMemory(&DumpFile));
		}
	}
	
	CTaskExplorer::CheckErrors(Errors);
}

void CMemoryView::OnFreeMemory()
{
	QModelIndex Index = m_pMemoryList->currentIndex();
	QModelIndex ModelIndex = m_pSortProxy->mapToSource(Index);
	CMemoryPtr pMemory = m_pMemoryModel->GetMemory(ModelIndex);
	if (!pMemory)
		return;

	bool bFree;
	if ((bFree = (sender() == m_pMenuFree)) || sender() == m_pMenuDecommit)
	{
		QString Message;
        if (pMemory->IsMapped())
			Message = tr("Unmapping a section view may cause the process to crash.");
		else if (bFree)
			Message = tr("Freeing memory regions may cause the process to crash.");
		else
            Message = tr("Decommitting memory regions may cause the process to crash.");

		if (QMessageBox("TaskExplorer", Message, QMessageBox::Question, QMessageBox::Yes, QMessageBox::No | QMessageBox::Default | QMessageBox::Escape, QMessageBox::NoButton).exec() != QMessageBox::Yes)
			return;
	}

	STATUS Status = pMemory->FreeMemory(bFree);
	if(!Status.IsError())
		m_pMemoryModel->RemoveIndex(ModelIndex);

	CTaskExplorer::CheckErrors(QList<STATUS>() << Status);
}

void CMemoryView::OnProtectMemory()
{
	QModelIndex Index = m_pMemoryList->currentIndex();
	QModelIndex ModelIndex = m_pSortProxy->mapToSource(Index);
	CMemoryPtr pMemory = m_pMemoryModel->GetMemory(ModelIndex);
	if (!pMemory)
		return;

	QString MessagPrompt = tr(
		"Possible values:\r\n"
		"\r\n"
		"0x01 - PAGE_NOACCESS\r\n"
		"0x02 - PAGE_READONLY\r\n"
		"0x04 - PAGE_READWRITE\r\n"
		"0x08 - PAGE_WRITECOPY\r\n"
		"0x10 - PAGE_EXECUTE\r\n"
		"0x20 - PAGE_EXECUTE_READ\r\n"
		"0x40 - PAGE_EXECUTE_READWRITE\r\n"
		"0x80 - PAGE_EXECUTE_WRITECOPY\r\n"
		"Modifiers:\r\n"
		"0x100 - PAGE_GUARD\r\n"
		"0x200 - PAGE_NOCACHE\r\n"
		"0x400 - PAGE_WRITECOMBINE\r\n");

	QString OldValue = "0x" + QString::number(pMemory->GetProtect(), 16);

	QString Value = QInputDialog::getText(this, "TaskExplorer", MessagPrompt, QLineEdit::Normal, OldValue);
	if(Value.isEmpty())
		return;

	bool bOK = false;
	quint32 NewValue = Value.mid(2).toInt(&bOK, 16);
	if (Value.left(2).toLower() != "0x" || bOK) {
		QMessageBox("TaskExplorer", tr("InvalidInput"), QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton).exec();
		return;
	}

	if (Value == OldValue)
		return;

	STATUS Status = pMemory->SetProtect(NewValue);
	if (!Status.IsError())
		m_pMemoryModel->UpdateMemory(pMemory);
	CTaskExplorer::CheckErrors(QList<STATUS>() << Status);
}

void CMemoryView::UpdateFilter()
{
	m_pSortProxy->SetFilter(m_pHideFree->isChecked());
}
