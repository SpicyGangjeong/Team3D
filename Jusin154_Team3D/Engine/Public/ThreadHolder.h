#pragma once

#include "Base.h"

NS_BEGIN(Engine)


class ENGINE_DLL CThreadHolder final : public CBase
{
private:
	CThreadHolder();
	~CThreadHolder();
public:
	template <class Function, class... Args>
	future<invoke_result_t<Function, Args...>> EnqueueJob(Function&& func, Args&&... args)
	{
		if (true == m_bStop_All) {
			throw runtime_error("ThreadPool 사용 중지됨");
		}
		using return_type = invoke_result_t<Function, Args...>;

		auto job = make_shared<packaged_task<return_type()>>(bind(forward<Function>(func), forward<Args>(args)...));
		future<return_type> job_result_future = job->get_future();
		{
			lock_guard<mutex> lock(m_mtxJobQueue);
			m_JobQueue.push([job]() { (*job)(); });
		}
		m_cvJobQueue.notify_one();

		return job_result_future;
	}

private:
	_uint						m_iNumThreads = {};
	vector<thread>				m_Workers = {};

	queue<function<void()>>		m_JobQueue = {};
	condition_variable			m_cvJobQueue = {};
	mutex						m_mtxJobQueue = {};

	_bool						m_bStop_All = { true };

private:
	HRESULT Initialize(_uint iSize);
	void WorkerThread();

public:
	static CThreadHolder* Create(_uint iSize);
	virtual void Free() override;
	void Describe_Entity();
};

NS_END
