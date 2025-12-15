#include "pch.h"
#include "Enemy_Detection.h"
#include "GameInstance.h"
#include "Monster.h"

CEnemy_Detection::CEnemy_Detection(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CEnemy_Detection::CEnemy_Detection(const CEnemy_Detection& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CEnemy_Detection::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy_Detection::Initialize(void* pArg)
{
	ENEMY_DETECTION_DESC* pDesc = static_cast<ENEMY_DETECTION_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_fOrigin_Size = _float3(1.f, 1.f, 1.f);
	m_fSize.x = m_fOrigin_Size.x;
	m_fSize.y = m_fOrigin_Size.y;
	_float3 scals = _float3(1.f, 1.f, 1.f);
	m_pTransformCom->Set_Scale(scals);
	m_fSize.z = 1.f;
	m_fAlpha = 1.f;
	m_bStep1 = true;
	m_vImageposi = _float4(0.3f, 0.3f, 0.4f, 0.4f);

	return S_OK;
}

void CEnemy_Detection::Priority_Update(_float fTimeDelta)
{

	XMVECTOR MonsterPos = static_cast<CMonster*>(m_pOwner)->Get_WorldPostion();

	_float fOffsetY = 3.0f; // 적절한 Y 오프셋 값 (몬스터 머리 위로 올리기 위함)

	XMVECTOR HeadPos = XMVectorAdd(MonsterPos, XMVectorSet(0.f, fOffsetY, 0.f, 0.f));

	m_vPosition = HeadPos;
	m_fSize = _float3(2.f, 2.f, 2.f);
	m_vImageposi = _float4(0.5f, 0.5f, 1.f, 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, m_vPosition);
}

void CEnemy_Detection::Update(_float fTimeDelta)
{
	if (m_bActive == true)
	{
		m_fTime += fTimeDelta;

		if (m_fTime >= 2.f)
		{
			if (m_bStep1 == true)
			{
				m_bStep1 = false;
				m_bStep2 = true;
				m_bStep3 = false;
				m_fTime = 0.f;
			}
			else if (m_bStep2 == true)
			{
				m_bStep1 = false;
				m_bStep2 = false;
				m_bStep3 = true;
				m_fTime = 0.f;
			}
			else if (m_bStep3 == true)
			{
				m_bStep1 = false;
				m_bStep2 = false;
				m_bStep3 = true;
				m_fAlpha -= fTimeDelta;
			}
		}
	}
	else if (m_bActive == false)
	{
		m_fTime = 0.f;
		m_bStep1 = true;
		m_bStep2 = false;
		m_bStep3 = false;
		m_bSizeUp = false;
		m_fAlpha = 1.f;
		SizeUpX(m_fOrigin_Size.x);
		SizeUpY(m_fOrigin_Size.y);
		m_vImageposi.z = 75.f;
		m_vImageposi.w = 75.f;
	}

	if (m_bStep3 == true)
	{
		if (m_bSizeUp == false)
		{
			SizeUpX(m_fSize.x + 1.f);
			SizeUpY(m_fSize.y + 1.f);
			m_vImageposi.z += 0.4f;
			m_vImageposi.w += 0.4f;
			m_bSizeUp = true;
		}
	}

	if (m_fSize.x != m_fOrigin_Size.x)
	{
		SizeUpdate(-fTimeDelta);
		m_vImageposi.z -= fTimeDelta;
		m_vImageposi.w -= fTimeDelta;
	}

	if (m_fAlpha <= 0.f)
		m_fAlpha = 0.f;

}

void CEnemy_Detection::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CEnemy_Detection::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ENEMY_DETECTION)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Detection::Render_Disolve()
{
	return S_OK;
}

void CEnemy_Detection::SizeUpX(_float fX)
{
	m_fSize.x = fX;
}

void CEnemy_Detection::SizeUpY(_float fY)
{
	m_fSize.y = fY;
}

void CEnemy_Detection::SizeUpdate(_float fSizeX)
{
	m_fSize.x += fSizeX;
	m_fSize.y += fSizeX;
	m_vImageposi.z += fSizeX;
	m_vImageposi.w += fSizeX;
	if (m_fSize.x - m_fOrigin_Size.x <= fSizeX)
	{
		m_fSize.x = m_fOrigin_Size.x;
		m_fSize.y = m_fOrigin_Size.y;
		m_vImageposi.z = 1.f;
		m_vImageposi.w = 1.f;
	}
}

void CEnemy_Detection::Set_Active(_bool bAtcive)
{
	m_bActive = bAtcive;
}

_bool CEnemy_Detection::Get_Step()
{
	return m_bStep3;
}

HRESULT CEnemy_Detection::Ready_Components()
{
	__super::Ready_Components(nullptr);

	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_Ring_Aggroed_Burst"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_NPC_WhiteFrameBlip"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_NPC_Alert"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_Ring_Alert_Fill"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_Ring_Alert_Yellow"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom4), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_Ring_Aggro_Blip"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom5), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Detection::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamaraPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom3->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom4->Bind_ShaderResource(m_pShaderCom, "g_Texture4", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom5->Bind_ShaderResource(m_pShaderCom, "g_Texture5", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iStep1", &m_bStep1, sizeof(_bool))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iStep2", &m_bStep2, sizeof(_bool))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iStep3", &m_bStep3, sizeof(_bool))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImageposi, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_fSize, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CEnemy_Detection* CEnemy_Detection::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Detection* pInstance = new CEnemy_Detection(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnemy_Detection");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Detection::Clone(void* pArg, CGameObject* pOwner)
{
	CEnemy_Detection* pInstance = new CEnemy_Detection(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnemy_Detection");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEnemy_Detection::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pDiffuse_TextureCom4);
	SAFE_RELEASE(m_pDiffuse_TextureCom5);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pShaderCom);

}

#ifdef _DEBUG
void CEnemy_Detection::Describe_Entity()
{
}
#endif // _DEBUG
