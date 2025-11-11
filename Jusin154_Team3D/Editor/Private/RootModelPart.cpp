#include "pch.h"
#include "RootModelPart.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "ModelParts.h"

CRootModelPart::CRootModelPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CRootModelPart::CRootModelPart(const CRootModelPart& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CRootModelPart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRootModelPart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CRootModelPart::Priority_Update(_float fTimeDelta)
{
}

void CRootModelPart::Update(_float fTimeDelta)
{
	if (!m_pMainModel)
		return;

	Update_Anim(fTimeDelta);
}

void CRootModelPart::Late_Update(_float fTimeDelta)
{
}

HRESULT CRootModelPart::Render()
{
	return S_OK;
}

void CRootModelPart::Update_Anim(_float fTimeDelta)
{
	for (size_t i = 0; i < m_ModelParts.size(); i++)
	{
		m_ModelParts[i]->Get_Component<CModel>()->Set_PlayAnim(m_pMainModel->Get_PlayAnim());
		m_ModelParts[i]->Get_Component<CModel>()->Set_AnimationIndex(m_pMainModel->Get_AnimIndex());
		m_ModelParts[i]->Get_Component<CModel>()->Play_Animation(fTimeDelta);
	}
}

void CRootModelPart::Push_ModelParts(CModelParts* ModelParts)
{
	m_ModelParts.push_back(ModelParts);
	SAFE_ADDREF(ModelParts);
	for (auto& ModelParts : m_ModelParts)
	{
		ModelParts->Get_Component<CModel>()->Set_AnimationIndex(0);
		ModelParts->Get_Component<CModel>()->Reset_CurrentTrackPosition();
	}
}

void CRootModelPart::Push_MainModel(CModel* MainModel)
{
	m_pMainModel = MainModel;
	SAFE_ADDREF(MainModel);
}

HRESULT CRootModelPart::Ready_Components()
{
	return S_OK;
}

HRESULT CRootModelPart::Bind_ShaderResources()
{
	return S_OK;
}

CRootModelPart* CRootModelPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRootModelPart* pInstance = new CRootModelPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRootModelPart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRootModelPart::Clone(void* pArg, CGameObject* pOwner)
{
	CRootModelPart* pInstance = new CRootModelPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRootModelPart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRootModelPart::Free()
{
	__super::Free();

	for (auto& ModelParts : m_ModelParts)
	{
		SAFE_RELEASE(ModelParts);
	}
	m_ModelParts.clear();
	SAFE_RELEASE(m_pMainModel);
}

void CRootModelPart::Describe_Entity()
{
	GUI::Text("HI");
}
