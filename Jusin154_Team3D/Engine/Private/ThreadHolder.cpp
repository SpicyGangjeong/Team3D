#include "pch.h"

#include "ThreadHolder.h"

CThreadHolder::CThreadHolder()
{
}

CThreadHolder::~CThreadHolder()
{
}

HRESULT CThreadHolder::Initialize(_uint iSize)
{
    m_bStop_All = false;
    m_iNumThreads = iSize;
    m_Workers.reserve(iSize);
    for (_uint i = 0; i < m_iNumThreads; ++i) {
        m_Workers.emplace_back([this]() { this->WorkerThread(); });

    }

    return S_OK;
}

void CThreadHolder::WorkerThread()
{
    while (true)
    {
        function<void()> job = {};
        unique_lock<mutex> lock(m_mtxJobQueue);
        {
            m_cvJobQueue.wait(lock, [this]() { return !this->m_JobQueue.empty() || m_bStop_All; });
            if (m_bStop_All && this->m_JobQueue.empty()) {
                return;
            }

            job = move(m_JobQueue.front());
            m_JobQueue.pop();
        } lock.unlock();

        job();
    }
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

    {
        unique_lock<mutex> lock(m_mtxJobQueue);
        m_bStop_All = true;
    }

    m_cvJobQueue.notify_all();

    for (auto& thread : m_Workers) {
        thread.join();
    }
}

void CThreadHolder::Describe_Entity()
{
}
