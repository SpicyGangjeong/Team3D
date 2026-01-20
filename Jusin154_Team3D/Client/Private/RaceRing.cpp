#include "pch.h"
#include "RaceRing.h"

#include "BroomRaceManager.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "EffectParts.h"

CRaceRing::CRaceRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CRaceRing::CRaceRing(const CRaceRing& Prototype)
	: CContainerObject(Prototype)
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

	if (FAILED(Ready_Parts())) {
		return E_FAIL;
	}


	//m_iIndex = static_cast<RACERING_DESC*>(pArg)->iIndex;
	//_float X = m_pGameInstance->Real_Random_Float(-200.f, 200.f);
	//_float Y = m_pGameInstance->Real_Random_Float(15.f, 60.f);
	//_float Z = m_pGameInstance->Real_Random_Float(-50.f, 50.f);

	RACERING_DESC* pDesc = static_cast<RACERING_DESC*>(pArg);

	m_pBroomRaceManager = pDesc->pBroomRaceManager;
	_float3 vRotation = pDesc->vRotation;
	m_pTransformCom->Rotation(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(pDesc->vPosition.x, pDesc->vPosition.y, pDesc->vPosition.z, 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pBroomRaceManager->Push_RaceRing(this);

	return S_OK;
}

void CRaceRing::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG

	__super::Priority_Update(fTimeDelta);
}

void CRaceRing::Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();

	World_to_ScreenUI();

	__super::Update(fTimeDelta);

	if (m_bTarget == true)
	{
		m_pRingEffect->Set_Visible(true);
		m_pRingEffect->Get_Component<CTransform>()->Set_WorldMatrix(m_pTransformCom->Get_XMWorldMatrix());
	}
	else {
		m_pRingEffect->Set_Visible(false);
	}
}

void CRaceRing::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
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

_bool CRaceRing::World_Screen(_float3 fWorld_Pos, _float2& fScreenPos, _float& fDepth)
{
	_matrix mView = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix mProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_matrix mVP = mView * mProj;

	_vector	Pos = XMVectorSet(fWorld_Pos.x, fWorld_Pos.y, fWorld_Pos.z, 1.f);
	Pos = XMVector4Transform(Pos, mVP);

	_float w = XMVectorGetW(Pos);

	_bool bBehind = (w <= 0.f);

	_float NDCX = XMVectorGetX(Pos) / w;
	_float NDCY = XMVectorGetY(Pos) / w;
	_float NDCZ = XMVectorGetZ(Pos) / w;

	if (bBehind)
	{
		fScreenPos.x = g_iWinSizeX * 0.5f;   // 중앙
		fScreenPos.y = g_iWinSizeY + 100.f;  // 화면 아래
		fDepth = -1.f;
		return true;
	}

	fDepth = NDCZ;

	fScreenPos.x = (NDCX + 1.f) * 0.5f * g_iWinSizeX;
	fScreenPos.y = (1.f - NDCY) * 0.5f * g_iWinSizeY;

	return true;
}

_bool CRaceRing::Screen(_float2& fWorld_Pos, _float Winx, _float Winy)
{
	return fWorld_Pos.x >= 0 && fWorld_Pos.x <= Winx &&
		fWorld_Pos.y >= 0 && fWorld_Pos.y <= Winy;
}

_float2 CRaceRing::Get_EdgePosition(_float2& fWorld_Pos, _float Winx, _float Winy)
{
	_float2 center = _float2(Winx * 0.5f, Winy * 0.5f);
	_float2 dir =  _float2(fWorld_Pos.x - center.x,	fWorld_Pos.y - center.y);

	_float tX = FLT_MAX;
	_float tY = FLT_MAX;

	if (dir.x != 0.f)
	{
		tX = (dir.x > 0) ? (Winx - center.x) / dir.x	: (0.f - center.x) / dir.x;
	}

	if (dir.y != 0.f)
	{
		tY = (dir.y > 0) ? (Winy - center.y) / dir.y : (0.f - center.y) / dir.y;
	}
	
	float t = min(tX, tY);

	return _float2(center.x + dir.x * t, center.y + dir.y * t);
}

void CRaceRing::World_to_ScreenUI()
{
	_float2			fScreenPos{};
	_float			fDepth{};
	_float3			WorldPos{};

	XMStoreFloat3(&WorldPos, m_pTransformCom->Get_State(STATE::POSITION));

	if (!World_Screen(WorldPos, fScreenPos, fDepth))
	{
		return;
	}

	_float			fWinSizeX = static_cast<_float>(g_iWinSizeX);
	_float			fWinSizeY = static_cast<_float>(g_iWinSizeY);

	if (!Screen(fScreenPos, fWinSizeX, fWinSizeY))
	{
		fScreenPos = Get_EdgePosition(fScreenPos, fWinSizeX, fWinSizeY);
		m_bShowOffScreenUI = true;
	}
	else
	{
		m_bShowOffScreenUI = false;
	}

	m_fScreenPos = fScreenPos;
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

HRESULT CRaceRing::Ready_Parts()
{
	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;


	if (FAILED(Add_PartObject<CEffectParts>("Ring_Blur", g_iStaticLevel, &m_pRingEffect, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pRingEffect->Load("../Bin/Resources/Data/Effect/BroomRace/Ring_Blur", static_cast<LEVEL>(g_iStaticLevel));
	m_pRingEffect->Set_Visible(false);

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
	SAFE_RELEASE(m_pRingEffect);

}
#ifdef _DEBUG

void CRaceRing::Describe_Entity()
{

}

#endif // _DEBUG
