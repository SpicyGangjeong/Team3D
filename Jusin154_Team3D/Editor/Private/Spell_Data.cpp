#include "pch.h"
#include "Spell_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"

;

CSpell_Data::CSpell_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CSpell_Data::CSpell_Data(const CSpell_Data& rhs)
	:CGameObject(rhs)
{
}

HRESULT CSpell_Data::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSpell_Data::Initialize(void* pArg)
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
		pInfo[id].pType_Name = CMyTools::ToWstring(pSpell->Attribute("Type_Name"));
		pSpell->QueryIntAttribute("Skill_Type", &pInfo[id].iSkill_Type);
		pSpell->QueryFloatAttribute("Spell_Damage", &pInfo[id].fSpell_Damage);
		pSpell->QueryFloatAttribute("Spell_Cooltime", &pInfo[id].fSpell_CoolTime);
		pSpell->QueryFloatAttribute("Duration", &pInfo[id].fDuration);
		pSpell->QueryIntAttribute("AnimNum", &pInfo[id].iAnimNum);
		pInfo[id].pSpellInfo = CMyTools::ToWstring(pSpell->Attribute("SpellInfo"));
		_int lock = 0, equip = 0, use = 0;
		pSpell->QueryIntAttribute("Spell_Lock", &lock);
		pSpell->QueryIntAttribute("Equip_Spell", &equip);
		pSpell->QueryIntAttribute("Use_Skill", &use);
		pInfo[id].bSpell_Lock = lock != 0;
		pInfo[id].bEquip_Spell = equip != 0;
		pInfo[id].bUse_Skill = use != 0;
		pSpell->QueryFloatAttribute("Preview", &pInfo[id].fPreview);
		pSpell->QueryFloatAttribute("Vidio", &pInfo[id].fVidio);

		pSpell = pSpell->NextSiblingElement("Spell");
	}

	//pInfo = SpellInfo;
	return S_OK;
}

//const CUIObject::SPELLINFO& CSpell_Data::Get_Info(_uint SpellID) const
//{
//	return SpellInfo[SpellID];
//}

void CSpell_Data::Priority_Update(_float fTimeDelta)
{
}

void CSpell_Data::Update(_float fTimeDelta)
{
}

void CSpell_Data::Late_Update(_float fTimeDelta)
{
}

HRESULT CSpell_Data::Render()
{
	return S_OK;
}

_vector CSpell_Data::Get_WorldPostion()
{
	return _vector();
}

HRESULT CSpell_Data::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSpell_Data::Ready_Components(void* pArg)
{
	return S_OK;
}

CSpell_Data* CSpell_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Data* pInstance = new CSpell_Data(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Data::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Data* pInstance = new CSpell_Data(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Data::Free()
{
	__super::Free();

}

void CSpell_Data::Describe_Entity()
{
}
