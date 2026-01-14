#include "pch.h"
#include "CamPosition_WorldLook.h"
#include "GameInstance.h"

CCamPosition_WorldLook::CCamPosition_WorldLook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamPosition(pDevice, pContext)
{
}
CCamPosition_WorldLook::CCamPosition_WorldLook(const CCamPosition_WorldLook& rhs)
	: CCamPosition(rhs)
{
}
HRESULT CCamPosition_WorldLook::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamPosition_WorldLook::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	CameraWorldLook_DESC* pDesc = static_cast<CameraWorldLook_DESC*>(pArg);
	Set_Position(pDesc->vPosition);
#ifdef _DEBUG
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 1.f, 12, false, false));
	m_Batch = make_unique<PrimitiveBatch<VertexPositionColor>>(m_pContext);
#endif // _DEBUG
	return S_OK;
}

void CCamPosition_WorldLook::Priority_Update(_float fTimeDelta)
{
}

void CCamPosition_WorldLook::Update(_float fTimeDelta)
{
}

void CCamPosition_WorldLook::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
#endif // _DEBUG
}

HRESULT CCamPosition_WorldLook::Render()
{
#ifdef _DEBUG
	m_Batch->Begin();
	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x0f000f, 1.f);
	m_pSubShape->Draw(m_pTransformCom->Get_XMWorldMatrix(), ViewMatrix, ProjMatrix, vColor, nullptr, true);
	m_Batch->End();
#endif // _DEBUG
	return S_OK;
}

void CCamPosition_WorldLook::Set_Position(PSX::PxVec3 vPosition)
{
	Set_Position(_float3(vPosition.x, vPosition.y, vPosition.z));
}

void CCamPosition_WorldLook::Set_Position(_float3 vPosition)
{
	Set_Position(XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
}

void CCamPosition_WorldLook::Set_Position(_float4 vPosition)
{
	Set_Position(XMLoadFloat4(&vPosition));
}

void CCamPosition_WorldLook::Set_Position(_fvector vPosition)
{
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(vPosition, 1.f));
}

_vector CCamPosition_WorldLook::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CCamPosition_WorldLook::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC DescTransfrom = {};
	DescTransfrom.fRadius = 30.f;
	DescTransfrom.fRotationPerSec = 0.f;
	DescTransfrom.fSpeedPerSec = 0.f;
	if (FAILED(__super::Ready_Components(&DescTransfrom))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CCamPosition_WorldLook::Bind_ShaderResources()
{
	return S_OK;
}

CCamPosition_WorldLook* CCamPosition_WorldLook::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamPosition_WorldLook* pInstance = new CCamPosition_WorldLook(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamPosition_WorldLook");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CCamPosition_WorldLook* CCamPosition_WorldLook::Clone(void* pArg, class CGameObject* pOWner)
{
	CCamPosition_WorldLook* pInstance = new CCamPosition_WorldLook(*this);
	pInstance->m_pOwner = pOWner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamPosition_WorldLook");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCamPosition_WorldLook::Free()
{
	__super::Free();
}
#ifdef _DEBUG

void CCamPosition_WorldLook::Describe_Entity()
{
}

#endif // _DEBUG
