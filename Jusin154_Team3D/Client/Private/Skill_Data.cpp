#include "pch.h"
#include "Skill_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"

;

CSkill_Data::CSkill_Data()
{
}

HRESULT CSkill_Data::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{


	return S_OK;
}

const SPELL_INFO& CSkill_Data::Get_Info(_uint SpellID) const
{
	return SpellInfo[SpellID];
}

void CSkill_Data::Update(_float fTimeDelta)
{
}

void CSkill_Data::Change_Level()
{
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
		pSpell->QueryIntAttribute("Skill_Type", &SpellInfo[id].iSkill_Type);
		pSpell->QueryFloatAttribute("Spell_Cooltime", &SpellInfo[id].fSpell_CoolTime);
		pSpell->QueryFloatAttribute("Duration", &SpellInfo[id].fDuration);
		pSpell->QueryIntAttribute("AnimNum", &SpellInfo[id].iAnimNum);
		SpellInfo[id].pSpellInfo = CMyTools::ToWstring(pSpell->Attribute("SpellInfo"));
		int lock = 0, equip = 0;
		pSpell->QueryIntAttribute("Spell_Lock", &lock);
		pSpell->QueryIntAttribute("Equip_Spell", &equip);
		SpellInfo[id].bSpell_Lock = lock != 0;
		SpellInfo[id].bEquip_Spell = equip != 0;

		pSpell = pSpell->NextSiblingElement("Spell");
	}

	return S_OK;
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

