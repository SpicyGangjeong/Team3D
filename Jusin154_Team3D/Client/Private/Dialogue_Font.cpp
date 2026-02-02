#include "pch.h"
#include "Dialogue_Font.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Dialogue.h"
#include "Unit.h"

CDialogue_Font::CDialogue_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CDialogue_Font::CDialogue_Font(const CDialogue_Font& rhs)
	:CGameObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CDialogue_Font::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialogue_Font::Initialize(void* pArg)
{
	m_pInfoInstance->Add_Event(TEXT("Dialogue"), [this](void* p) {this->Add_Text(p); });
	m_pInfoInstance->Add_Event(TEXT("NpcInteract"), [this](void* p) {this->NpcInteract(*reinterpret_cast<_bool*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("NPCInteractionOn"), [this](void* p) {this->NpcInfo(p); });
	m_pInfoInstance->Add_Event(TEXT("NPCDialogue"), [this](void* p) {this->NpcDialogue(reinterpret_cast<CNPCStat*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("NPCNEXTTEXT"), [this](void* p) {this->NextLevel(*reinterpret_cast<CHOICEINFO*>(p)); });

	for (_int i = 0; i < 5; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDialogue>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CDialogue**>(&m_pDialogue)))) {
			return E_FAIL;
		}

		m_pDialogue->Visible(true);
		m_DialoguInfo.push_back(m_pDialogue);
	}

	return S_OK;
}

void CDialogue_Font::Priority_Update(_float fTimeDelta)
{
}

void CDialogue_Font::Update(_float fTimeDelta)
{
	if (m_bNpcInteract == false)
	{
		if (m_bCurrentInteract == true)
			m_bCurrentInteract = false;
	}

	if (m_bNpcInteract != m_bCurrentInteract)
	{
		m_bCurrentInteract = m_bNpcInteract;
		NpcDialogue();
	}

	if (m_bCurrentInteract == true)
	{
		if (m_bChoiceText == true)
		{
			_bool Map = true;
			switch (m_iType)
			{
			case ENUM_CLASS(NPCTEXTTYPE::ENDTEXT):
				ENDText();
				break;

			case ENUM_CLASS(NPCTEXTTYPE::NEXTTEXT):
				NextText();
				break;

			case ENUM_CLASS(NPCTEXTTYPE::CHOICE):
				CHoice();
				break;

			case ENUM_CLASS(NPCTEXTTYPE::SHOPTEXT):
				break;

			case ENUM_CLASS(NPCTEXTTYPE::QUESTTEXT):
				Quest();
				break;

			case ENUM_CLASS(NPCTEXTTYPE::SPELLLEAN):
				break;

			case ENUM_CLASS(NPCTEXTTYPE::BROOM):
				m_bRace = true;
				m_pInfoInstance->Event_CallBack(TEXT("RACEREADY"), &m_bRace);
				MapMove();
				break;

			case ENUM_CLASS(NPCTEXTTYPE::BATTLE):
				m_bBattle = true;
				m_pInfoInstance->Event_CallBack(TEXT("BATTLE"), &m_bBattle);
				MapMove();
				break;

			default:
				break;
			}
		}
		else
		{
			if (m_fTime <= 0.f)
			{
				if (m_pGameInstance->Key_Down(DIK_RETURN) && m_bCurrentChoiceText == false)
				{
					switch (m_iType)
					{
					case ENUM_CLASS(NPCTEXTTYPE::ENDTEXT):
						m_fTime = 0.f;
						ENDText();
						break;
					case ENUM_CLASS(NPCTEXTTYPE::NEXTTEXT):
						m_fTime = 0.5f;
						NextText();
						break;
					case ENUM_CLASS(NPCTEXTTYPE::CHOICE):
						m_fTime = 0.5f;
						CHoice();
						break;
					default:
						break;
					}
				}

			}
		}
	}

	if (m_fTime > 0.f)
	{
		m_fTime -= fTimeDelta;
		if (m_fTime <= 0.f)
		{
			m_fTime = 0.f;
		}
	}
}

void CDialogue_Font::Late_Update(_float fTimeDelta)
{
	if (!m_pCurrentDialogue.empty())
	{
		if (!m_pCurrentDialogue.empty())
		{
			for (auto it = m_pCurrentDialogue.begin(); it != m_pCurrentDialogue.end();)
			{
				if ((*it)->Get_Hover() == false)
				{
					(*it)->Visible(false);
					m_DialoguInfo.push_back((*it));
					it = m_pCurrentDialogue.erase(it);
				}
				else
				{
					it++;
				}
			}
		}
	}
	if (!m_pCurrentDialogue.empty())
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI_OVERLAY, this);
	}
}

HRESULT CDialogue_Font::Render()
{
	for (auto it : m_pCurrentDialogue)
	{
		it->Render();
	}

	return S_OK;
}

_vector CDialogue_Font::Get_WorldPostion()
{
	return _vector();
}

HRESULT CDialogue_Font::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CDialogue_Font::Ready_Components(void* pArg)
{
	return S_OK;
}

void CDialogue_Font::Add_Text(void* pArg)
{
	if (m_DialoguInfo.empty() == true)
		return;

	DIALOGUEINFO pInfo = *reinterpret_cast<DIALOGUEINFO*>(pArg);

	CDialogue* Dialogue = m_DialoguInfo.back();
	m_DialoguInfo.pop_back();

	for (auto& it : m_pCurrentDialogue)
	{
		it->Up();
	}

	pInfo.fTime = 2.5f;
	Dialogue->SizeUpdate(pInfo.pName, pInfo.pText);
	Dialogue->Visible(true);
	Dialogue->Set_Time(pInfo.fTime);
	Dialogue->Set_Hover(true);

	m_pCurrentDialogue.push_front(Dialogue);
}

void CDialogue_Font::Npc_Dialogue(DIALOGUEINFO Info)
{
	if (m_pCurrentDialogue.size() >= 5)
	{
		CDialogue* old = m_pCurrentDialogue.back();
		m_pCurrentDialogue.pop_back();
		m_DialoguInfo.push_back(old);
	}

	if (m_DialoguInfo.empty() == true)
		return;

	CDialogue* Dialogue = m_DialoguInfo.back();
	m_DialoguInfo.pop_back();

	for (auto& it : m_pCurrentDialogue)
	{
		it->Up();
	}

	Dialogue->SizeUpdate(Info.pName, Info.pText);
	Dialogue->Visible(true);
	Dialogue->Set_Time(Info.fTime);
	Dialogue->Set_Hover(true);

	m_pCurrentDialogue.push_front(Dialogue);
}

void CDialogue_Font::NpcInfo(void* pArg)
{
	NPCINTERACTIONINFO* Info = reinterpret_cast<NPCINTERACTIONINFO*>(pArg);
	m_pNpc = static_cast<CUnit*>(Info->pOwner);
	m_pNpcName = Info->pName;
	m_pName = Info->pNPCName;
	m_iTextID = Info->iTextID;
	m_iNextID = Info->iNextID;
}

void CDialogue_Font::NpcInteract(_bool bInteract)
{
	m_bNpcInteract = bInteract;
}

void CDialogue_Font::NpcDialogue()
{
	_int ID{};
	if (m_iNextID == 0)
	{
		ID = m_iTextID;
	}
	else
	{
		ID = m_iNextID;
	}
	auto Info = m_pInfoInstance->Get_Dialogue(m_pNpcName, ID);
	m_bTag = Info.bTag;
	if (m_bTag == false)
	{
		m_Info.pName = m_pInfoInstance->Get_PlayerStatPtr()->Get_Stat().pUnit_Name;
	}
	else
	{
		m_Info.pName = m_pName;
	}
	m_Info.pText = Info.pText;
	m_Info.fTime = 99999.f;
	m_iType = Info.iType;
	m_iNextID = Info.NextTextID;
	m_iTextID = Info.iLineID;
	m_pNpc->Set_NextID(m_iNextID);
	Npc_Dialogue(m_Info);
}

void CDialogue_Font::NpcDialogue(CNPCStat* Stat)
{
	auto Info = m_pInfoInstance->Get_Dialogue(Stat->Get_Stat().pName, 0);
	m_Info.pName = Stat->Get_Stat().pNpc_Name;
	m_Info.pText = Info.pText;
	m_Info.fTime = 3.f;
	m_iType = Info.iType;
	m_iNextID = Info.NextTextID;
	m_iTextID = Info.iLineID;
	m_bTag = Info.bTag;
	Npc_Dialogue(m_Info);
}

void CDialogue_Font::NpcNextText()
{
	auto Info = m_pInfoInstance->Get_Dialogue(m_pNpcName, m_iTextID);
	m_bTag = Info.bTag;
	if (m_bTag == false)
	{
		m_Info.pName = m_pInfoInstance->Get_PlayerStatPtr()->Get_Stat().pUnit_Name;
	}
	else
	{
		m_Info.pName = m_pName;
	}
	m_Info.pText = Info.pText;
	m_Info.fTime = 99999.f;
	Npc_Dialogue(m_Info);
}

void CDialogue_Font::CHoice()
{
	m_NextLevel.clear();
	m_bCurrentChoiceText = true;
	size_t InfoCount = m_pInfoInstance->Get_Dialogue(m_pNpcName, m_iTextID).ChoiceInfo.size();
	for (size_t i = 0; i < InfoCount; ++i)
	{
		DIALOGUECHOICEINFO Info = m_pInfoInstance->Get_Dialogue(m_pNpcName, m_iTextID).ChoiceInfo[i];
		m_pInfoInstance->Event_CallBack(TEXT("TEXTTYPE"), &Info);
		m_NextLevel.push_back(m_pInfoInstance->Get_Dialogue(m_pNpcName, m_iTextID).ChoiceInfo[i].NextTypeID);
	}
}

void CDialogue_Font::Shop()
{
}

void CDialogue_Font::SpellLearn()
{
}

void CDialogue_Font::Quest()
{
	_int Index = m_pInfoInstance->Set_AcceptQuest(m_iQuestID);
	NPCINTERACT Interact{};

	switch (Index)
	{
	case 0:
		m_pCurrentDialogue[0]->Set_Hover(Interact.bInteract);
		m_pCurrentDialogue[0]->Visible(Interact.bInteract);
		m_DialoguInfo.push_back(m_pCurrentDialogue[0]);
		m_pCurrentDialogue.erase(m_pCurrentDialogue.begin());
		if (m_bChoiceText == true)
		{
			m_pInfoInstance->Event_CallBack(TEXT("CHOICERESET"));
			m_pInfoInstance->Event_CallBack(TEXT("NpcInteract"), &Interact);
			ENDText();
		}
		break;

	case 1:
		NextText(999);
		break;

	case 2:
		Quest_Complete();
		break;

	default:
		break;
	}

}

void CDialogue_Font::ENDText()
{
	_bool bInteract = false;
	NPCINTERACT Interact{};
	Interact.bInteract = bInteract;
	Interact.fAlpha = 1.f;
	m_pInfoInstance->Event_CallBack(TEXT("NpcInteract"), &Interact.bInteract);
	m_pNpc->Set_Flow(m_pNpc->Get_Flow() + 1, Interact.fAlpha);
	m_pNpc->Set_NextID(m_iNextID);
	m_pInfoInstance->Event_CallBack(TEXT("NpcInteraction"), &bInteract);
	m_pInfoInstance->Event_CallBack(TEXT("CHOICERESET"));
	m_pInfoInstance->Event_CallBack(TEXT("ReSetNPC"));
	m_bChoiceText = false;
	m_bCurrentChoiceText = false;
	m_bRace = false;
	m_bBattle = false;
	vector<_int> Dummy;
	m_NextLevel.swap(Dummy);
	for (auto it = m_pCurrentDialogue.begin(); it != m_pCurrentDialogue.end(); ++it)
	{
		(*it)->Set_Time(3.f);
	}
}

void CDialogue_Font::ReSet()
{
	NPCINTERACT Interact{};
	Interact.bInteract = false;
	Interact.fAlpha = 0.f;
	m_pInfoInstance->Event_CallBack(TEXT("NpcInteract"), &Interact);
	m_pNpc->Set_Flow(m_pNpc->Get_Flow() + 1, Interact.fAlpha);
	m_pNpc->Set_NextID(m_iNextID);
	m_pInfoInstance->Event_CallBack(TEXT("NpcInteraction"), &Interact);
	m_pInfoInstance->Event_CallBack(TEXT("CHOICERESET"));
	m_pInfoInstance->Event_CallBack(TEXT("ReSetNPC"));
	m_bChoiceText = false;
	m_bCurrentChoiceText = false;
	m_bRace = false;
	m_bBattle = false;
	vector<_int> Dummy;
	m_NextLevel.swap(Dummy);
	for (auto it = m_pCurrentDialogue.begin(); it != m_pCurrentDialogue.end();)
	{
		(*it)->Set_Hover(false);
		(*it)->Visible(false);
		m_DialoguInfo.push_back((*it));
		it = m_pCurrentDialogue.erase(it);
	}
}

void CDialogue_Font::MapMove()
{
	NPCINTERACT Interact{};
	Interact.bInteract = false;
	Interact.fAlpha = 1.f;
	m_pInfoInstance->Event_CallBack(TEXT("NpcInteract"), &Interact);
	m_pNpc->Set_Flow(m_pNpc->Get_Flow() + 1, Interact.fAlpha);
	m_pNpc->Set_NextID(m_iNextID);
	m_pInfoInstance->Event_CallBack(TEXT("NpcInteraction"), &Interact);
	m_pInfoInstance->Event_CallBack(TEXT("CHOICERESET"));
	m_pInfoInstance->Event_CallBack(TEXT("ReSetNPC"));
	m_bChoiceText = false;
	m_bCurrentChoiceText = false;
	vector<_int> Dummy;
	m_NextLevel.swap(Dummy);
	for (auto it = m_pCurrentDialogue.begin(); it != m_pCurrentDialogue.end();)
	{
		(*it)->Set_Hover(false);
		(*it)->Visible(false);
		m_DialoguInfo.push_back((*it));
		it = m_pCurrentDialogue.erase(it);
	}
	
	if (m_bRace == true)
	{
		m_pInfoInstance->Event_CallBack(TEXT("UIFadeIn"), &Interact.fAlpha);
	}
	m_bRace = false;
	m_bBattle = false;
}

void CDialogue_Font::Quest_Complete()
{
	NextText();
}

void CDialogue_Font::NextText()
{
	if (m_bChoiceText == true)
	{
		m_bChoiceText = false;
		m_bCurrentChoiceText = false;
		m_pInfoInstance->Event_CallBack(TEXT("CHOICERESET"));
	}
	Skip();
	NpcDialogue();
}

void CDialogue_Font::Skip()
{
	m_pCurrentDialogue[0]->Visible(false);
	m_DialoguInfo.push_back(m_pCurrentDialogue[0]);
	m_pCurrentDialogue.erase(m_pCurrentDialogue.begin());
}

void CDialogue_Font::NextLevel(CHOICEINFO Choice)
{
	m_iNextID = m_NextLevel[Choice.iChoice];
	m_iType = Choice.iType;
	m_iQuestID = Choice.QuestID;
	m_bChoiceText = true;
	Skip();
	if (m_iType != 0)
		NpcNextText();
}

void CDialogue_Font::NextText(_int Index)
{
	m_pCurrentDialogue[0]->Visible(false);
	m_DialoguInfo.push_back(m_pCurrentDialogue[0]);
	m_pCurrentDialogue.erase(m_pCurrentDialogue.begin());
	if (m_bChoiceText == true)
	{
		ENDText();
	}
	NpcDialogue(Index);
}

void CDialogue_Font::NpcDialogue(_int Index)
{
	auto Info = m_pInfoInstance->Get_Dialogue(m_pNpcName, Index);
	m_bTag = Info.bTag;
	if (m_bTag == false)
	{
		m_Info.pName = m_pInfoInstance->Get_PlayerStatPtr()->Get_Stat().pUnit_Name;
	}
	else
	{
		m_Info.pName = m_pName;
	}
	m_Info.pText = Info.pText;
	m_Info.fTime = 99999.f;
	m_iType = Info.iType;
	m_iNextID = Info.NextTextID;
	m_iTextID = Info.iLineID;
	m_pNpc->Set_NextID(m_iNextID);
	Npc_Dialogue(m_Info);
}

CDialogue_Font* CDialogue_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDialogue_Font* pInstance = new CDialogue_Font(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialogue_Font");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDialogue_Font::Clone(void* pArg, CGameObject* pOwner)
{
	CDialogue_Font* pInstance = new CDialogue_Font(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialogue_Font");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDialogue_Font::Free()
{
	__super::Free();

	m_DialoguInfo.clear();
}

#ifdef _DEBUG
void CDialogue_Font::Describe_Entity()
{
}
#endif // _DEBUG
