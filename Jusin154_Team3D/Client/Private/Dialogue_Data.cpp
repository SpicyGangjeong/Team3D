#include "pch.h"
#include "Dialogue_Data.h"
#include "GameObject.h"
#include "tinyxml2.h"
#include "InfoInstance.h"

CDialogue_Data::CDialogue_Data()
	:m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CDialogue_Data::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	Load_SpellInfo("../Bin/Resources/Data/Dialogue/Dialogue.xml");

	return S_OK;
}

void CDialogue_Data::Update(_float fTimeDelta)
{
}

HRESULT CDialogue_Data::Load_SpellInfo(const _char* pFilePath)
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(pFilePath) != tinyxml2::XML_SUCCESS)
        return E_FAIL;

    tinyxml2::XMLElement* pDialogueInfo = doc.FirstChildElement("DialogueInfo");
    if (!pDialogueInfo) return E_FAIL;

    tinyxml2::XMLElement* pDialogue = pDialogueInfo->FirstChildElement("Dialogue");
    while (pDialogue)
    {
        NPCDIALOGUEINFO npc{};
        const char* NpcName = pDialogue->Attribute("NpcName");
        if (pDialogue) npc.pNpcName = CMyTools::ToWstring(NpcName);

        pDialogue->QueryIntAttribute("NpcID", &npc.iNpcID);

        tinyxml2::XMLElement* pLine = pDialogue->FirstChildElement("Line");
        while (pLine)
        {
            CURRENTDIALOGUEINFO line{};
            pLine->QueryIntAttribute("ID", &line.iLineID);

            int type = 0;
            pLine->QueryIntAttribute("Type", &type);
            line.bType = (type == 1);

            const char* text = pLine->Attribute("Text");
            if (text) line.pText = CMyTools::ToWstring(text);

            pLine->QueryIntAttribute("NextText", &line.NextTextID);

            // 선택지 처리
            tinyxml2::XMLElement* pChoice = pLine->FirstChildElement("Choice");
            while (pChoice)
            {
                DIALOGUECHOICEINFO choice{};

                pChoice->QueryIntAttribute("Type", &choice.eType);

                const char* choiceText = pChoice->Attribute("Text");
                if (choiceText) choice.pText = CMyTools::ToWstring(choiceText);

                pChoice->QueryIntAttribute("NextText", &choice.NextTypeID);

                line.ChoiceInfo.push_back(choice);
                pChoice = pChoice->NextSiblingElement("Choice");
            }

            npc.Info.emplace(line.iLineID, line);
            pLine = pLine->NextSiblingElement("Line");
        }

        m_DialogueInfo.emplace(npc.pNpcName, npc);
        pDialogue = pDialogue->NextSiblingElement("Dialogue");
    }
	return S_OK;
}

const CURRENTDIALOGUEINFO& CDialogue_Data::Get_Info(_wstring NpcName, _int iTextID) const
{
    auto iter = m_DialogueInfo.find(NpcName);
    if (iter == m_DialogueInfo.end())
    {
        return Dummy;
    }

    auto Text = iter->second.Info.find(iTextID);
    if (Text == iter->second.Info.end())
    {
        return Dummy;
    }

    return Text->second;
}

CDialogue_Data* CDialogue_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDialogue_Data* pInstance = new CDialogue_Data();

    if (FAILED(pInstance->Initialize(pDevice, pContext)))
    {
        MSG_BOX("Failed to Created : CDialogue_Data");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

void CDialogue_Data::Free()
{
	__super::Free();
}
