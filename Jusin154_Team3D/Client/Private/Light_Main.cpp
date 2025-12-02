#include "pch.h"
#include "Light_Main.h"
#include "GameInstance.h"

CLight_Main::CLight_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLight_Main::CLight_Main(const CLight_Main& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CLight_Main::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight_Main::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::LOOK, XMVectorSet(1.f, -1.f, 1.f, 0.f));

	return S_OK;
}

void CLight_Main::Priority_Update(_float fTimeDelta)
{

}

void CLight_Main::Update(_float fTimeDelta)
{

}

void CLight_Main::Late_Update(_float fTimeDelta)
{
}

HRESULT CLight_Main::Render()
{
	return S_OK;
}

HRESULT CLight_Main::Ready_Components()
{
	__super::Ready_Components(nullptr);

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	/*LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 0.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);*/
	LightDesc.vDiffuse = _float4(0.3f, 0.3f, 0.1f, 0.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.5f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.pDirection = m_pTransformCom->Get_StatePtr(STATE::LOOK);
	LightDesc.iLevel = NEXT_LEVEL;

	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}
	m_pGameInstance->Add_Light(NEXT_LEVEL, m_pLightCom);


	_float4 vColor = _float4(0.1f, 0.1f, 0.15f, 0.5f);
	m_pGameInstance->Set_FogColor(vColor);

	return S_OK;
}

HRESULT CLight_Main::Bind_ShaderResources()
{
	return S_OK;
}

CLight_Main* CLight_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Main* pInstance = new CLight_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLight_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLight_Main::Clone(void* pArg, CGameObject* pOwner)
{
	CLight_Main* pInstance = new CLight_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLight_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Main::Free()
{
	__super::Free();

	Safe_Release(m_pLightCom);
}
#ifdef _DEBUG

void CLight_Main::Describe_Entity()
{
}

#endif // _DEBUG
