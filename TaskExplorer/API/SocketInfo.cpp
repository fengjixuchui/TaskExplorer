#include "stdafx.h"
#include "SocketInfo.h"

CSocketInfo::CSocketInfo(QObject *parent) : CAbstractInfoEx(parent)
{
	m_HashID = -1;

	m_ProtocolType = 0;;
	m_LocalPort = 0;
	m_RemotePort = 0;
	m_State = 0;
	m_ProcessId = -1;
}

CSocketInfo::~CSocketInfo()
{
}

bool CSocketInfo::Match(quint64 ProcessId, quint32 ProtocolType, const QHostAddress& LocalAddress, quint16 LocalPort, const QHostAddress& RemoteAddress, quint16 RemotePort, EMatchMode Mode)
{
	QReadLocker Locker(&m_Mutex); 

#ifdef _DEBUG
	QString m_LocalAddressStr = m_LocalAddress.toString();
	QString m_RemoteAddressStr = m_RemoteAddress.toString();
	QString LocalAddressStr = LocalAddress.toString();
	QString RemoteAddressStr = RemoteAddress.toString();
#endif

	if (m_ProcessId != ProcessId)
		return false;

	if (m_ProtocolType != ProtocolType)
		return false;

	if ((m_ProtocolType & (NET_TYPE_PROTOCOL_TCP | NET_TYPE_PROTOCOL_UDP)) != 0)
	{
		if (m_LocalPort != LocalPort)
			return false;
	}

	// a socket may be bount to all adapters than it has a local null address
	if (Mode == eStrict || m_LocalAddress != QHostAddress::AnyIPv4)
	{
		if(m_LocalAddress != LocalAddress)
			return false;
	}

	// don't test the remote endpoint if this is a udp socket
	if (Mode == eStrict || (m_ProtocolType & NET_TYPE_PROTOCOL_TCP) != 0)
	{
		if ((m_ProtocolType & (NET_TYPE_PROTOCOL_TCP | NET_TYPE_PROTOCOL_UDP)) != 0)
		{
			if (m_RemotePort != RemotePort)
				return false;
		}

		if (m_RemoteAddress != RemoteAddress)
			return false;
	}

	return true;
}

quint64 CSocketInfo::MkHash(quint64 ProcessId, quint32 ProtocolType, const QHostAddress& LocalAddress, quint16 LocalPort, const QHostAddress& RemoteAddress, quint16 RemotePort)
{
	if ((ProtocolType & NET_TYPE_PROTOCOL_UDP) != 0)
		RemotePort = 0;

	quint64 HashID = ((quint64)LocalPort << 0) | ((quint64)RemotePort << 16) | (quint64)(((quint32*)&ProcessId)[0] ^ ((quint32*)&ProcessId)[1]) << 32;

	return HashID;
}

void CSocketInfo::UpdateStats()
{
	QWriteLocker Locker(&m_StatsMutex);
	m_Stats.UpdateStats();
}

QString CSocketInfo::GetProtocolString()
{
	QReadLocker Locker(&m_Mutex);
    switch (m_ProtocolType)
    {
		case NET_TYPE_IPV4_TCP:	return tr("TCP");
		case NET_TYPE_IPV6_TCP:	return tr("TCP6");
		case NET_TYPE_IPV4_UDP:	return tr("UDP");
		case NET_TYPE_IPV6_UDP:	return tr("UDP6");
		default:						return tr("Unknown");
    }
}

#ifndef MIB_TCP_STATE
typedef enum {
    MIB_TCP_STATE_CLOSED     =  1,
    MIB_TCP_STATE_LISTEN     =  2,
    MIB_TCP_STATE_SYN_SENT   =  3,
    MIB_TCP_STATE_SYN_RCVD   =  4,
    MIB_TCP_STATE_ESTAB      =  5,
    MIB_TCP_STATE_FIN_WAIT1  =  6,
    MIB_TCP_STATE_FIN_WAIT2  =  7,
    MIB_TCP_STATE_CLOSE_WAIT =  8,
    MIB_TCP_STATE_CLOSING    =  9,
    MIB_TCP_STATE_LAST_ACK   = 10,
    MIB_TCP_STATE_TIME_WAIT  = 11,
    MIB_TCP_STATE_DELETE_TCB = 12,
    //
    // Extra TCP states not defined in the MIB
    //
    MIB_TCP_STATE_RESERVED      = 100
} MIB_TCP_STATE;
#endif

void CSocketInfo::SetClosed()
{ 
	QWriteLocker Locker(&m_Mutex); 
	if(m_State != -1)
		m_State = MIB_TCP_STATE_CLOSED; 
	Locker.unlock();

	QWriteLocker StatsLocker(&m_StatsMutex);
	m_Stats.Net.ReceiveDelta.Delta = 0;
	m_Stats.Net.SendDelta.Delta = 0;
	m_Stats.Net.ReceiveRawDelta.Delta = 0;
	m_Stats.Net.SendRawDelta.Delta = 0;
	m_Stats.Net.ReceiveRate.Clear();
	m_Stats.Net.SendRate.Clear();
}

QString CSocketInfo::GetStateString()
{
	QReadLocker Locker(&m_Mutex);

	if ((m_ProtocolType & NET_TYPE_PROTOCOL_TCP) == 0)
	{
		if(m_State == MIB_TCP_STATE_CLOSED)
			return tr("Closed");
		return tr("Open");
	}

	// all these are TCP states
    switch (m_State)
    {
    case MIB_TCP_STATE_CLOSED:		return tr("Closed");
    case MIB_TCP_STATE_LISTEN:		return tr("Listen");
    case MIB_TCP_STATE_SYN_SENT:	return tr("SYN sent");
    case MIB_TCP_STATE_SYN_RCVD:	return tr("SYN received");
    case MIB_TCP_STATE_ESTAB:		return tr("Established");
    case MIB_TCP_STATE_FIN_WAIT1:	return tr("FIN wait 1");
    case MIB_TCP_STATE_FIN_WAIT2:	return tr("FIN wait 2");
    case MIB_TCP_STATE_CLOSE_WAIT:	return tr("Close wait");
    case MIB_TCP_STATE_CLOSING:		return tr("Closing");
    case MIB_TCP_STATE_LAST_ACK:	return tr("Last ACK");
    case MIB_TCP_STATE_TIME_WAIT:	return tr("Time wait");
    case MIB_TCP_STATE_DELETE_TCB:	return tr("Delete TCB");
	case -1:						return tr("Blocked");
    default:						return tr("Unknown");
    }
}
