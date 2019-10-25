#pragma once
#include <qobject.h>
#include "AbstractInfo.h"
#include "../Common/FlexError.h"

class CHandleInfo: public CAbstractInfoEx
{
	Q_OBJECT

public:
	CHandleInfo(QObject *parent = nullptr);
	virtual ~CHandleInfo();

	virtual quint64 GetHandleId() const				{ QReadLocker Locker(&m_Mutex); return m_HandleId; }
	virtual quint64 GetProcessId()	const			{ QReadLocker Locker(&m_Mutex); return m_ProcessId; }

	virtual quint32 GetTypeIndex() const = 0;
	virtual QString GetTypeName() const = 0;
	virtual QString GetTypeString() const = 0;
	virtual QString GetFileName() const				{ QReadLocker Locker(&m_Mutex); return m_FileName; }
	virtual quint64 GetPosition() const				{ QReadLocker Locker(&m_Mutex); return m_Position; }
	virtual quint64 GetSize()	const				{ QReadLocker Locker(&m_Mutex); return m_Size; }
	virtual quint32 GetGrantedAccess() const = 0;
	virtual QString GetGrantedAccessString() const = 0;

	virtual QSharedPointer<QObject>	GetProcess() const;
	//virtual QSharedPointer<QObject>	GetProcess() const { QReadLocker Locker(&m_Mutex); return m_pProcess; }
	virtual void SetProcess(QSharedPointer<QObject> pProcess) { QWriteLocker Locker(&m_Mutex); m_pProcess = pProcess; }

	virtual STATUS		Close(bool bForce = false) = 0;

protected:
	quint64				m_HandleId;
	quint64				m_ProcessId;

	QString				m_FileName;
	quint64				m_Position;
	quint64				m_Size;

	QSharedPointer<QObject>	m_pProcess;
};

typedef QSharedPointer<CHandleInfo> CHandlePtr;
typedef QWeakPointer<CHandleInfo> CHandleRef;