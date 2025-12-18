#include "pch.h"
#include "QuestInstance.h"

CQuestInstance::CQuestInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CQuest(pDevice, pContext)
{
}

CQuestInstance::CQuestInstance(const CQuestInstance& rhs)
	:CQuest(rhs)
{
}


HRESULT CQuestInstance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuestInstance::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) 
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuestInstance::Bind_ShaderResources()
{
	return S_OK;
}

CQuestInstance* CQuestInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuestInstance* pInstance = new CQuestInstance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuestInstance");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuestInstance::Clone(void* pArg, CGameObject* pOwner)
{
	CQuestInstance* pInstance = new CQuestInstance(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Header");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuestInstance::Free()
{
	__super::Free();
}

void CQuestInstance::Describe_Entity()
{
}
