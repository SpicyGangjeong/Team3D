#include "pch.h"
#include "Dialogue_Font.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Dialogue.h"

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
		Npc_Dialogue(m_Info);
	}

	if (m_bCurrentInteract == true)
	{
		if (m_pGameInstance->Key_Down(DIK_RETURN))
		{
			switch (m_iType)
			{
			case 0:
				NextText();
				break;
			case 1:
				CHoice();
				break;
			default:
				break;
			}
		}

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
}

void CDialogue_Font::Late_Update(_float fTimeDelta)
{
	if (!m_DialoguInfo.empty())
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI_OVERLAY, this);
	}
}

HRESULT CDialogue_Font::Render()
{
	if (!m_pCurrentDialogue.empty())
	{
		for (auto it : m_pCurrentDialogue)
		{
			it->Render();
		}
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

	m_pCurrentDialogue.push_front(Dialogue);
}

void CDialogue_Font::Npc_Dialogue(DIALOGUEINFO Info)
{
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

	m_pCurrentDialogue.push_front(Dialogue);
}

void CDialogue_Font::NpcInfo(void* pArg)
{
	NPCINTERACTIONINFO* Info = reinterpret_cast<NPCINTERACTIONINFO*>(pArg);
	m_pNpcName = Info->pName;
	m_iTextID = Info->iTextID;
}

void CDialogue_Font::NpcInteract(_bool bInteract)
{
	m_bNpcInteract = bInteract;
}

void CDialogue_Font::NpcDialogue()
{
	auto Info = m_pInfoInstance->Get_Dialogue(m_pNpcName, m_iTextID);
	m_Info.pName = m_pNpcName;
	m_Info.pText = Info.pText;
	m_Info.fTime = 99999.f;
	m_iType = Info.iTextType;
	m_iNextID = Info.NextTextID;
}

void CDialogue_Font::CHoice()
{
	auto Info = m_pInfoInstance->Get_Dialogue(m_pNpcName, m_iTextID);
//	Info.ChoiceInfo.size();
}

void CDialogue_Font::Shop()
{
}

void CDialogue_Font::SpellLearn()
{
}

void CDialogue_Font::NextText()
{
	if (m_iNextID == -1)
	{
		m_pCurrentDialogue[0]->Set_Hover(false);
		_bool Interact = false;
		m_pInfoInstance->Event_CallBack(TEXT("NpcInteract"), &Interact);
	}
	else
	{
		m_pCurrentDialogue[0]->Set_Hover(false);
		m_iTextID = m_iNextID;
		NpcDialogue();
		Npc_Dialogue(m_Info);
		if (m_iNextID == -1)
		{
			m_bEndText = true;
		}
	}
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
