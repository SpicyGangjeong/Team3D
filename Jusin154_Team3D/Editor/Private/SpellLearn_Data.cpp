#include "pch.h"
#include "SpellLearn_Data.h"
#include "GameObject.h"
#include "UIObject.h"

CSpellLearn_Data::CSpellLearn_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CSpellLearn_Data::CSpellLearn_Data(const CSpellLearn_Data& rhs)
	:CGameObject(rhs)
{
}

HRESULT CSpellLearn_Data::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellLearn_Data::Initialize(void* pArg)
{
	auto* pInfo = static_cast<vector<CUIObject::SPELLLEARNINFO>*>(pArg);

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile("../Bin/Resources/Data/UI/SpellLearn.xml") != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	tinyxml2::XMLElement* pSpellLearnInfo = doc.FirstChildElement("SpellLearnInfo");
	if (!pSpellLearnInfo) return E_FAIL;

	tinyxml2::XMLElement* pSpellLearnData = pSpellLearnInfo->FirstChildElement("SpellLearnData");
	if (!pSpellLearnData) return E_FAIL;

	// 모든 Name 요소 반복
	tinyxml2::XMLElement* pName = pSpellLearnData->FirstChildElement("Name");
	while (pName)
	{
		CUIObject::SPELLLEARNINFO Info{};

		// 속성 읽기
		Info.pSpellName = CMyTools::ToWstring(pName->Attribute("SpellName"));
		Info.pImageName = CMyTools::ToWstring(pName->Attribute("ImageName"));
		pName->QueryIntAttribute("SpellID", &Info.iSpellID);
		pName->QueryIntAttribute("SpellType", &Info.iSpellType);
		pName->QueryFloatAttribute("Size", &Info.fSpellSize);

		_float fX{}, fY{};
		pName->QueryFloatAttribute("x", &fX);
		pName->QueryFloatAttribute("y", &fY);
		Info.fStartPosition = _float2(fX, fY);
		_float endx{}, endy{};
		pName->QueryFloatAttribute("endx", &endx);
		pName->QueryFloatAttribute("endy", &endy);
		Info.fEndPosition = _float2(endx, endy);

		tinyxml2::XMLElement* pBooster = pName->FirstChildElement("Booster");
		while (pBooster)
		{
			tinyxml2::XMLElement* pPos = pBooster->FirstChildElement("Pos");
			if (pPos)
			{
				_float px{}, py{};
				pPos->QueryFloatAttribute("x", &px);
				pPos->QueryFloatAttribute("y", &py);
				Info.Booster.push_back(_float4(px, py, 0.f, 1.f));
			}

			// 다음 Position으로 이동
			pBooster = pBooster->NextSiblingElement("Booster");
		}

		// Position 요소 반복
		tinyxml2::XMLElement* pPosition = pName->FirstChildElement("Position");
		while (pPosition)
		{
			tinyxml2::XMLElement* pPos = pPosition->FirstChildElement("Pos");
			if (pPos)
			{
				_float px{}, py{};
				pPos->QueryFloatAttribute("x", &px);
				pPos->QueryFloatAttribute("y", &py);
				Info.Lines.push_back(_float4(px, py, 0.f, 1.f));
			}

			// 다음 Position으로 이동
			pPosition = pPosition->NextSiblingElement("Position");
		}

		// 벡터에 추가
		m_SpellLearnInfos.push_back(Info);
		m_iSpellLearn_Count++;

		// 다음 Name으로 이동
		pName = pName->NextSiblingElement("Name");
	}

	*pInfo = m_SpellLearnInfos;

	return S_OK;
}

const CUIObject::SPELLLEARNINFO& CSpellLearn_Data::Get_SpellLearn(_int Index) const
{
	return m_SpellLearnInfos[Index];
}

void CSpellLearn_Data::Priority_Update(_float fTimeDelta)
{
}

void CSpellLearn_Data::Update(_float fTimeDelta)
{
}

void CSpellLearn_Data::Late_Update(_float fTimeDelta)
{
}

HRESULT CSpellLearn_Data::Render()
{
	return S_OK;
}

_vector CSpellLearn_Data::Get_WorldPostion()
{
	return _vector();
}

HRESULT CSpellLearn_Data::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSpellLearn_Data::Ready_Components(void* pArg)
{
	return S_OK;
}

CSpellLearn_Data* CSpellLearn_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn_Data* pInstance = new CSpellLearn_Data(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpellLearn_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellLearn_Data::Clone(void* pArg, CGameObject* pOwner)
{
	CSpellLearn_Data* pInstance = new CSpellLearn_Data(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn_Data::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpellLearn_Data::Describe_Entity()
{
}
