#include "pch.h"
#include "UI_Manager.h"
#include "GameInstance.h"
#include "GamePlay_Canvas.h"
#include "Spell_Canvas.h"
#include "Quest_Canvas.h"
#include "Dialogue_Canvas.h"
#include "InfoInstance.h"
#include "Mouse_Cursor.h"
#include "CameraLockOn.h"
#include "Damage_Font.h"
#include "Dialogue_Font.h"
#include "SpellLearn_Canvas.h"
#include "Interaction_Key.h"
#include "NPCInteraction.h"
#include "Broom_TargetGate.h"
#include "Player.h"
#include "Layer.h"

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
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = 540.f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_bCanvas_Change = false;
	m_eType = UI_STATE::GAMEPLAYER;
	m_pGameInstance->Toggle_MouseCenter();
	//ShowCursor(false);
	m_fAlpha = 0.f;
	m_fAlphaTime = 1.f;
	m_fSortZ = 0.f;
	m_bActive = false;
	m_pInfoInstance->Set_UISTATE(m_eType);
	m_pInfoInstance->Add_Event(TEXT("Canvas_Change"), [this](void* p) {this->Canvas_Change(*reinterpret_cast<UI_STATE*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("NpcInteract"), [this](void* p) {this->NpcInteract(*reinterpret_cast<_bool*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("UIManagerFadeIn"), [this](void* p) {this->Set_Fade(); });
	m_pInfoInstance->Add_Event(TEXT("RACEREADY"), [this](void* p) {this->Set_Race(*reinterpret_cast<_bool*>(p)); });
	return S_OK;
}

void CUI_Manager::Canvas_Change(UI_STATE eType)
{
	m_eType = eType;

	switch (eType)
	{
	case UI_STATE::GAMEPLAYER:
		m_pMouse_Cursor->Set_Visible(false);
		m_pCamera_LockOn->Set_Visible(false);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(true);
		static_cast<CCanvasObject*>(m_pSpell_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pQuest_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pSpellLearn_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pDialogue_Canvas)->Visible(false);
		static_cast<CGameObject*>(m_pInteraction_Key)->Set_Visible(true);
		static_cast<CGameObject*>(m_pNPCInteraction)->Set_Visible(true);
		break;

	case UI_STATE::SPELL:
		m_pMouse_Cursor->Set_Visible(true);
		m_pCamera_LockOn->Set_Visible(false);
		static_cast<CCanvasObject*>(m_pSpell_Canvas)->Visible(true);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(false);
		break;

	case UI_STATE::QUEST:
		m_pMouse_Cursor->Set_Visible(true);
		m_pCamera_LockOn->Set_Visible(false);
		static_cast<CCanvasObject*>(m_pQuest_Canvas)->Visible(true);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(false);
		break;

	case UI_STATE::INVENTORY:
		break;

	case UI_STATE::SPELLLNEARN:
		m_pMouse_Cursor->Set_Visible(true);
		m_pCamera_LockOn->Set_Visible(false);
		static_cast<CCanvasObject*>(m_pSpellLearn_Canvas)->Visible(true);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(false);
		break;

	case UI_STATE::NPC_INTERACT:
		m_pMouse_Cursor->Set_Visible(false);
		m_pCamera_LockOn->Set_Visible(false);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(false);
		static_cast<CCanvasObject*>(m_pSpell_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pQuest_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pSpellLearn_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pDialogue_Canvas)->Visible(true);
		static_cast<CGameObject*>(m_pInteraction_Key)->Set_Visible(false);
		static_cast<CGameObject*>(m_pNPCInteraction)->Set_Visible(false);
		static_cast<CGameObject*>(m_pBroom_TargetGate)->Set_Visible(false);
		static_cast<CGameObject*>(m_pDamage_Font)->Set_Visible(false);
		break;

	case UI_STATE::LEVELCHANGE:
		m_pMouse_Cursor->Set_Visible(false);
		m_pCamera_LockOn->Set_Visible(false);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(false);
		static_cast<CCanvasObject*>(m_pSpell_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pQuest_Canvas)->Visible(false);
		static_cast<CCanvasObject*>(m_pSpellLearn_Canvas)->Visible(false);
		static_cast<CGameObject*>(m_pInteraction_Key)->Set_Visible(false);
		static_cast<CGameObject*>(m_pNPCInteraction)->Set_Visible(false);
		static_cast<CGameObject*>(m_pBroom_TargetGate)->Set_Visible(false);
		static_cast<CGameObject*>(m_pDamage_Font)->Set_Visible(false);
		break;

	default:
		return;
	}
	m_pInfoInstance->Event_CallBack(TEXT("Player_CanvasChange"), &m_eType);
	m_pGameInstance->Toggle_MouseCenter();
	m_pInfoInstance->Event_CallBack(TEXT("Player_CanvasChange"), &m_eType);
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
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

	if (m_pGameInstance->Key_Down(DIK_T))
	{
		m_fAlphaVelue = 1.f;
		if (m_eType == UI_STATE::GAMEPLAYER)
			Canvas_Change(UI_STATE::SPELL);
		else if (m_eType == UI_STATE::SPELL)
			Canvas_Change(UI_STATE::GAMEPLAYER);
	}
	

	if (m_bRace == false)
	{
		if (m_eType == UI_STATE::GAMEPLAYER || m_eType == UI_STATE::QUEST)
		{
			if (m_pGameInstance->Key_Down(DIK_TAB))
			{
				m_fAlphaVelue = 1.f;
				if (m_bActive == false)
				{
					m_bActive = true;
					m_bAlphaZero = true;
					if (m_eType == UI_STATE::GAMEPLAYER)
					{
						m_eType = UI_STATE::QUEST;

					}
					else if (m_eType == UI_STATE::QUEST)
					{
						m_eType = UI_STATE::GAMEPLAYER;
					}
				}
			}
		}

		if (m_eType == UI_STATE::GAMEPLAYER || m_eType == UI_STATE::SPELLLNEARN)
		{
			if (m_pGameInstance->Key_Down(DIK_ESCAPE))
			{
				m_fAlphaVelue = 1.f;
				if (m_bActive == false)
				{
					m_bActive = true;
					m_bAlphaZero = true;
					if (m_eType == UI_STATE::GAMEPLAYER)
					{
						m_eType = UI_STATE::SPELLLNEARN;
					}
					else if (m_eType == UI_STATE::SPELLLNEARN)
					{
						m_eType = UI_STATE::GAMEPLAYER;
					}
				}
			}
		}
	}

	if (m_eType == UI_STATE::GAMEPLAYER && m_bNPCInteract == true)
	{
		m_fAlphaVelue = 1.f;
		if (m_bCurrentNPCInteract != m_bNPCInteract)
			m_bCurrentNPCInteract = m_bNPCInteract;

		if (m_eType == UI_STATE::GAMEPLAYER)
		{
			Canvas_Change(UI_STATE::NPC_INTERACT);
		}
	}

	if (m_bCurrentNPCInteract == true && m_bNPCInteract == false)
	{
		m_fAlphaVelue = 1.f;
		m_bCurrentNPCInteract = m_bNPCInteract;

		if (m_bActive == false)
		{
			m_bActive = true;
			m_bAlphaZero = true;
			if (m_eType == UI_STATE::NPC_INTERACT)
			{
				m_eType = UI_STATE::GAMEPLAYER;
			}
		}
	}

	if (m_bActive == true && m_bHover == false)
	{
		if (m_fAlpha <= m_fAlphaVelue)
		{
			m_fAlpha += fTimeDelta * m_fAlphaTime;
		}

		if (m_fAlpha > m_fAlphaVelue && m_bAlphaZero == true)
		{
			m_bAlphaZero = false;
			m_fAlpha = 1.f;
			m_bFadeIn = true;
			Canvas_Change(m_eType);
		}
	}

	if (m_bFadeIn == true)
	{
		m_fTime += fTimeDelta;
	}

	if (m_fTime >= 1.f)
	{
		m_bHover = true;
		m_bFadeIn = false;
		m_fTime = 0.f;
	}

	if (m_bActive == true && m_bHover == true)
	{
		if (m_bCgangede == false)
		{
			m_bCgangede = true;
		}

		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			m_bActive = false;
			m_bHover = false;
			m_bCgangede = false;
		}

		if (m_fAlpha > 0.f)
		{
			m_fAlpha -= fTimeDelta * m_fAlphaTime;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_J))
	{
		Change_Map();
	}

	__super::Update(fTimeDelta);
}

void CUI_Manager::Late_Update(_float fTimeDelta)
{
	if (m_bActive == true)
		m_pGameInstance->Add_RenderGroup(RENDER::UI_OVERLAY, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Manager::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::CANVASFADE)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

_vector CUI_Manager::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CUI_Manager::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Manager::Ready_Components(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGamePlay_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CGamePlay_Canvas**>(&m_pGamePlay_Canves)))) {
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UIChange"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("GamePlay_Canvas"), m_pGamePlay_Canves);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Canvas**>(&m_pSpell_Canvas)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Spell_Canvas"), m_pSpell_Canvas);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Canvas**>(&m_pQuest_Canvas)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Quest_Canvas"), m_pQuest_Canvas);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn_Canvas**>(&m_pSpellLearn_Canvas)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("SpellLearn_Canvas"), m_pSpellLearn_Canvas);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDialogue_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CDialogue_Canvas**>(&m_pDialogue_Canvas)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Dialogue_Canvas"), m_pDialogue_Canvas);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMouse_Cursor>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMouse_Cursor**>(&m_pMouse_Cursor)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Mouse_Cursor"), m_pMouse_Cursor);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCameraLockOn>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CCameraLockOn**>(&m_pCamera_LockOn)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Camera_LockOn"), m_pCamera_LockOn);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDamage_Font>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CDamage_Font**>(&m_pDamage_Font)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Camera_LockOn"), m_pDamage_Font);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDialogue_Font>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CDialogue_Font**>(&m_pDialogue_Font)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Dialogue_Font"), m_pDialogue_Font);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInteraction_Key>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CInteraction_Key**>(&m_pInteraction_Key)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("Interaction_Key"), m_pInteraction_Key);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CNPCInteraction>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CNPCInteraction**>(&m_pNPCInteraction)))) {
		return E_FAIL;
	}
	Add_Canvas(TEXT("NPCInteraction"), m_pNPCInteraction);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_TargetGate>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_TargetGate**>(&m_pBroom_TargetGate))))
	{
		return E_FAIL;
	}
	Add_Canvas(TEXT("Broom_TargetGate"), m_pBroom_TargetGate);

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

void CUI_Manager::NpcInteract(_bool bInteract)
{
	m_bNPCInteract = bInteract;
	m_bHover = bInteract;
	m_bActive = bInteract;
	m_bAlphaZero = true;
}

void CUI_Manager::Set_Fade()
{
	m_fAlphaVelue = 1.5f;
	m_bActive = true;
	m_bHover = true;
	m_bCgangede = true;
}

void CUI_Manager::Change_Map()
{
	if (FAILED(m_pInfoInstance->Load_DADA_INT()))
		return;
	
	if (FAILED(m_pInfoInstance->Load_EffectParts("Bon_Fire_Data", "../Bin/Resources/Data/Effect/MapEffect/Bon_Fire")))
		return;

	CLayer* pLayer = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER);
	if (nullptr != pLayer) {
		CPlayer* pPlayer = pLayer->Get_Object<CPlayer>();
		pPlayer->Get_Component<CCharacter_Controller>()->Set_Position(XMVectorSet(1003.f, 5.f, 1005.f, 1.f));
	}

	m_pGameInstance->Setting_Volumetirc(
		3.f,
		0.01f,
		0.11f,
		1.0f,
		0.f
	);
}

void CUI_Manager::Set_Race(_bool bRace)
{
	m_bRace = bRace;
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

	m_Event_map.clear();
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
}
#ifdef _DEBUG

void CUI_Manager::Describe_Entity()
{
	GUI::Begin("실내맵 전환");

	if (GUI::Button("Load_DADA_INT"))
	{
		Change_Map();
	}

	GUI::End();
}

#endif // _DEBUG
