#include "pch.h"
#include "Skill_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"
#include "InfoInstance.h"

CSkill_Data::CSkill_Data()
	:m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CSkill_Data::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	m_bNoCool = false;
	m_pInfoInstance->Add_Event(TEXT("NoCooL"), [this](void* p) {this->NoCool(*reinterpret_cast<_bool*>(p)); });
	return S_OK;
}

const SPELL_INFO& CSkill_Data::Get_Info(_uint SpellID) const
{
	return SpellInfo[SpellID];
}

_int CSkill_Data::Update_Spell(_int SpellID)
{
	if (SpellID < 0 || SpellID > m_iSpell_Count)
		return ENUM_CLASS(SKILL_TYPE::END);

	if (SpellInfo[SpellID].bUse_Skill == false)
		return -1;

	if (m_bNoCool == false)
	{
		SpellInfo[SpellID].bUse_Skill = false;
		m_iSpell_CoolTime[SpellID] = 0.f;
	}

	return SpellInfo[SpellID].iSkill_Type;
}

void CSkill_Data::Update(_float fTimeDelta)
{
	for (_int i = 0; i < m_iSpell_Count; ++i)
	{
		if (SpellInfo[i].bUse_Skill == false)
		{
			if (m_iSpell_CoolTime[i] <= 1.f)
			{
				m_iSpell_CoolTime[i] += fTimeDelta * (1.f / SpellInfo[i].fSpell_CoolTime);
			}
			else
			{
				SpellInfo[i].bUse_Skill = true;
			}

		}
	}

	if (m_bNoCool == true)
	{
		for (_int i = 0; i < 34; ++i)
		{
			m_iSpell_CoolTime[i] = 1.f;
		}
	}

}

void CSkill_Data::Change_Level()
{
}

_float CSkill_Data::Get_CoolTime(_int SpellID)
{
	if (SpellID < 0 || SpellID > m_iSpell_Count)
		return 0.f;

	return m_iSpell_CoolTime[SpellID];
}

HRESULT CSkill_Data::Load_SpellInfo(const _char* pFilePath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError Error = doc.LoadFile(pFilePath);
	if (Error != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	tinyxml2::XMLElement* pSpellInfo = doc.FirstChildElement("SpellInfo");
	if (!pSpellInfo)
		return E_FAIL;

	tinyxml2::XMLElement* pSpell = pSpellInfo->FirstChildElement("Spell");
	while (pSpell)
	{
		int id = 0;
		pSpell->QueryIntAttribute("Spell_ID", &id);

		if (id < 0 || id > 34)
		{
			pSpell = pSpell->NextSiblingElement("Spell");
			continue;
		}

		SpellInfo[id].iSpell_ID = id;
		SpellInfo[id].pSpell_Name = CMyTools::ToWstring(pSpell->Attribute("Spell_Name"));
		SpellInfo[id].pImage_Name = CMyTools::ToWstring(pSpell->Attribute("Image_Name"));
		pSpell->QueryIntAttribute("Spell_Type", &SpellInfo[id].iSpell_Type);
		SpellInfo[id].pType_Name = CMyTools::ToWstring(pSpell->Attribute("Type_Name"));
		pSpell->QueryIntAttribute("Skill_Type", &SpellInfo[id].iSkill_Type);
		pSpell->QueryFloatAttribute("Spell_Damage", &SpellInfo[id].fSpell_Damage);
		pSpell->QueryFloatAttribute("Spell_Cooltime", &SpellInfo[id].fSpell_CoolTime);
		pSpell->QueryFloatAttribute("Duration", &SpellInfo[id].fDuration);
		pSpell->QueryIntAttribute("AnimNum", &SpellInfo[id].iAnimNum);
		SpellInfo[id].pSpellInfo = CMyTools::ToWstring(pSpell->Attribute("SpellInfo"));
		int lock = 0, equip = 0, use = 0;
		pSpell->QueryIntAttribute("Spell_Lock", &lock);
		pSpell->QueryIntAttribute("Equip_Spell", &equip);
		pSpell->QueryIntAttribute("Use_Skill", &use);
		SpellInfo[id].bSpell_Lock = lock != 0;
		SpellInfo[id].bEquip_Spell = equip != 0;
		SpellInfo[id].bUse_Skill = use != 0;
		pSpell = pSpell->NextSiblingElement("Spell");
		m_iSpell_Count++;
		m_iSpell_CoolTime[id] = 1.2f;
	}

	return S_OK;
}

void CSkill_Data::NoCool(_bool bNoCool)
{
	m_bNoCool = bNoCool;
}

CSkill_Data* CSkill_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Data* pInstance = new CSkill_Data();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed to Created : CSkill_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSkill_Data::Free()
{
	__super::Free();
}

