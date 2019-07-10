#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WinSvcWindow.h"
#include "../../API/Windows/WinService.h"	

class CServiceListWidget;

class CWinSvcWindow : public QMainWindow
{
	Q_OBJECT

public:
	CWinSvcWindow(QSharedPointer<CWinService> pService, QWidget *parent = Q_NULLPTR);
	~CWinSvcWindow();

private slots:
	void accept();
	void reject();

	void OnBrowse();
	void OnShowPW(int state);
	void OnPermissions();
	void OnGeneralChanged()				{ m_GeneralChanged = true; }

	void OnBrowseRun();
	void OnRestartOptions();
	void FixReciveryControls();
	void OnRecoveryChanged()			{ m_RecoveryChanged = true; }

	void OnTrigger(QTreeWidgetItem *item, int column);
	void OnNewTrigger();
	void OnEditTrigger();
	void OnDeleteTrigger();


	void OnPreShutdown()				{ m_PreshutdownTimeoutValid = true; }
	void OnSidType()					{ m_SidTypeValid = true; }
	void OnProtectionType()				{ m_LaunchProtectedValid = true; }

	void OnAddPrivilege();
	void OnRemovePrivilege();


protected:
	void closeEvent(QCloseEvent *e);

	QSharedPointer<CWinService>		m_pService;

	bool							m_GeneralChanged;
	bool							m_OldDelayedStart;

	bool							m_RecoveryChanged;
	int								m_NumberOfActions;
	bool							m_EnableFlagCheckBox;
	quint64							m_RebootAfter;
	QString							m_RebootMessage;

	QVector<void*>					m_TriggerInfos;

	bool							m_TriggersChanged;
	int								m_InitialNumberOfTriggers;

	bool							m_PreshutdownTimeoutValid;
	bool							m_RequiredPrivilegesValid;
	bool							m_SidTypeValid;
	bool							m_LaunchProtectedValid;
	int								m_OriginalLaunchProtected;

private:
	void							AddTrigger(void* pInfo);
	void							UpdateTrigger(QTreeWidgetItem* pItem, void* pInfo);
	void							AddPrivilege(const QString& Privilege);
	Ui::WinSvcWindow ui;
	CServiceListWidget* m_pDependencies;
	CServiceListWidget* m_pDependants;
};