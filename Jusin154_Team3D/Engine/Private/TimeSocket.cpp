#include "pch.h"
#include "TimeSocket.h"
#include "GameObject.h"

CTimeSocket::CTimeSocket()
{
}

_bool CTimeSocket::Trigger(_float fRatio)
{
	_bool bResult = false;
	if (m_fRatio <= fRatio) {
		bResult = true;
		m_funcEvent(*this);

		Finalize();
	}

	return bResult;
}

HRESULT CTimeSocket::Initialize(CGameObject* pTarget, _string& strTag)
{
	m_pEventTarget = pTarget;
	SAFE_ADDREF(m_pEventTarget);
	m_strEventName = strTag;
	m_eTypeParam = TIMESOCKET_PARAM::END;
	return S_OK;
}

HRESULT CTimeSocket::Initialize(CGameObject* pTarget, _string& strTag, PSX::PxTransform& pxTransform)
{
	m_pEventTarget = pTarget;
	SAFE_ADDREF(m_pEventTarget);
	m_strEventName = strTag;
	m_pxTransform = pxTransform;
	m_eTypeParam = TIMESOCKET_PARAM::TRANSFORM;
	return S_OK;
}

HRESULT CTimeSocket::Initialize(CGameObject* pTarget, _string& strTag, CGameObject* pOther)
{
	m_pEventTarget = pTarget;
	SAFE_ADDREF(m_pEventTarget);
	m_strEventName = strTag;
	m_pOtherTarget = pOther;
	SAFE_ADDREF(m_pOtherTarget);
	m_eTypeParam = TIMESOCKET_PARAM::TARGET;
	return S_OK;
}

void CTimeSocket::Finalize()
{
	switch (m_eTypeParam)
	{
	case TIMESOCKET_PARAM::TARGET:
		SAFE_RELEASE(m_pOtherTarget);
		break;
	case TIMESOCKET_PARAM::TRANSFORM:
		m_pxTransform = {};
		break;
	default:
		break;
	}
	SAFE_RELEASE(m_pEventTarget);
}

CTimeSocket* CTimeSocket::Create()
{
	return new CTimeSocket();
}

void CTimeSocket::Free()
{
	__super::Free();

	Finalize();
}
