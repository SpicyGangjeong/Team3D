#include "pch.h"
#include "Mission_Panel.h"
#include "GameInstance.h"
#include "MissionBanner_Border.h"
#include "MissionBanner_Key.h"

CMission_Panel::CMission_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CMission_Panel::CMission_Panel(const CMission_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CMission_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMission_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};
	
	Desc.fX = 380.f;
	Desc.fY = -580.f;
	Desc.fSizeX = 750.f;
	Desc.fSizeY = 410.f;


	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Element(pArg)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CMission_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CMission_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CMission_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bVisible) {

	}
	__super::Late_Update(fTimeDelta);

}

HRESULT CMission_Panel::Render()
{
	return S_OK;
}

_vector CMission_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMission_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CMission_Panel::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CMission_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMissionBanner_Border>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMissionBanner_Border**>(&m_pMissionBanner_Border))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("MissionBanner_Border"), m_pMissionBanner_Border);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMissionBanner_Key>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMissionBanner_Key**>(&m_pMissionBanner_Key))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("MissionBanner_Key"), m_pMissionBanner_Key);

	return S_OK;
}

CMission_Panel* CMission_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMission_Panel* pInstance = new CMission_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMission_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMission_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CMission_Panel* pInstance = new CMission_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMission_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMission_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CMission_Panel::Describe_Entity()
{
}
