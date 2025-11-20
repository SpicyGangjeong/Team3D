#include "pch.h"

#include "ThreadHolder.h"

CThreadHolder::CThreadHolder()
{
}

CThreadHolder::~CThreadHolder()
{
}

void CThreadHolder::WorkerThread()
{
    while (true)
    {
        function<void()> job = {};
        unique_lock<mutex> lock(m_mtxJobQueue);
        {
            m_cvJobQueue.wait(lock, [this]() {
                return !this->m_JobQueue.empty() || m_bStop_All;
                });
            if (m_bStop_All && this->m_JobQueue.empty()) {
                return;
            }

            function<void()> job = move(m_JobQueue.front());
            m_JobQueue.pop();
        }
        lock.unlock();

        job();
    }
}

void CThreadHolder::EnqueueJob(function<void()> job)
{
    if (m_bStop_All) {
        throw runtime_error("ThreadPool 사용 중지됨");
    }

    lock_guard<mutex> lock(m_mtxJobQueue);
    m_JobQueue.push(move(job));

    m_cvJobQueue.notify_one();
}

HRESULT CThreadHolder::Initialize(_uint iSize)
{
    m_bStop_All = false;
    m_iNumThreads = iSize;
    m_Workers.reserve(iSize);
    for (_uint i = 0; i < m_iNumThreads; ++i) {
        m_Workers.emplace_back([this]() {this->WorkerThread(); });

    }

    return S_OK;
}

CThreadHolder* CThreadHolder::Create(_uint iSize)
{
    CThreadHolder* pInstance = new CThreadHolder();
    if (FAILED(pInstance->Initialize(iSize))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CThreadHolder::Free()
{
    __super::Free();
}

void CThreadHolder::Describe_Entity()
{
}
