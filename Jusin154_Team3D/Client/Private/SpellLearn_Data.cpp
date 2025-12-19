#include "pch.h"
#include "SpellLearn_Data.h"
#include "UIObject.h"
#include "InfoInstance.h"

CSpellLearn_Data::CSpellLearn_Data()
	:m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CSpellLearn_Data::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	Load_SpellInfo("../Bin/Resources/Data/UI/SpellLearn.xml");

	return S_OK;
}

const SPELLLEARNINFO& CSpellLearn_Data::Get_SpellLearn(_int Index) const
{
	return m_SpellLearnInfos[Index];
}

_int CSpellLearn_Data::Get_Index()
{
	return m_iSpellLearn_Count;
}

void CSpellLearn_Data::Update(_float fTimeDelta)
{
}

HRESULT CSpellLearn_Data::Load_SpellInfo(const _char* pFilePath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(pFilePath) != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	tinyxml2::XMLElement* pSpellLearnInfo = doc.FirstChildElement("SpellLearnInfo");
	if (!pSpellLearnInfo) return E_FAIL;

	tinyxml2::XMLElement* pSpellLearnData = pSpellLearnInfo->FirstChildElement("SpellLearnData");
	if (!pSpellLearnData) return E_FAIL;

	// 모든 Name 요소 반복
	tinyxml2::XMLElement* pName = pSpellLearnData->FirstChildElement("Name");
	while (pName)
	{
		SPELLLEARNINFO Info{};

		// 속성 읽기
		Info.pSpellName = CMyTools::ToWstring(pName->Attribute("SpellName"));
		Info.pImageName = CMyTools::ToWstring(pName->Attribute("ImageName"));
		pName->QueryFloatAttribute("Size", &Info.fSpellSize);

		_float fX{}, fY{};
		pName->QueryFloatAttribute("x", &fX);
		pName->QueryFloatAttribute("y", &fY);
		Info.fStartPosition = _float2(fX, fY);

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
				Info.Lines.push_back(XMVectorSet(px, py, 0.f, 1.f));
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
	
	return S_OK;
}

CSpellLearn_Data* CSpellLearn_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn_Data* pInstance = new CSpellLearn_Data();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn_Data::Free()
{
	__super::Free();
}
