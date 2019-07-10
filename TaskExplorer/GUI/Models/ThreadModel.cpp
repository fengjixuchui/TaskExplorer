#include "stdafx.h"
#include "../TaskExplorer.h"
#include "ThreadModel.h"
#include "../../Common/Common.h"
#ifdef WIN32
#include "../../API/Windows/WinThread.h"
#endif

CThreadModel::CThreadModel(QObject *parent)
:CListItemModel(parent)
{
}

CThreadModel::~CThreadModel()
{
}

void CThreadModel::Sync(QMap<quint64, CThreadPtr> ThreadList)
{
	QList<SListNode*> New;
	QMap<QVariant, SListNode*> Old = m_Map;

	foreach (const CThreadPtr& pThread, ThreadList)
	{
		QVariant ID = pThread->GetThreadId();

		int Row = -1;
		SThreadNode* pNode = static_cast<SThreadNode*>(Old[ID]);
		if(!pNode)
		{
			pNode = static_cast<SThreadNode*>(MkNode(ID));
			pNode->Values.resize(columnCount());
			pNode->pThread = pThread;
			pNode->IsBold = pThread->IsMainThread();
			New.append(pNode);
		}
		else
		{
			Old[ID] = NULL;
			Row = m_List.indexOf(pNode);
		}

#ifdef WIN32
		CWinThread* pWinThread = qobject_cast<CWinThread*>(pThread.data());
#endif

		int Col = 0;
		bool State = false;
		int Changed = 0;

		int RowColor = CTaskExplorer::eNone;
		if (pThread->IsMarkedForRemoval())		RowColor = CTaskExplorer::eToBeRemoved;
		else if (pThread->IsNewlyCreated())		RowColor = CTaskExplorer::eAdded;
#ifdef WIN32
		else if (pWinThread->IsGuiThread())		RowColor = CTaskExplorer::eGuiThread;
#endif
		
		if (pNode->iColor != RowColor) {
			pNode->iColor = RowColor;
			pNode->Color = CTaskExplorer::GetColor(RowColor);
			Changed = 2;
		}

		if (pNode->IsGray != pThread->IsSuspended())
		{
			pNode->IsGray = pThread->IsSuspended();
			Changed = 2;
		}

		STaskStats CpuStats = pThread->GetCpuStats();

		for(int section = eThread; section < columnCount(); section++)
		{
			QVariant Value;
			switch(section)
			{
				case eThread:				Value = pThread->GetThreadId(); break;
				case eCPU_History:
				case eCPU:					Value = CpuStats.CpuUsage; break;
				case eCyclesDelta:			Value = CpuStats.CycleDelta.Delta; break;
#ifdef WIN32
				case eStartAddress:			Value = pWinThread->GetStartAddressString(); break;
#endif
				case ePriority:				Value = pThread->GetPriority(); break;
#ifdef WIN32
				case eService:				Value = pWinThread->GetServiceName(); break;
				case eName:					Value = pWinThread->GetThreadName(); break;
				case eType:					Value = pWinThread->GetTypeString(); break;
#endif
				case eCreated:				Value = pThread->GetCreateTimeStamp(); break;
#ifdef WIN32
				case eStartModule:			Value = pWinThread->GetStartAddressFileName(); break;
#endif
				case eContextSwitches:		Value = CpuStats.ContextSwitchesDelta.Value; break;
				case eBasePriority:			Value = pThread->GetBasePriority(); break;
				case ePagePriority:			Value = pThread->GetPagePriority(); break;
				case eIOPriority:			Value = pThread->GetIOPriority(); break;
				case eCycles:				Value = CpuStats.CycleDelta.Value; break;
				case eState:				Value = pThread->GetStateString(); break;
				case eKernelTime:			Value = CpuStats.CpuKernelDelta.Value;
				case eUserTime:				Value = CpuStats.CpuUserDelta.Value;
#ifdef WIN32
				case eIdealProcessor:		Value = pWinThread->GetIdealProcessor(); break;
				case eCritical:				Value = pWinThread->IsCriticalThread() ? tr("Critical") : ""; break;
#endif
			}

			SThreadNode::SValue& ColValue = pNode->Values[section];

			if (ColValue.Raw != Value)
			{
				if(Changed == 0)
					Changed = 1;
				ColValue.Raw = Value;

				switch (section)
				{
					//case eThread:				ColValue.Formated = "0x" + QString::number(pThread->GetThreadId(), 16); break;
					case eCPU:					ColValue.Formated = QString::number(CpuStats.CpuUsage*100, 10,2) + "%"; break;
					case ePriority:				ColValue.Formated = pThread->GetPriorityString(); break;
					case eCreated:				ColValue.Formated = QDateTime::fromTime_t(pThread->GetCreateTimeStamp()/1000).toString("dd.MM.yyyy hh:mm:ss"); break;
				}
			}

			if(State != (Changed != 0))
			{
				if(State && Row != -1)
					emit dataChanged(createIndex(Row, Col), createIndex(Row, section-1));
				State = (Changed != 0);
				Col = section;
			}
			if (Changed == 1)
				Changed = 0;
		}
		if(State && Row != -1)
			emit dataChanged(createIndex(Row, Col, pNode), createIndex(Row, columnCount()-1, pNode));

	}

	CListItemModel::Sync(New, Old);
}

CThreadPtr CThreadModel::GetThread(const QModelIndex &index) const
{
	if (!index.isValid())
        return CThreadPtr();

	SThreadNode* pNode = static_cast<SThreadNode*>(index.internalPointer());
	return pNode->pThread;
}

int CThreadModel::columnCount(const QModelIndex &parent) const
{
	return eCount;
}

QVariant CThreadModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
			case eThread:				return tr("Thread");
			case eCPU_History:			return tr("CPU graph");
			case eCPU:					return tr("CPU");
			case eCyclesDelta:			return tr("Cycles delta");
#ifdef WIN32
			case eStartAddress:			return tr("Start address");
#endif
			case ePriority:				return tr("Priority");
#ifdef WIN32
			case eService:				return tr("Service");
			case eName:					return tr("Name");
			case eType:					return tr("Type");
#endif
			case eCreated:				return tr("Created");
#ifdef WIN32
			case eStartModule:			return tr("Start module");
#endif
			case eContextSwitches:		return tr("Context switches");
			case eBasePriority:			return tr("Base priority");
			case ePagePriority:			return tr("Page priority");
			case eIOPriority:			return tr("I/O priority");
			case eCycles:				return tr("Cycles");
			case eState:				return tr("State");
			case eKernelTime:			return tr("Kernel time");
			case eUserTime:				return tr("User time");
#ifdef WIN32
			case eIdealProcessor:		return tr("Ideal processor");
			case eCritical:				return tr("Critical");
#endif
		}
	}
    return QVariant();
}
