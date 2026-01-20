#include "pch.h"
#include "Broom_TargetGate.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CBroom_TargetGate::CBroom_TargetGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBroom_TargetGate::CBroom_TargetGate(const CBroom_TargetGate& rhs)
	: CGameObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CBroom_TargetGate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_TargetGate::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_fSize = _float3(1.f, 1.f, 1.f);
	m_fAlpha = 1.f;
	m_fViewPort = _float2(_float(g_iWinSizeX), _float(g_iWinSizeY));
	m_fFontSize = 0.f;

	m_pInfoInstance->Add_Event(TEXT("BroomTargetGate"), [this](void* p) {this->Set_TargetPosition(*reinterpret_cast<_vector*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("Race_Start"), [this](void* p) {this->Visibie(); });
	m_bVisible = false;
	return S_OK;
}

void CBroom_TargetGate::Priority_Update(_float fTimeDelta)
{
}

void CBroom_TargetGate::Update(_float fTimeDelta)
{
	if (m_bVisible != true)
		return;

	m_fSize = _float3(70.f, 70.f, 1.f);
	m_vPlayerPos = m_pInfoInstance->Get_PalyerPos();

	if (m_vActive == true)
	{
		_vector Target = XMLoadFloat4(&m_vTargetPos);
		_vector PlayerPos = XMLoadFloat4(&m_vPlayerPos);
		_vector Dir = XMVectorSubtract(Target, PlayerPos);
		_float Dist = XMVectorGetX(XMVector3Length(Dir));
		m_Distance = to_wstring(_int(Dist));
	}
}

void CBroom_TargetGate::Late_Update(_float fTimeDelta)
{
	if (m_bVisible != true)
		return;
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CBroom_TargetGate::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::BROOMTARGETGATE)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	_vector Pos = XMVectorAdd(XMLoadFloat4(&m_vTargetPos), XMVectorSet(0.f, -3.f, 0.f, 0.f));

	m_pGameInstance->Perspective_Render_Text(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW),
		m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ),
		TEXT("UI_size20"),
		m_Distance.c_str(),
		Pos);

	return S_OK;
}

_vector CBroom_TargetGate::Get_WorldPostion()
{
	return m_pOwner->Get_WorldPostion();
}

HRESULT CBroom_TargetGate::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamaraPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
	{
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
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

HRESULT CBroom_TargetGate::Ready_Components()
{
	__super::Ready_Components(nullptr);

	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_Map_RaceGate"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CBroom_TargetGate::Set_TargetPosition(_fvector TargetPos)
{
	XMStoreFloat4(&m_vTargetPos ,TargetPos);
	m_pTransformCom->Set_State(STATE::POSITION, TargetPos);
	m_vActive = true;
}

void CBroom_TargetGate::Visibie()
{
	m_bVisible = !m_bVisible;
}

CBroom_TargetGate* CBroom_TargetGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_TargetGate* pInstance = new CBroom_TargetGate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom_TargetGate");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom_TargetGate::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom_TargetGate* pInstance = new CBroom_TargetGate(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_TargetGate");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom_TargetGate::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pShaderCom);

}

#ifdef _DEBUG
void CBroom_TargetGate::Describe_Entity()
{
}
#endif // _DEBUG
