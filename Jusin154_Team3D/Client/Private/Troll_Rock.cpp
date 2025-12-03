#include "pch.h"
#include "Troll_Rock.h"

#include "GameInstance.h"
#include "TrailObject.h"
#include "EffectParts.h"

CTroll_Rock::CTroll_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CTroll_Rock::CTroll_Rock(const CTroll_Rock& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CTroll_Rock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTroll_Rock::Initialize(void* pArg)
{
	TROLLROCK_DESC* pDesc = static_cast<TROLLROCK_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CTroll_Rock::Priority_Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();

	if (m_bAttach)
	{
		_matrix socketMatrix = {};

		socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

		for (int i = 0; i < 3; ++i) {
			socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
		}

		m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	}
	else
	{
		_matrix world = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		m_pTransformCom->Set_WorldMatrix(world);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}


#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CTroll_Rock::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Up(DIK_U))
	{
		m_bAttach = false;
	}

}

void CTroll_Rock::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CTroll_Rock::Render()
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

HRESULT CTroll_Rock::Ready_Components()
{

	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Troll_Rock_Big_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTroll_Rock::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
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


CTroll_Rock* CTroll_Rock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Rock* pInstance = new CTroll_Rock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Rock");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTroll_Rock::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Rock* pInstance = new CTroll_Rock(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Rock");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Rock::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CTroll_Rock::Describe_Entity()
{
	GUI::Begin("Rock");
	GUI::Text("X %.2f,Y%.2f,Z%.2f", m_pTransformCom->Get_Scale().x, m_pTransformCom->Get_Scale().y, m_pTransformCom->Get_Scale().z);

	static _float Scale = 1.f;
	if (GUI::DragFloat("Scale", &Scale,0.01f))
	{
		_float3 vScale = { Scale,Scale ,Scale };
		m_pTransformCom->Set_Scale(vScale);
	}
	GUI::End();
}

#endif // _DEBUG
