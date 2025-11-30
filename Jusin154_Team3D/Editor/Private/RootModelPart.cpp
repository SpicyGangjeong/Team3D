#include "pch.h"
#include "RootModelPart.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
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

	if (FAILED(Ready_Components(pArg)))
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
	if (!m_pMainModel)
		return;

	for (auto& ModelParts : m_ModelParts)
	{
		CModel* pModel = ModelParts->Get_Component<CModel>();
		if (pModel)
		{
			pModel->Set_PlayAnim(m_pMainModel->Get_PlayAnim());
			pModel->Set_AnimSpeed(m_pMainModel->Get_AnimSpeed());
			//pModel->ComputeAnimation();
			pModel->Play_Animation(fTimeDelta,m_pTransformCom);
		}
	}
}

void CRootModelPart::Set_MainModel(CModel* Model)
{
	if (m_pMainModel)
	{
		SAFE_RELEASE(m_pMainModel);
	}
	m_pMainModel = Model;
}

void CRootModelPart::Change_Model(_uint iIndex)
{
	if (m_pMainModel)
	{
		m_iAnimIndex = m_pMainModel->Get_AnimIndex();
		m_fTrackPosition = m_pMainModel->Get_CurrentTrackPosition();
	}

	m_ModelParts[iIndex]->Change_Model(m_PrototypeModelName);

	CModel* pModel = m_ModelParts[iIndex]->Get_Component<CModel>();

	pModel->Set_AnimationIndex(m_iAnimIndex);
	pModel->Set_CurrentTrackPosition(m_fTrackPosition);         
	//pModel->ComputeAnimation();
	pModel->Play_Animation(0.f, m_pTransformCom);

	if (iIndex == ENUM_CLASS(PARTSTYPE::BODY))
	{
		Set_MainModel(m_ModelParts[iIndex]->Get_Component<CModel>());
	}
	
}

void CRootModelPart::Set_Animation(_uint iAnimIndex)
{
	m_pMainModel->Set_AnimationIndex(iAnimIndex);
	m_pMainModel->Set_CurrentTrackPosition(0.f);
	//m_pMainModel->ComputeAnimation();
	m_pMainModel->Play_Animation(0.f, m_pTransformCom);
	for (auto& PartObject : m_ModelParts)
	{
		CModel* pModel = PartObject->Get_Component<CModel>();
		if (pModel)
		{
			pModel->Set_AnimationIndex(m_pMainModel->Get_AnimIndex());
			pModel->Set_CurrentTrackPosition(m_pMainModel->Get_CurrentTrackPosition());
			//pModel->ComputeAnimation();
			pModel->Play_Animation(0.f, m_pTransformCom);
		}
	}
}

void CRootModelPart::Set_Texture(_uint iIndex, const _wchar* Texture)
{
	m_ModelParts[iIndex]->Set_Texture(Texture);
}

HRESULT CRootModelPart::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);

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
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CRootModelPart::Clone(void* pArg, CGameObject* pOwner)
{
	CRootModelPart* pInstance = new CRootModelPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRootModelPart");
		SAFE_RELEASE(pInstance);
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
