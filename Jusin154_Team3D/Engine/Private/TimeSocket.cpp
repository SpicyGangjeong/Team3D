#include "pch.h"
#include "TimeSocket.h"
#include "GameObject.h"

CTimeSocket::CTimeSocket()
{
}

_bool CTimeSocket::Trigger(_float fRatio)
{
	_bool bResult = false;
	if (m_Contents.fRatio <= fRatio) {
		bResult = true;
		m_Contents.funcEvent(*this);
	}

	return bResult;
}
HRESULT CTimeSocket::Initialize(void* pArg)
{
    SOCKETCONTENTS* pDesc = static_cast<SOCKETCONTENTS*>(pArg);

    m_Contents.fRatio = pDesc->fRatio;
    m_Contents.bTriggerred = pDesc->bTriggerred;
    m_Contents.pEventTarget = pDesc->pEventTarget;
    m_Contents.strEventName = pDesc->strEventName;
    m_Contents.wstrLayerName = pDesc->wstrLayerName;
    m_Contents.wstrKeyName = pDesc->wstrKeyName;
    m_Contents.eTypeParam = pDesc->eTypeParam;
    m_Contents.eTypeFunc = pDesc->eTypeFunc;
    m_Contents.vFlags = pDesc->vFlags;
    m_Contents.vParam_10 = pDesc->vParam_10;
    m_Contents.vParam_11 = pDesc->vParam_11;
    m_Contents.funcEvent = pDesc->funcEvent;
    m_Contents.vParam_12 = pDesc->vParam_12;

    switch (m_Contents.eTypeParam)
    {
    case TIMESOCKET_PARAM::TARGET:
        m_Contents.pOtherTarget = pDesc->pOtherTarget;
        break;
    case TIMESOCKET_PARAM::TRANSFORM:
        m_Contents.pxTransform = pDesc->pxTransform;
        break;
    case TIMESOCKET_PARAM::NOT_USE:
        m_Contents.pOtherTarget = nullptr;
        break;
    default:
        m_Contents.pOtherTarget = nullptr;
        break;
    }

    SAFE_ADDREF(m_Contents.pEventTarget);
    if (m_Contents.eTypeParam == TIMESOCKET_PARAM::TARGET){
        SAFE_ADDREF(m_Contents.pOtherTarget);
    }

    return S_OK;
}

void CTimeSocket::Finalize()
{
	switch (m_Contents.eTypeParam)
	{
	case TIMESOCKET_PARAM::TARGET:
		SAFE_RELEASE(m_Contents.pOtherTarget);
		break;
	default:
		break;
	}
	SAFE_RELEASE(m_Contents.pEventTarget);
}

CTimeSocket* CTimeSocket::Create(void* pArg)
{
	CTimeSocket* pInstance = new CTimeSocket();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTimeSocket");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTimeSocket::Free()
{
	__super::Free();

	Finalize();
}
