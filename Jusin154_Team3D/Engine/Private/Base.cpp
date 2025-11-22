#include "pch.h"
#include "Base.h"


CBase::CBase()
{
}

_uint CBase::AddRef()
{
	return m_iRefCnt.fetch_add(1, memory_order_relaxed) + 1; // +1 한 이후의 갯수를 출력
}

_uint CBase::Release()
{
    for (;;)
    {
        _uint iOldRefCount = m_iRefCnt.load(memory_order_acquire);

        if (iOldRefCount == 0u) // -1 할 때 갯수가 0이면 소멸
        {
            Free();

            delete this;

            return 0u;
        }

        _uint iNewRefCount = iOldRefCount - 1u;

        if (m_iRefCnt.compare_exchange_weak(
            iOldRefCount,
            iNewRefCount,
            memory_order_acq_rel,
            memory_order_acquire))
        {
            return iNewRefCount; // -1 한 이후의 갯수를 출력함을 보장
        }
    }

	if (0 == m_iRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else {
		return m_iRefCnt--;
	}
}

void CBase::Free()
{
}
