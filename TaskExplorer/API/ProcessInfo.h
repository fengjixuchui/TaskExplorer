#pragma once
#include <qobject.h>

#include "ModuleInfo.h"
#include "ThreadInfo.h"
#include "HandleInfo.h"
#include "WndInfo.h"
#include "AbstractTask.h"
#include "IOStats.h"
#include "MemoryInfo.h"

struct STaskStatsEx : STaskStats
{
	SDelta32 		PageFaultsDelta;
	SDelta64 		PrivateBytesDelta;
};

class CProcessInfo: public CAbstractTask
{
	Q_OBJECT

public:
	CProcessInfo(QObject *parent = nullptr);
	virtual ~CProcessInfo();

	// Basic
	virtual quint64 GetProcessId() const				{ QReadLocker Locker(&m_Mutex); return m_ProcessId; }
	virtual quint64 GetParentId() const					{ QReadLocker Locker(&m_Mutex); return m_ParentProcessId; }
	virtual QString GetName() const						{ QReadLocker Locker(&m_Mutex); return m_ProcessName; }

	virtual bool ValidateParent(CProcessInfo* pParent) const = 0;

	virtual QString GetArchString() const = 0;
	virtual quint64 GetSessionID() const = 0;

	virtual ulong GetSubsystem() const = 0;
	virtual QString GetSubsystemString() const = 0; // on windows wls, etc... on linux wine or not

	// Parameters
	virtual QString GetFileName() const					{ QReadLocker Locker(&m_Mutex); return m_FileName; }
	virtual QString GetCommandLine() const				{ QReadLocker Locker(&m_Mutex); return m_CommandLine; }
	virtual QString GetWorkingDirectory() const			{ QReadLocker Locker(&m_Mutex); return m_WorkingDirectory; }

	// Other fields
	virtual QString GetUserName() const					{ QReadLocker Locker(&m_Mutex); return m_UserName; }

	// Dynamic
	virtual ulong GetNumberOfThreads() const			{ QReadLocker Locker(&m_Mutex); return m_NumberOfThreads; }
	virtual ulong GetNumberOfHandles() const			{ QReadLocker Locker(&m_Mutex); return m_NumberOfHandles; }

	virtual quint64 GetWorkingSetPrivateSize() const	{ QReadLocker Locker(&m_Mutex); return m_WorkingSetPrivateSize; }
	virtual ulong GetPeakNumberOfThreads() const		{ QReadLocker Locker(&m_Mutex); return m_PeakNumberOfThreads; }
	virtual ulong GetHardFaultCount() const				{ QReadLocker Locker(&m_Mutex); return m_HardFaultCount; }

	virtual quint64 GetPeakPrivateBytes() const = 0;
	virtual quint64 GetWorkingSetSize() const = 0;
	virtual quint64 GetPeakWorkingSetSize() const = 0;
	virtual quint64 GetPrivateWorkingSetSize() const = 0;
	virtual quint64 GetSharedWorkingSetSize() const = 0;
	virtual quint64 GetShareableWorkingSetSize() const = 0;
	virtual quint64 GetVirtualSize() const = 0;
	virtual quint64 GetPeakVirtualSize() const = 0;
	virtual quint32 GetPageFaultCount() const = 0;
	virtual quint64 GetPagedPool() const = 0;
	virtual quint64 GetPeakPagedPool() const = 0;
	virtual quint64 GetNonPagedPool() const = 0;
	virtual quint64 GetPeakNonPagedPool() const = 0;
	virtual quint64 GetMinimumWS() const = 0;
	virtual quint64 GetMaximumWS() const = 0;

	virtual STaskStatsEx GetCpuStats() const			{ QReadLocker Locker(&m_StatsMutex); return m_CpuStats; }

	virtual QString GetStatusString() const = 0;

	virtual bool HasDebugger() const = 0;
	virtual STATUS AttachDebugger() = 0;
	virtual STATUS DetachDebugger() = 0;

	virtual bool IsSystemProcess() const = 0;
	virtual bool IsServiceProcess() const = 0;
	virtual bool IsUserProcess() const = 0;
	virtual bool IsElevated() const = 0;

	virtual SProcStats	GetStats() const				{ QReadLocker Locker(&m_StatsMutex); return m_Stats; }

	virtual CModulePtr GetModuleInfo()					{ QReadLocker Locker(&m_Mutex); return m_pModuleInfo; }

	// Threads
	virtual QMap<quint64, CThreadPtr> GetThreadList()	{ QReadLocker Locker(&m_ThreadMutex); return m_ThreadList; }

	// Handles
	virtual QMap<quint64, CHandlePtr> GetHandleList()	{ QReadLocker Locker(&m_HandleMutex); return m_HandleList; }
	
	// Modules
	virtual QMap<quint64, CModulePtr> GetModleList()	{ QReadLocker Locker(&m_ModuleMutex); return m_ModuleList; }

	// Windows
	virtual QMap<quint64, CWndPtr>	  GetWindowList()	{ QReadLocker Locker(&m_WindowMutex); return m_WindowList; }

	struct SEnvVar
	{
		enum EType
		{
			eSystem,
			eUser,
			eProcess
		};
		SEnvVar() { Type = eProcess; }
		QString Name;
		QString Value;
		EType	Type;
		QString GetTypeName() const { return GetType() + "_" + Name; }
		QString GetType() const 
		{ 
			switch (Type)
			{
				case eSystem:	return tr("System");
				case eUser:		return tr("User");
				case eProcess:	return tr("Process");
			}
			return "";
		}
	};

	virtual QMap<QString, SEnvVar>	GetEnvVariables() const = 0;
	virtual STATUS					DeleteEnvVariable(const QString& Name) = 0;
	virtual STATUS					EditEnvVariable(const QString& Name, const QString& Value) = 0;

	virtual QMap<quint64, CMemoryPtr> GetMemoryMap() const = 0;

signals:
	void			ThreadsUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);
	void			HandlesUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);
	void			ModulesUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);
	void			WindowsUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);

public slots:
	virtual bool	UpdateThreads() = 0;
	virtual bool	UpdateHandles() = 0;
	virtual bool	UpdateModules() = 0;
	virtual bool	UpdateWindows() = 0;

protected:
	// Basic
	quint64							m_ProcessId;
	quint64							m_ParentProcessId;
	QString							m_ProcessName;

	// Parameters
	QString							m_FileName;
	QString							m_CommandLine;
	QString							m_WorkingDirectory;
	
	// Other fields
	QString							m_UserName;

	// Dynamic
	ulong							m_NumberOfThreads;
	ulong							m_NumberOfHandles;

	quint64							m_WorkingSetPrivateSize;
	ulong							m_PeakNumberOfThreads;
	ulong							m_HardFaultCount;


	// I/O stats
	mutable QReadWriteLock			m_StatsMutex;
	SProcStats						m_Stats;
	STaskStatsEx					m_CpuStats;

	// module info
	CModulePtr						m_pModuleInfo;

	// Threads
	mutable QReadWriteLock			m_ThreadMutex;
	QMap<quint64, CThreadPtr>		m_ThreadList;

	// Handles
	mutable QReadWriteLock			m_HandleMutex;
	QMap<quint64, CHandlePtr>		m_HandleList;

	// Modules
	mutable QReadWriteLock			m_ModuleMutex;
	QMap<quint64, CModulePtr>		m_ModuleList;

	// Window
	mutable QReadWriteLock			m_WindowMutex;
	QMap<quint64, CWndPtr>			m_WindowList;

};

typedef QSharedPointer<CProcessInfo> CProcessPtr;
typedef QWeakPointer<CProcessInfo> CProcessRef;