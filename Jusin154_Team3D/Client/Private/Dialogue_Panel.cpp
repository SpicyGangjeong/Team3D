#include "pch.h"
#include "Dialogue_Panel.h"
#include "GameInstance.h"
#include "Dialogue_Choice.h"
#include "InfoInstance.h"
#include "Player.h"
#include "Layer.h"

CDialogue_Panel::CDialogue_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CDialogue_Panel::CDialogue_Panel(const CDialogue_Panel& rhs)
	:CPanelObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CDialogue_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialogue_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 500.f;
	Desc.fY = -625.f;
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

	m_fCanvasAlpha = 1.f;
	m_pInfoInstance->Add_Event(TEXT("TEXTTYPE"), [this](void* p) {this->Setting_Choice(*reinterpret_cast<DIALOGUECHOICEINFO*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("CHOICERESET"), [this](void* p) {this->ReSet_Choice(); });
	Add_Function(TEXT("TEXTTYPECHOICE"), [this](void* p) {this->Choice_Slot(*reinterpret_cast<CHOICEINFO*>(p)); });
	Visible(true);
	return S_OK;
}

void CDialogue_Panel::Setting_Choice(DIALOGUECHOICEINFO Choice)
{
	if (m_Choices.empty() == true)
		return;

	m_iChoiceCount = 0;

	CDialogue_Choice* pChoice = static_cast<CDialogue_Choice*>(m_Choices.back());
	m_Choices.pop_back();

	for (auto& it : m_CurrentChoices)
	{
		static_cast<CDialogue_Choice*>(it)->Up();
		m_iChoiceCount++;
	}

	pChoice->Reset();
	pChoice->Visible(true);
	pChoice->Set_Hover(true);
	pChoice->Text(Choice.pText);
	pChoice->Type(m_iChoiceCount, Choice.eType);

	m_CurrentChoices.push_back(pChoice);
}

void CDialogue_Panel::Choice_Slot(CHOICEINFO Choice)
{
	switch (Choice.iType)
	{
	case ENUM_CLASS(NPCTEXTTYPE::NEXTTEXT):
		m_pInfoInstance->Event_CallBack(TEXT("NPCNEXTTEXT"), &Choice);
		break;

	case ENUM_CLASS(NPCTEXTTYPE::CHOICE):
		m_pInfoInstance->Event_CallBack(TEXT("NPCNEXTTEXT"), &Choice);
		break;

	case ENUM_CLASS(NPCTEXTTYPE::SHOPTEXT):
		m_pInfoInstance->Event_CallBack(TEXT("NPCNEXTTEXT"), &Choice);
		break;

	case ENUM_CLASS(NPCTEXTTYPE::QUESTTEXT):
		m_pInfoInstance->Event_CallBack(TEXT("NPCNEXTTEXT"), &Choice);
		break;

	case ENUM_CLASS(NPCTEXTTYPE::SPELLLEAN):

		break;

	case ENUM_CLASS(NPCTEXTTYPE::BROOM):
		m_pInfoInstance->Event_CallBack(TEXT("NPCNEXTTEXT"), &Choice);
		m_bBroomRace = true;
		break;

	case ENUM_CLASS(NPCTEXTTYPE::BATTLE):
		m_pInfoInstance->Event_CallBack(TEXT("NPCNEXTTEXT"), &Choice);
		m_bBttle = true;
		break;

	default:
		break;
	}


}

void CDialogue_Panel::ReSet_Choice()
{
	for (auto it = m_CurrentChoices.begin(); it != m_CurrentChoices.end();)
	{
		(static_cast<CUIObject*>(*it))->Visible(false);
		(static_cast<CUIObject*>(*it))->Set_Hover(false);
		m_Choices.push_back((*it));
		it = m_CurrentChoices.erase(it);
	}
}

void CDialogue_Panel::Change_Map()
{
	m_pInfoInstance->Load_DADA_INT();
	m_pGameInstance->Setting_Volumetirc(
		3.f,
		0.01f,
		0.11f,
		1.0f,
		0.f
	);
	CLayer* pLayer = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER);
	if (nullptr != pLayer) {
		CPlayer* pPlayer = pLayer->Get_Object<CPlayer>();
		pPlayer->Get_Component<CCharacter_Controller>()->Set_Position(XMVectorSet(1003.f, 5.f, 1005.f, 1.f));
	}
}

void CDialogue_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CDialogue_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}


	if (m_bBroomRace == true || m_bBttle == true)
	{
		m_fTime += fTimeDelta;
	}

	if (m_fTime >= 1.f)
	{
		if(m_bBroomRace == true)
		{
			m_bBroomRace = false;
			m_pInfoInstance->Event_CallBack(TEXT("BROOMRACENPCINTERACT"));
		}
		else if (m_bBttle == true)
		{
			m_bBttle = false;
			Change_Map();
		}
	}
	__super::Update(fTimeDelta);
}

void CDialogue_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bVisible) {
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CDialogue_Panel::Render()
{
	return S_OK;
}

_vector CDialogue_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CDialogue_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CDialogue_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDialogue_Panel::Ready_Element(void* pArg)
{
	for (_int i = 0; i < 5; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDialogue_Choice>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CDialogue_Choice**>(&m_pDialogue_Choice))))
		{
			return E_FAIL;
		}

		m_Choices.push_back(m_pDialogue_Choice);
	}
	return S_OK;
}

CDialogue_Panel* CDialogue_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDialogue_Panel* pInstance = new CDialogue_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialogue_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDialogue_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CDialogue_Panel* pInstance = new CDialogue_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialogue_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDialogue_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CDialogue_Panel::Describe_Entity()
{
}
#endif // _DEBUG
