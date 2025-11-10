#include "pch.h"
#include "EditEffect.h"

#include "GameInstance.h"


CEditEffect::CEditEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CEditEffect::CEditEffect(const CEditEffect& rhs)
	: CEffectObject(rhs)
{
}

HRESULT CEditEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEditEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	return S_OK;
}

void CEditEffect::Priority_Update(_float fTimeDelta)
{

}

void CEditEffect::Update(_float fTimeDelta)
{
	if (m_pInstance_ModelCom == nullptr)
		return;

	m_pInstance_ModelCom->Drop(fTimeDelta);
}

void CEditEffect::Late_Update(_float fTimeDelta)
{

	if (m_pInstance_ModelCom == nullptr)
		return;

	_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);

	if(m_isBlur == true)
		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this, *vPos, m_pTransformCom->Get_Radius());

	m_pGameInstance->Add_RenderGroup(m_eRenderOrder, this, *vPos, m_pTransformCom->Get_Radius());
}

HRESULT CEditEffect::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_MODEL, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CEditEffect* CEditEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEditEffect* pInstance = new CEditEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEditEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEditEffect::Clone(void* pArg, CGameObject* pOwner)
{
	CEditEffect* pInstance = new CEditEffect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEditEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEditEffect::Free()
{
	__super::Free();
}

void CEditEffect::Describe_Entity()
{
	//여기서 모델, 텍스쳐, 선택할 수 있도록 함

	CInstance_Model::INSTANCE_DESC InstanceDesc = {};

	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vLifeTime = _float2(1.f, 3.f);
	InstanceDesc.vRange = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vSizeMin = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vSizeMax = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vSpeed = _float2(0.f, 0.f);

	const char* pRenderNames[] = {"PRIORITY" , "SHADOW", "NONBLEND", "BLUR" , "NONLIGHT", "BLEND" , "UI" };

	int iCurrentItem = static_cast<int>(m_eRenderOrder);

	if (ImGui::Combo("Render Order", &iCurrentItem, pRenderNames, ENUM_CLASS(RENDER::END)))
	{
		m_eRenderOrder = static_cast<RENDER>(iCurrentItem);
	}


	GUI::Checkbox("Diffuse", &m_isDiffuse);
	GUI::Checkbox("Masking", &m_isMasking);
	GUI::Checkbox("Dissolve", &m_isDissolve);
	GUI::Checkbox("Noise", &m_isNoise);
	GUI::Checkbox("UVMove", &m_isUVMove);
	GUI::Checkbox("Blur", &m_isBlur);


	GUI::ColorEdit4("Color", (_float*)&m_vColor);

	GUI::DragFloat("BlurIntensity", &m_fBlurIntensity , 0.01f , 0.f , 1.f);

	GUI::ColorEdit4("Emissive", (_float*)&m_vEmissive);
	GUI::InputFloat("ColorTargetOption", &m_fColorOption);

	GUI::DragFloat2("vUVGainAmount", (_float*)&m_vUVGainAmount , 0.01f);


	//UVMove를 각각의 텍스쳐마다 적용할 수 있어야함 
	//노이즈 적용하기
	//디졸브 적용하기
	//라이프 타임에 의해서가 아닌 공용시간으로 제어해야함
	//UV잘라서 적용할 수 있도록

	//블러 웨이트도 설정할 수 있도록


	if (GUI::TreeNode("MODEL"))
	{
		m_pGameInstance->Asset_Description<CInstance_Model>(ENUM_CLASS(LEVEL::EFFECT), "INSTANCE_MODEL", (CComponent**)&m_pInstance_ModelCom, &InstanceDesc, this);

		if (m_pInstance_ModelCom != nullptr)
		{
			m_pInstance_ModelCom->Describe_Entity();
		}

		GUI::TreePop();
	}


	if (m_isDiffuse == true)
	{
		if (GUI::TreeNode("DIFFUSE"))
		{
			m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DIFFUSE_TEXTURE", (CComponent**)&m_pDiffuse_TextureCom, nullptr, this);

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}


	if (m_isMasking == true)
	{
		if (GUI::TreeNode("MASKING"))
		{
			m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "MASKING_TEXTURE", (CComponent**)&m_pMasking_TextureCom, nullptr, this);

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}

	if (m_isDissolve == true)
	{
		if (GUI::TreeNode("DISOLVE"))
		{
			m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DISOLVE_TEXTURE", (CComponent**)&m_pNoise_TextureCom, &InstanceDesc, this);

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}

	}
	if (m_isNoise == true)
	{
		if (GUI::TreeNode("NOISE"))
		{
			m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NOISE_TEXTURE", (CComponent**)&m_pNoise_TextureCom, nullptr, this);

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}


}
