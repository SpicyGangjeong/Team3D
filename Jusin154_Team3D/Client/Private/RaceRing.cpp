#include "pch.h"
#include "RaceRing.h"

#include "BroomRaceManager.h"
#include "GameInstance.h"

CRaceRing::CRaceRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRaceRing::CRaceRing(const CRaceRing& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CRaceRing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRaceRing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}
	m_pBroomRaceManager = static_cast<RACERING_DESC*>(pArg)->pBroomRaceManager;
	m_iIndex = static_cast<RACERING_DESC*>(pArg)->iIndex;
	_float X = m_pGameInstance->Real_Random_Float(-200.f, 200.f);
	_float Y = m_pGameInstance->Real_Random_Float(15.f, 45.f);
	_float Z = m_pGameInstance->Real_Random_Float(-15.f, 15.f);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((_float)m_iIndex *100.f, Y, Z, 1.f));

	m_pBroomRaceManager->Push_RaceRing(this);

	return S_OK;
}

void CRaceRing::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CRaceRing::Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();

}

void CRaceRing::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CRaceRing::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CRaceRing::Ready_Components()
{
	__super::Ready_Components(nullptr);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_RaceRing_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRaceRing::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}

	return S_OK;
}



CRaceRing* CRaceRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRaceRing* pInstance = new CRaceRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRaceRing");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CRaceRing::Clone(void* pArg, CGameObject* pOwner)
{
	CRaceRing* pInstance = new CRaceRing(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRaceRing");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRaceRing::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CRaceRing::Describe_Entity()
{

}

#endif // _DEBUG
