#include "pch.h"
#include "ReparoObject.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Unit.h"
#include "PartObject.h"

CReparoObject::CReparoObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CReparoObject::CReparoObject(const CReparoObject& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CReparoObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReparoObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pModelCom->Set_AnimationIndex(0);



	return S_OK;
}

void CReparoObject::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	Describe_Entity();
}

void CReparoObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
}

void CReparoObject::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CReparoObject::Render()
{
	if (!m_pModelCom) {
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT);
	if (true == m_bDrawOutLine) {
		iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::OUTLINE_WRITE);
	}

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

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(32, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	if (m_bDrawOutLine) {
		Render_OutLine();
	}

	return S_OK;
}

HRESULT CReparoObject::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
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

	Compute_Depth();
	_float fCamFar = *m_pGameInstance->Get_CurrentCameraFar();
	_float fRatio = CMyTools::Saturate((m_fCamDepth / (fCamFar * fCamFar)));
	m_fOutLineThickness = CMyTools::Lerp_f1D(0.024f, 0.5f, fRatio);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutLineColor", &m_vOutLineColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineThickness", &m_fOutLineThickness, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineScale", &m_fOutLineScale, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLinePower", &m_fOutLinePower, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()
		)))
		{
			return E_FAIL;
		}


		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::OUTLINE_READ)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(32, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CReparoObject::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(Add_Component<CTransform>(g_iStaticLevel, &m_pTransformCom, &Desc))) {
		return E_FAIL;
	}

	///* Com_Model */
	//if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerA_Model"),
	//	reinterpret_cast<CComponent**>(&m_pModelCom)))) {
	//	return E_FAIL;
	//}

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerB_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CReparoObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
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

CReparoObject* CReparoObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReparoObject* pInstance = new CReparoObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CReparoObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CReparoObject::Clone(void* pArg, CGameObject* pOwner)
{
	CReparoObject* pInstance = new CReparoObject(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CReparoObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CReparoObject::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CReparoObject::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);

	if (m_pModelCom->Get_AnimSize() <= 0)
		return;

	m_pTransformCom->Describe_Entity();
	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
		}
	}

	if (GUI::BeginTabBar("Anim_List"))
	{
		if (GUI::BeginTabItem("ReparoObject"))
		{
			for (_uint i = 0; i < m_pModelCom->Get_AnimSize(); i++)
			{
				if (GUI::Button(m_pModelCom->Get_AnimList(i)))
				{
					m_pModelCom->Set_AnimationIndex(i);
				}
			}

			GUI::EndTabItem();
		}
		GUI::EndTabBar();
	}

	_char label[24];

	sprintf_s(label, "%s %d", "AnimIndex", m_pModelCom->Get_AnimIndex());

	GUI::Text(label);

	_float AnimTrack = m_pModelCom->Get_CurrentTrackPosition();
	if (GUI::DragFloat("AnimTrack", &AnimTrack))
	{
		if (AnimTrack >= 0.f)
			m_pModelCom->Set_CurrentTrackPosition(AnimTrack);
	}

	_float AnimRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	GUI::DragFloat("AnimRatio", &AnimRatio);

	_float AnimSpeed = m_pModelCom->Get_AnimSpeed();
	GUI::DragFloat("AnimSpeed", &AnimSpeed, 0.1f);
	m_pModelCom->Set_AnimSpeed(AnimSpeed);

	_bool bPlayAnim = m_pModelCom->Get_PlayAnim();
	if (GUI::Checkbox("Play Anim", &bPlayAnim))
	{
		m_pModelCom->Set_PlayAnim(bPlayAnim);
	}

	GUI::End();
}

#endif // _DEBUG
