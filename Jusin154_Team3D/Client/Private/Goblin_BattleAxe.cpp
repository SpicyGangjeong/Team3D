#include "pch.h"
#include "Goblin_BattleAxe.h"

#include "GameInstance.h"
#include "TrailObject.h"
#include "EffectParts.h"

CGoblin_BattleAxe::CGoblin_BattleAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CGoblin_BattleAxe::CGoblin_BattleAxe(const CGoblin_BattleAxe& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CGoblin_BattleAxe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_BattleAxe::Initialize(void* pArg)
{
	GOBLIN_BATTLEAXE_DESC* pDesc = static_cast<GOBLIN_BATTLEAXE_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_BattleAxe::Priority_Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();
	_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}

	m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CGoblin_BattleAxe::Update(_float fTimeDelta)
{
	if (m_bDisolve) {
		m_fDisolveTime += fTimeDelta;
		if (m_fDisolveTime >= 1.f)
		{
			m_fDisolveTime = 0.f;
			m_bDisolve = false;
			m_bVisible = false;
		}
	}

	_matrix AxeMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bone"));


	for (int i = 0; i < 3; ++i) {
		AxeMat.r[i] = XMVector3Normalize(AxeMat.r[i]);
	}

	XMStoreFloat4x4(&m_vAxeMat, AxeMat * m_pTransformCom->Get_XMWorldMatrix());

}

void CGoblin_BattleAxe::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CGoblin_BattleAxe::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(Render_Disolve())) {
		return E_FAIL;
	}


	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SPECTOR_WEAPON)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	{
		_bool bDisolve = false;
		_float zero = 0.f;
		m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &zero, sizeof(_float));
	}

	return S_OK;
}

HRESULT CGoblin_BattleAxe::Render_Disolve()
{
	if (FLT_EPSILON3 * 10 < m_fDisolveTime)
	{
		_bool bDisolve = true;
		_float fDisolveAmount = 0.1f;
		_float fDisolveEdgeWidth = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &m_fDisolveTime, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveAmount", &fDisolveAmount, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveEdgeWidth", &fDisolveEdgeWidth, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_NOISE_05"), "g_DeadDisolveTexture"))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_BURN_VERTICAL"), "g_DisolveTexture"))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CGoblin_BattleAxe::Ready_Components()
{
	__super::Ready_Components(nullptr);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Goblin_BattleAxe_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoblin_BattleAxe::Bind_ShaderResources()
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


CGoblin_BattleAxe* CGoblin_BattleAxe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_BattleAxe* pInstance = new CGoblin_BattleAxe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_BattleAxe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CGoblin_BattleAxe::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_BattleAxe* pInstance = new CGoblin_BattleAxe(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_BattleAxe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_BattleAxe::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CGoblin_BattleAxe::Describe_Entity()
{
}

#endif // _DEBUG
