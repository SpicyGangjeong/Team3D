#include "pch.h"
#include "TriggerBox.h"

CTriggerBox::CTriggerBox():
    m_pGameInstance(CGameInstance::GetInstance())
{
    SAFE_ADDREF(m_pGameInstance);
}

HRESULT CTriggerBox::TryScanArea(_float fTimeDelta)
{
    m_vScanTimer.x += fTimeDelta;
    if (m_vScanTimer.x > m_vScanTimer.y) {
        m_vScanTimer.x = m_vScanTimer.y;
		return Scan();
    }
    return E_FAIL;
}

HRESULT CTriggerBox::Initialize(TRIGGERBOX_DESC* pDesc)
{
    m_fRadius = pDesc->vPosition_Radius.w;
    m_vPosition = { pDesc->vPosition_Radius.x, pDesc->vPosition_Radius.y, pDesc->vPosition_Radius.z, 1.f };
    return S_OK;
}

HRESULT CTriggerBox::Scan()
{
    PSX::PxSweepBuffer pxBuffer = {};
	_bool bHit = m_pGameInstance->SphereCast(m_fRadius, XMLoadFloat4(&m_vPosition), XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.1f,
		PSX::PxHitFlag::ePOSITION, PSX::PxQueryFlag::eDYNAMIC, pxBuffer);

	const PSX::PxSweepHit & hit = pxBuffer.block;
	PSX::PxRigidActor* pActor = hit.actor;
	if (bHit) {
		if (nullptr != pActor && nullptr != pActor->userData)
		{
			PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
			switch (pUserData->eKind)
			{
			case PHYSX_KIND::BODY_DYNAMIC:
			{
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::PLAYER:
				{
					return S_OK;
				}break;
				default:
					break;
				} 
			}break;
			case PHYSX_KIND::CCTActor:
			{
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::PLAYER: {
					return S_OK;
				} break;
				default:
					break;
				}
			} break;
			default:
				break;
			}
		}
	}
    return E_FAIL;
}

CTriggerBox* CTriggerBox::Create(TRIGGERBOX_DESC* Desc)
{
    CTriggerBox* pInstance = new CTriggerBox();
    
    if (FAILED(pInstance->Initialize(Desc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CTriggerBox::Free()
{
    __super::Free();
    SAFE_RELEASE(m_pGameInstance);
}
