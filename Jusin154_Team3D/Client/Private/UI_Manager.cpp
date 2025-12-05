#include "pch.h"
#include "UI_Manager.h"
#include "GameInstance.h"
#include "GamePlay_Canvas.h"
#include "Spell_Canvas.h"
#include "InfoInstance.h"

CUI_Manager::CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject(pDevice, pContext)
{
}

CUI_Manager::CUI_Manager(const CUI_Manager& rhs)
	:CUIObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CUI_Manager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Manager::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	m_bCanvas_Change = false;
	m_eType = UI_STATE::GAMEPLAYER;
	m_pInfoInstance->Set_UISTATE(m_eType);
	m_pInfoInstance->Add_Event(TEXT("Canvas_Change"), [this](void* p) {this->Canvas_Change(*reinterpret_cast<UI_STATE*>(p)); });
	return S_OK;
}

void CUI_Manager::Canvas_Change(UI_STATE eType)
{
	m_eType = eType;

	switch (eType)
	{
	case UI_STATE::GAMEPLAYER:
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(true);
		static_cast<CCanvasObject*>(m_pSpell_Canvas)->Visible(false);
		break;

	case UI_STATE::SPELL:
		static_cast<CCanvasObject*>(m_pSpell_Canvas)->Visible(true);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(false);
		break;

	case UI_STATE::POTION:
		break;

	case UI_STATE::INVENTORY:
		break;

	default:
		return;
	}
}

void CUI_Manager::Clear_Canvas()
{
}

CGameObject* CUI_Manager::Get_Canvas(const wstring& Name)
{
	return Find_Canvas(Name);
}

_int CUI_Manager::Canvas_Count()
{
	return m_iCanvas_Count;
}

const vector<wstring> CUI_Manager::Canvas_Name()
{
	return m_CanvasNames;
}

void CUI_Manager::Add_Manager_Event(_wstring Name, function<void(void*)> Event)
{
	m_Event_map.emplace(Name, Event);
}

void CUI_Manager::Priority_Update(_float fTimeDelta)
{

}

void CUI_Manager::Update(_float fTimeDelta)
{

}

void CUI_Manager::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Manager::Render()
{
	return S_OK;
}

_vector CUI_Manager::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CUI_Manager::Bind_ShaderResources()
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Manager::Ready_Components(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGamePlay_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CGamePlay_Canvas**>(&m_pGamePlay_Canves)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("GamePlay_Canvas"), m_pGamePlay_Canves);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Canvas**>(&m_pSpell_Canvas)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Spell_Canvas"), m_pSpell_Canvas);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMouse_Cursor>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMouse_Cursor**>(&m_pMouse_Cursor)))) {
	//	return E_FAIL;
	//}
	//Add_Canvas(TEXT("Mouse_Cursor"), m_pMouse_Cursor);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCameraLockOn>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CCameraLockOn**>(&m_pCamera_LockOn)))) {
	//	return E_FAIL;
	//}
	//Add_Canvas(TEXT("Camera_LockOn"), m_pCamera_LockOn);

	return S_OK;
}

void CUI_Manager::Add_Canvas(_wstring Name, CGameObject* pCanvas)
{
	if (pCanvas == nullptr)
	{
		return;
	}

	if (Find_Canvas(Name) != nullptr)
	{
		return;
	}
	m_Canvases.push_back(pCanvas);
	m_CanvasNames.push_back(Name);
	m_Canvas_map.emplace(Name, pCanvas);
	m_iCanvas_Count++;
}

CGameObject* CUI_Manager::Find_Canvas(const _wstring& Name)
{
	auto iter = m_Canvas_map.find(Name);

	if (iter == m_Canvas_map.end())
		return nullptr;

	return iter->second;
}

CUI_Manager* CUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Manager* pInstance = new CUI_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Manager::Clone(void* pArg, CGameObject* pOwner)
{
	CUI_Manager* pInstance = new CUI_Manager(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}
#ifdef _DEBUG

void CUI_Manager::Describe_Entity()
{
}

#endif // _DEBUG
