#include "pch.h"
#include "TriggerBox.h"

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTriggerBox::TryScanArea(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
#endif // _DEBUG
    m_vScanTimer.x += fTimeDelta;
    if (m_vScanTimer.x > m_vScanTimer.y) {
        m_vScanTimer.x = 0.f;
		return Scan();
    }
    return E_FAIL;
}

HRESULT CTriggerBox::Render()
{
	m_Batch->Begin();
	
	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0xff0f0f, CMyTools::Saturate(m_vScanTimer.x / m_vScanTimer.y + 0.1f));
	m_pSubShape->Draw(XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z), ViewMatrix, ProjMatrix, vColor, nullptr, true);

	m_Batch->End();
	return S_OK;
}

#ifdef _DEBUG
void CTriggerBox::Create_DebugShape(ID3D11DeviceContext* pContext)
{
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, m_fRadius, 12, false, false));
	m_Batch = make_unique<PrimitiveBatch<VertexPositionColor>>(m_pContext);
}
#endif // _DEBUG

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

CTriggerBox* CTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TRIGGERBOX_DESC* Desc)
{
	CTriggerBox* pInstance = new CTriggerBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize(Desc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CTriggerBox::Free()
{
    __super::Free();
}

HRESULT CTriggerBox::Bind_ShaderResources()
{
	return E_NOTIMPL;
}

CGameObject* CTriggerBox::Clone(void* pArg, CGameObject* pOwner)
{
	return nullptr;
}

void CTriggerBox::Describe_Entity()
{
}
