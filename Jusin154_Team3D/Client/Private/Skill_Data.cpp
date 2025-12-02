#include "pch.h"
#include "Skill_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"

;

CSkill_Data::CSkill_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CSkill_Data::CSkill_Data(const CSkill_Data& rhs)
	:CGameObject(rhs)
{
}

HRESULT CSkill_Data::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSkill_Data::Initialize(void* pArg)
{
	auto* pInfo = static_cast<CUIObject::SPELLINFO*>(pArg);

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError Error = doc.LoadFile("../Bin/Resources/Data/UI/Spell/SpellInfo.xml");
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

		pInfo[id].iSpell_ID = id;
		pInfo[id].pSpell_Name = CMyTools::ToWstring(pSpell->Attribute("Spell_Name"));
		pInfo[id].pImage_Name = CMyTools::ToWstring(pSpell->Attribute("Image_Name"));
		pSpell->QueryIntAttribute("Spell_Type", &pInfo[id].iSpell_Type);
		pSpell->QueryIntAttribute("Skill_Type", &pInfo[id].iSkill_Type);
		pSpell->QueryFloatAttribute("Spell_Cooltime", &pInfo[id].fSpell_CoolTime);
		pSpell->QueryFloatAttribute("Duration", &pInfo[id].fDuration);
		pSpell->QueryIntAttribute("AnimNum", &pInfo[id].iAnimNum);
		pInfo[id].pSpellInfo = CMyTools::ToWstring(pSpell->Attribute("SpellInfo"));
		int lock = 0, equip = 0;
		pSpell->QueryIntAttribute("Spell_Lock", &lock);
		pSpell->QueryIntAttribute("Equip_Spell", &equip);
		pInfo[id].bSpell_Lock = lock != 0;
		pInfo[id].bEquip_Spell = equip != 0;

		pSpell = pSpell->NextSiblingElement("Spell");
	}

	//pInfo = SpellInfo;
	return S_OK;
}

//const CUIObject::SPELLINFO& CSkill_Data::Get_Info(_uint SpellID) const
//{
//	return SpellInfo[SpellID];
//}

void CSkill_Data::Priority_Update(_float fTimeDelta)
{
}

void CSkill_Data::Update(_float fTimeDelta)
{
}

void CSkill_Data::Late_Update(_float fTimeDelta)
{
}

HRESULT CSkill_Data::Render()
{
	return S_OK;
}

_vector CSkill_Data::Get_WorldPostion()
{
	return _vector();
}

HRESULT CSkill_Data::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSkill_Data::Ready_Components(void* pArg)
{
	return S_OK;
}

CSkill_Data* CSkill_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Data* pInstance = new CSkill_Data(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkill_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Data::Clone(void* pArg, CGameObject* pOwner)
{
	CSkill_Data* pInstance = new CSkill_Data(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkill_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSkill_Data::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CSkill_Data::Describe_Entity()
{
}

#endif // _DEBUG
