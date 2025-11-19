#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CThreadHolder final : public CBase
{
private:
	CThreadHolder();
	~CThreadHolder();
public:
	void WorkerThread();
	void EnqueueJob(function<void()> job);

private:
	_uint						m_iNumThreads = {};
	vector<thread>				m_Workers = {};

	queue<function<void()>>		m_JobQueue = {};
	condition_variable			m_cvJobQueue = {};
	mutex						m_mtxJobQueue = {};

	_bool m_bStop_All = { true };

private:
	HRESULT Initialize(_uint iSize);


public:
	static CThreadHolder* Create(_uint iSize);
	virtual void Free() override;
	void Describe_Entity();
};

NS_END
