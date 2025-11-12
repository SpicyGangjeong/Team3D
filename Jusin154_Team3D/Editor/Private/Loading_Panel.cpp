#include "pch.h"
#include "Loading_Panel.h"
#include "GameInstance.h"
#include "LoadingWidget.h"
#include "LoadingWidget_Flame.h"

CLoading_Panel::CLoading_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CLoading_Panel::CLoading_Panel(const CLoading_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CLoading_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoading_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 1200.f;
	Desc.fY = 900.f;
	Desc.fSizeX = 1515.f;
	Desc.fSizeY = 300.f;

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

void CLoading_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CLoading_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CLoading_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CLoading_Panel::Render()
{
	return S_OK;
}

_vector CLoading_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CLoading_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CLoading_Panel::Ready_Components(void* pArg)
{

	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLoadingWidget>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CLoadingWidget**>(&m_pLoadingWidget))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("LoadingWidget"), m_pLoadingWidget);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLoadingWidget_Flame>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CLoadingWidget_Flame**>(&m_pLoadingWidget_Flame))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("LoadingWidget_Flame"), m_pLoadingWidget_Flame);

	return S_OK;
}

CLoading_Panel* CLoading_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoading_Panel* pInstance = new CLoading_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoading_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CLoading_Panel* pInstance = new CLoading_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoading_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLoading_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CLoading_Panel::Describe_Entity()
{
}
