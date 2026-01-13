#include "pch.h"
#include "TriggerBox.h"
#include "Layer.h"
#include "Player.h"

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
	if (false == m_bRender) {
		m_bRender = true;
		m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
	}
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
#ifdef _DEBUG
	m_Batch->Begin();

	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0xff0f0f, CMyTools::Saturate(m_vScanTimer.x / m_vScanTimer.y + 0.1f));
	m_pSubShape->Draw(m_pTransformCom->Get_XMWorldMatrix(), ViewMatrix, ProjMatrix, vColor, nullptr, true);

	m_Batch->End();
	m_bRender = false;
#endif // _DEBUG

	return S_OK;
}

HRESULT CTriggerBox::Initialize(TRIGGERBOX_DESC* pDesc)
{
	CTransform::TRANSFORM_DESC Desc{};
	Desc.fRadius = pDesc->vPosition_Radius.w;
	Desc.fRotationPerSec = 0;
	Desc.fSpeedPerSec = 0;
	if (FAILED(__super::Ready_Components(&Desc))) {
		return E_FAIL;
	}
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat4(&pDesc->vPosition_Radius), 1.f));
#ifdef _DEBUG

	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 1.f, 12, false, false));
	m_Batch = make_unique<PrimitiveBatch<VertexPositionColor>>(m_pContext);

#endif // _DEBUG
	_float3 vScale = { Desc.fRadius, Desc.fRadius, Desc.fRadius };
	m_pTransformCom->Set_Scale(vScale);
	m_vScanTimer.y += m_pGameInstance->Real_Random_Float(0.f, 0.2f);

    return S_OK;
}

HRESULT CTriggerBox::Scan()
{
	_float fRadius = m_pTransformCom->Get_Radius();

	PSX::PxSweepBufferN<32> pxBuffer = {};
	_bool bHit = m_pGameInstance->SphereCast(fRadius, m_pTransformCom->Get_State(STATE::POSITION), XMVectorSet(0.f, 1.f, 0.f, 0.f), 1.1f,
		PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eDYNAMIC, pxBuffer);

	const PSX::PxSweepHit & hit = pxBuffer.block;
	PSX::PxRigidActor* pActor = hit.actor;
	_uint iHitCount = pxBuffer.getNbAnyHits();
	HRESULT hr = E_FAIL;
	if (bHit) {
		hr = CheckPlayerHit(pActor);
		if (FAILED(hr)) {
			for (_uint i = 0; i < iHitCount; ++i) {
				PSX::PxSweepHit* pHit = &pxBuffer.touches[i];
				hr = CheckPlayerHit(pHit->actor);
				if (SUCCEEDED(hr)) {
					break;
				}
			}
		}
	}
    return hr;
}

HRESULT CTriggerBox::CheckPlayerHit(PSX::PxActor* pActor)
{
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
#ifdef _DEBUG

void CTriggerBox::Describe_Entity()
{
}

#endif // _DEBUG
