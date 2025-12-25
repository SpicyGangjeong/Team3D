#include "pch.h"
#include "DummyObject.h"

#include "GameInstance.h"
#include "Camera_Debug.h"

CDummyObject::CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CDummyObject::CDummyObject(const CDummyObject& Prototype)
	: CUnit(Prototype)
{
}

HRESULT CDummyObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummyObject::Initialize(void* pArg)
{
	PARTS_OBJECT_DESC* pDesc = static_cast<PARTS_OBJECT_DESC*>(pArg);

	m_strModelPrototypeTag = pDesc->pModelPrototypeTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CDummyObject::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
}

void CDummyObject::Update(_float fTimeDelta)
{
	Describe_Entity();
	m_pModelCom->Play_Animation(fTimeDelta,m_pTransformCom);
}

void CDummyObject::Late_Update(_float fTimeDelta)
{
#ifdef 기무리
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#else
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
#endif // 기무리

}

HRESULT CDummyObject::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()
		)))
		{
			return E_FAIL;
		}

if (m_pModelCom->Get_Type() == MODEL::PBR_ANIM)
{
	if (FAILED(m_pModelCom->Begin(i, m_pShaderCom, false))) {
		return E_FAIL;
	}

	m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
	m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);
	if (FAILED(Bind_ShaderParameters(i))) {
		return E_FAIL;
	}

}
else {

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
		return E_FAIL;
	}

	m_pModelCom->Bind_OutPut_SRV_VS(31, 0);
	m_pModelCom->Bind_OutPut_SRV_VS_Prev(32, 0);
}

if (FAILED(m_pModelCom->Render(i))) {
	return E_FAIL;
}
	}

	return S_OK;
}

HRESULT CDummyObject::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	if (m_pModelCom->Get_Type() == MODEL::PBR_ANIM)
	{
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
			reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
			return E_FAIL;
		}
	}
	else {
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
			reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CDummyObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}
#ifdef 기무리
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
#endif // 기무리

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummyObject::Bind_ShaderParameters(_uint iMeshOrder)
{
	_bool bUseColorMixer = false;

	_uint iColorParam = { UINT_MAX };
	_float fMixerFactor = { FLT_MAX };
	_uint iColorMixerMethod = { 0 };

	switch (MESH_ORDER(iMeshOrder))
	{
	case CDummyObject::HAIR_MAIN:
	case CDummyObject::HEAD_EYELASH:
	case CDummyObject::HAIR_SUB:
		bUseColorMixer = true;
		iColorParam = 0x2E2E2E;
		fMixerFactor = 0.9f;
		iColorMixerMethod = 1;
		break;
	case CDummyObject::LOWER:
		bUseColorMixer = true;
		iColorParam = 0x292557;
		fMixerFactor = 0.5f;
		iColorMixerMethod = 1;
		break;
	case CDummyObject::SHOES:
		bUseColorMixer = true;
		iColorParam = 0x614242;
		fMixerFactor = 0.5f;
		iColorMixerMethod = 1;
		break;
	case CDummyObject::UPPER:
		bUseColorMixer = true;
		iColorParam = 0xBFAC29;
		fMixerFactor = 0.658333f;
		iColorMixerMethod = 1;
		break;
	default:
		break;
	}
	if (true == bUseColorMixer) {
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iPackedBlendColor", &iColorParam, sizeof(_uint)))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fMixerFactor", &fMixerFactor, sizeof(_float)))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iColorMixerMethod", &iColorMixerMethod, sizeof(_uint)))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

CDummyObject* CDummyObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummyObject* pInstance = new CDummyObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummyObject");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CDummyObject::Clone(void* pArg, CGameObject* pOwner)
{
	CDummyObject* pInstance = new CDummyObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummyObject");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CDummyObject::Free()
{
	__super::Free();
}

void CDummyObject::Describe_Entity()
{
	if (nullptr != m_pModelCom) {
		m_pModelCom->Describe_Entity();
	}
	if (nullptr != m_pShaderCom) {
		if (GUI::Button("RefreshShader")) {
			if (FAILED(m_pShaderCom->Shader_Refresh())) {
				assert(false);
			}
		}
	}
}
