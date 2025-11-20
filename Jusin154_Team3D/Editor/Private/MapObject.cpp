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

	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapObject::Render()
{
	return S_OK;
}

_wstring CMapObject::Get_PrototypeTag(_uint iLodIndex)
{
	return _wstring();
}

_uint CMapObject::Get_LodLevel()
{
	 return m_iMaxLodLevel;
}

HRESULT CMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	MAPOBJECT_LOD_DESC* pDesc = static_cast<MAPOBJECT_LOD_DESC*>(pArg);

	PARTOBJECT_DESC Desc = {};
	Desc.pParentTransform = pDesc->pParentTransform;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

#ifdef _DEBUG
	m_bSelected = false;
	m_vPosition = _float3(0.f, 0.f, 0.f);
	m_vRotation = _float3(0.f, 0.f, 0.f);
	m_vScale = _float3(1.f, 1.f, 1.f);
#endif // _DEBUG

	return S_OK;
}

HRESULT CMapObject::Ready_Components()
{
	__super::Ready_Components(nullptr);

	return S_OK;
}

HRESULT CMapObject::Bind_ShaderResources()
{
	return S_OK;
}

void CMapObject::Free()
{
	__super::Free();
}

void CMapObject::Describe_Entity()
{
}
