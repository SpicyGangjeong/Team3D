#include "pch.h"
#include "RootModelPart.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "ModelParts.h"
#include "Body.h"
#include "Head.h"
#include "Hair.h"

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

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CRootModelPart::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CRootModelPart::Update(_float fTimeDelta)
{
	Update_Anim(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CRootModelPart::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CRootModelPart::Render()
{
	return S_OK;
}

void CRootModelPart::Update_Anim(_float fTimeDelta)
{
	for (auto& ModelParts : m_ModelParts)
	{
		CModel* pModel = ModelParts->Get_Component<CModel>();
		if (pModel)
		{
			pModel->Set_PlayAnim(m_pMainModel->Get_PlayAnim());
			pModel->Set_AnimationIndex(m_pMainModel->Get_AnimIndex());
			pModel->Set_CurrentTrackPosition(m_pMainModel->Get_CurrentTrackPosition());
			pModel->Play_Animation(fTimeDelta);
		}
	}
}


HRESULT CRootModelPart::Ready_Components()
{
	return S_OK;
}

HRESULT CRootModelPart::Ready_PartObjects()
{
	CModelParts::PARTS_OBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;
	PartsDesc.pModelPrototypeTag = m_PrototypeModelName;

	CModelParts* pModelPartsObject = { nullptr };

	if (FAILED(Add_PartObject<CBody>("Human_Body", ENUM_CLASS(LEVEL::STATIC), reinterpret_cast<CBody**>(&pModelPartsObject), &PartsDesc)))
		return E_FAIL;

	m_ModelParts.push_back(pModelPartsObject);

	if (FAILED(Add_PartObject<CHead>("Human_Head", ENUM_CLASS(LEVEL::STATIC), reinterpret_cast<CHead**>(&pModelPartsObject), &PartsDesc)))
		return E_FAIL;

	m_ModelParts.push_back(pModelPartsObject);

	if (FAILED(Add_PartObject<CHair>("Human_Hair", ENUM_CLASS(LEVEL::STATIC), reinterpret_cast<CHair**>(&pModelPartsObject), &PartsDesc)))
		return E_FAIL;

	m_ModelParts.push_back(pModelPartsObject);

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
	
}
