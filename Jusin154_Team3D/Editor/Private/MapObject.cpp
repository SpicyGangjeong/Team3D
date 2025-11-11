#include "pch.h"
#include "MapObject.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "Layer.h"
#include "Terrain.h"
#include "VIBuffer_Terrain.h"

CMapObject::CMapObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject(pDevice, pContext)
{
}

CMapObject::CMapObject(const CMapObject& Prototype)
	: CPartObject(Prototype)
{
}



void CMapObject::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapObject::Update(_float fTimeDelta)
{

}

void CMapObject::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if(m_bSelected)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		m_pTransformCom->Set_Scale(m_vScale);
		m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	}
#endif 

	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this, vPos, 20.f);
}

HRESULT CMapObject::Render()
{
	return S_OK;
}

_wstring CMapObject::Get_PrototypeTag(_uint iLodIndex)
{
	return _wstring();
}

HRESULT CMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#ifdef _DEBUG
	m_bSelected = false;
	m_vPosition = _float3(0.f, 0.f, 0.f);
	m_vRotation = _float3(0.f, 0.f, 0.f);
	m_vScale = _float3(1.f, 1.f, 1.f);
#endif // _DEBUG

	return S_OK;
}

_wstring CMapObject::Get_PrototypeTag(_uint iLodIndex)
{
	return _wstring();
}

	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

#ifdef _DEBUG
	m_bSelected = false;
	m_vPosition = _float3(0.f, 0.f, 0.f);
	m_vRotation = _float3(0.f, 0.f, 0.f);
	m_vScale = _float3(1.f, 1.f, 1.f);
#endif // _DEBUG

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
	//	return E_FAIL;
	//}
HRESULT CMapObject::Ready_Components()
{
	__super::Ready_Components(nullptr);

	return S_OK;
}

HRESULT CMapObject::Bind_ShaderResources()
{
	return S_OK;
}

	return pInstance;
}

CGameObject* CMapObject::Clone(void* pArg, CGameObject* pOwner)
{
	CMapObject* pInstance = new CMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();
}

void CMapObject::Describe_Entity()
{
}