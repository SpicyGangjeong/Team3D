#include "pch.h"
#include "GamePlay_Canvas.h"
#include "GameInstance.h"
#include "Mission_Panel.h"
#include "MiniMap_Panel.h"
#include "Loading_Panel.h"
#include "Action_Panel.h"
#include "Enemy_Panel.h"
//#include "Mouse_Cursor.h"

CGamePlay_Canvas::CGamePlay_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCanvasObject(pDevice, pContext)
{
}

CGamePlay_Canvas::CGamePlay_Canvas(const CGamePlay_Canvas& rhs)
	:CCanvasObject(rhs)
{
}


HRESULT CGamePlay_Canvas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGamePlay_Canvas::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 1920.f;
	Desc.fSizeY = 1080.f;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Panel(pArg)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CGamePlay_Canvas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CGamePlay_Canvas::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CGamePlay_Canvas::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {

	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CGamePlay_Canvas::Render()
{
	return S_OK;
}

_vector CGamePlay_Canvas::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CGamePlay_Canvas::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CGamePlay_Canvas::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CGamePlay_Canvas::Ready_Panel(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLoading_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CLoading_Panel**>(&m_pLoading_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("LoadingPanel"), m_pLoading_Panel);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMiniMap_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMiniMap_Panel**>(&m_pMinimap_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("MinimapPanel"), m_pMinimap_Panel);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMission_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMission_Panel**>(&m_pMission_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("MissionPanel"), m_pMission_Panel);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CAction_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CAction_Panel**>(&m_pAction_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("ActionPanel"), m_pAction_Panel);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEnemy_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CEnemy_Panel**>(&m_pEnemy_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("Enemy_Panel"), m_pEnemy_Panel);


	return S_OK;
}

void CGamePlay_Canvas::Clear_Penel()
{
}

CGamePlay_Canvas* CGamePlay_Canvas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGamePlay_Canvas* pInstance = new CGamePlay_Canvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGamePlay_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CGamePlay_Canvas::Clone(void* pArg, CGameObject* pOwner)
{
	CGamePlay_Canvas* pInstance = new CGamePlay_Canvas(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGamePlay_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGamePlay_Canvas::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CGamePlay_Canvas::Describe_Entity()
{
}
#endif // _DEBUG
