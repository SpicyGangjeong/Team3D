#include "pch.h"
#include "Interaction_Key.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "MapElement_Interactable.h"

CInteraction_Key::CInteraction_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteraction_Key::CInteraction_Key(const CInteraction_Key& Prototype)
	: CGameObject(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CInteraction_Key::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Key::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_fSize = _float3(1.f, 1.f, 1.f);
	m_fAlpha = 1.f;] 
	m_vUV = ComputeUV('Z');
	m_fViewPort = _float2(_float(g_iWinSizeX), _float(g_iWinSizeY));
	return S_OK;
}

void CInteraction_Key::Priority_Update(_float fTimeDelta)
{
}

void CInteraction_Key::Update(_float fTimeDelta)
{
	ReLockOnTarget();
}

void CInteraction_Key::Late_Update(_float fTimeDelta)
{
	if (m_bActive == true)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
}

HRESULT CInteraction_Key::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::INTERACTION_OBJECT)))) {
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

void CInteraction_Key::Set_Active(_bool bAtcive)
{
	m_bActive = bAtcive;
}

_float4 CInteraction_Key::ComputeUV(_tchar Alphabet)
{
	_uint Number = CMyTools::AlphabetToInt(Alphabet);

	_float2 fIamge_Size = { 320.f, 384.f };

	_uint iXCount = 5;
	_uint iYCount = 6;

	_float frameWidth = 64.f;
	_float frameHeight = 64.f;

	_uint frameX = Number % iXCount;
	_uint frameY = Number / iXCount;

	_float2 UVStart;
	UVStart.x = frameX * frameWidth / fIamge_Size.x;
	UVStart.y = frameY * frameHeight / fIamge_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (frameWidth / fIamge_Size.x);
	UVEnd.y = UVStart.y + (frameHeight / fIamge_Size.y);

	_float4 UV{};

	return UV = _float4(UVStart.x, UVStart.y, UVEnd.x, UVEnd.y);
}

void CInteraction_Key::ReLockOnTarget()
{
	m_pInfoInstance->Get_LockOnInfo(m_LockOnInteractionObject);
	if (m_LockOnInteractionObject.pUnit != nullptr)
		return;

	if (nullptr != m_LockOnInteractionObject.pInteractive)
	{
		if (true == m_LockOnInteractionObject.pInteractive->isDead()) {
			m_bActive = false;
			m_LockOnInteractionObject.pInteractive = nullptr;
		}
		else {
			m_bActive = true;
			m_fSize = _float3(40.f, 40.f, 1.f);
			_vector Position = XMVectorSet(m_LockOnInteractionObject.pInteractive->Get_WorldPostion().m128_f32[0],
				m_LockOnInteractionObject.pInteractive->Get_WorldPostion().m128_f32[1] + 0.5f,
				m_LockOnInteractionObject.pInteractive->Get_WorldPostion().m128_f32[2],
				m_LockOnInteractionObject.pInteractive->Get_WorldPostion().m128_f32[3]);
			m_pTransformCom->Set_State(STATE::POSITION, Position);
		}
	}
	else{
		m_bActive = false;
	}
}

HRESULT CInteraction_Key::Ready_Components()
{
	__super::Ready_Components(nullptr);

	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Atlas_Keyboard"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInteraction_Key::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_fSize, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fViewport", &m_fViewPort, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CInteraction_Key* CInteraction_Key::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInteraction_Key* pInstance = new CInteraction_Key(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CInteraction_Key::Clone(void* pArg, CGameObject* pOwner)
{
	CInteraction_Key* pInstance = new CInteraction_Key(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CInteraction_Key::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pShaderCom);

}
#ifdef _DEBUG

void CInteraction_Key::Describe_Entity()
{
}

#endif // _DEBUG
