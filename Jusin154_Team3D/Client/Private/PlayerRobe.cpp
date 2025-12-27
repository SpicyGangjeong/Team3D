#include "pch.h"
#include "PlayerRobe.h"
#include "GameInstance.h"

CPlayerRobe::CPlayerRobe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPlayerRobe::CPlayerRobe(const CPlayerRobe& Prototype)
	: CPartObject(Prototype)
{
}
void CPlayerRobe::Priority_Update(_float fTimeDelta)
{
}

void CPlayerRobe::Update(_float fTimeDelta)
{
	_matrix OwnerMatrix = XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());
	m_pTransformCom->Set_WorldMatrix(OwnerMatrix);
	m_pRobeMainAnchor->Set_Transform(XMLoadFloat4x4(m_pSocketMatrix) * OwnerMatrix);
	Update_LegsPosition();
}

void CPlayerRobe::Late_Update(_float fTimeDelta)
{
	Update_RobeSocketPosition();
#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
#endif // _DEBUG
}

HRESULT CPlayerRobe::Render()
{
	Render_BonePhysX();
	return S_OK;
}

HRESULT CPlayerRobe::Update_RobeSocketPosition()
{

	return S_OK;
}

HRESULT CPlayerRobe::Helper_RouteJointGenerater(CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC& Desc_Body, ROUTE_DESC& Desc_Route, _matrix* xmAnchorMatricesWorld)
{
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
		return E_FAIL;
	}
	m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(xmAnchorMatricesWorld[Desc_Route.iStartBoneIndex].r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3]);
	{ // Joint
		assert(!m_pDynamicJoints[Desc_Route.iStartJointIndex]);
		m_pDynamicJoints[Desc_Route.iStartJointIndex]	= m_pGameInstance->Create_PxD6Joint(m_pRobeJointAnchor[Desc_Route.iStartBoneIndex]->Get_Actor(), m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iStartBoneIndex]->Get_GlobalPosition());
		assert(!m_pDynamicJoints[Desc_Route.iEndJointIndex]);
		m_pDynamicJoints[Desc_Route.iEndJointIndex]		= m_pGameInstance->Create_PxD6Joint(m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_GlobalPosition());
	}
	return S_OK;
}


HRESULT CPlayerRobe::Update_LegsPosition()
{
	_matrix WorldMatrix = m_pParentTransformCom->Get_XMWorldMatrix();
	{
		
		_vector vFootPos	= XMVector4Transform(XMLoadFloat4((_float4*)&m_pLeftFootLocalMatrix->m[3][0]), WorldMatrix);
		_vector vLegPos		= XMVector4Transform(XMLoadFloat4((_float4*)&m_pLeftLegLocalMatrix->m[3][0]), WorldMatrix);
		m_pLeftLeg->Set_CenterTransform(vFootPos, vLegPos);

#ifdef _DEBUG
		{
			_vector vCenterWorldPos = (vFootPos + vLegPos ) * 0.5f;
			_vector vRotQ = XMQuaternionIdentity();
			_vector vDir = XMVectorSetW(vLegPos - vFootPos, 0.f);
			_float vLength = XMVectorGetX(XMVector3Length(vDir));
			if (vLength > FLT_EPSILON5) {
				vDir = XMVector3Normalize(vDir);
				vRotQ = CMyTools::MakeQuaternionFromTo(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
			}
			XMStoreFloat4x4(&m_LeftLegMatrix, XMMatrixRotationQuaternion(vRotQ) * XMMatrixTranslationFromVector(vCenterWorldPos));
		}
#endif // _DEBUG

		vFootPos	= XMVector4Transform(XMLoadFloat4((_float4*)&m_pRightFootLocalMatrix->m[3][0]), WorldMatrix);
		vLegPos		= XMVector4Transform(XMLoadFloat4((_float4*)&m_pRightLegLocalMatrix->m[3][0]), WorldMatrix);
		m_pRightLeg->Set_CenterTransform(vFootPos, vLegPos);

#ifdef _DEBUG
		{
			_vector vCenterWorldPos = (vFootPos + vLegPos) * 0.5f;
			_vector vRotQ = XMQuaternionIdentity(); 
			_vector vDir = XMVectorSetW(vLegPos - vFootPos, 0.f);
			_float vLength = XMVectorGetX(XMVector3Length(vDir));
			if (vLength > FLT_EPSILON5) {
				vDir = XMVector3Normalize(vDir);
				vRotQ = CMyTools::MakeQuaternionFromTo(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
			}
			XMStoreFloat4x4(&m_RightLegMatrix, XMMatrixRotationQuaternion(vRotQ) * XMMatrixTranslationFromVector(vCenterWorldPos));
		}
#endif // _DEBUG
	}

	//_matrix PreTransformMatrix = XMLoadFloat4x4(&m_PreTransformMatrix);
	//_matrix SocketMatrix = {};
	//for (_uint i = ENUM_CLASS(PLAYER_JOINT_ORDER::RUP_TO_RMIDDLE_START); i < ENUM_CLASS(PLAYER_JOINT_ORDER::END); ++i) {
	//	SocketMatrix = XMLoadFloat4x4(m_RobeJointAnchorMatrices[(i - ENUM_CLASS(PLAYER_JOINT_ORDER::RUP_TO_RMIDDLE_START)) /2]);
	//	m_pRobeJointAnchor[i]->Move_Kinematic((SocketMatrix * PreTransformMatrix * WorldMatrix));
	//}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPlayerRobe::Render_Legs()
{
	_float fHeight = 0.25f;

	_matrix WorldMatrix = XMLoadFloat4x4(&m_LeftLegMatrix);
	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0xff0000, 1.f);

	m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	_matrix WorldUp = XMMatrixTranslationFromVector(WorldMatrix.r[1] * fHeight);
	_matrix WorldDown = XMMatrixTranslationFromVector(WorldMatrix.r[1] * -fHeight);
	m_pSubShape->Draw(WorldUp * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	m_pSubShape->Draw(WorldDown * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});

	WorldMatrix = XMLoadFloat4x4(&m_RightLegMatrix);

	m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	WorldUp = XMMatrixTranslationFromVector(WorldMatrix.r[1] * fHeight);
	WorldDown = XMMatrixTranslationFromVector(WorldMatrix.r[1] * -fHeight);
	m_pSubShape->Draw(WorldUp * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	m_pSubShape->Draw(WorldDown * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});

	return S_OK;
}

HRESULT CPlayerRobe::Render_BonePhysX()
{
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x00ff00, 1.f);
	_fmatrix OwnerMatrix = m_pParentTransformCom->Get_XMWorldMatrix();
	_matrix PreTransformMatrix = XMLoadFloat4x4(&m_PreTransformMatrix);
	_cmatrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	ID3D11DepthStencilState* pPrevDSS = nullptr;
	UINT iPrevStencilRef = 0;
	m_pContext->OMGetDepthStencilState(&pPrevDSS, &iPrevStencilRef);

	//for (_uint iBoneIndex = 0; iBoneIndex < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++iBoneIndex)
	//{
	//	_matrix WorldMatrix = XMLoadFloat4x4(m_RobeJointAnchorMatrices[iBoneIndex]) * PreTransformMatrix * OwnerMatrix;

	//	m_pSubShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
	//		[this]() {
	//			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
	//		});
	//}
	_matrix MainAnchorMatrix = XMLoadFloat4x4(m_pSocketMatrix) * OwnerMatrix;
	m_pSubShape->Draw(MainAnchorMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	m_pRobeMainAnchor->Render([this]() {
		m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	Render_Legs();
	_vector vRouteColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x0000ff, 1.f);
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::END); ++i) {
		if (nullptr != m_pRobeJointRoute[i]) {
			m_pRobeJointRoute[i]->Render([this]() {
				m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
				});
		}
	}
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i) {
		m_pRobeJointAnchor[i]->Render([this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
			});
	}
	m_pContext->OMSetDepthStencilState(pPrevDSS, iPrevStencilRef);
	if (pPrevDSS != nullptr) {
		pPrevDSS->Release();
	}
	return S_OK;
}
#endif // _DEBUG

HRESULT CPlayerRobe::Ready_Components()
{
	__super::Ready_Components(nullptr);

	_matrix PreTransformMatrix = XMLoadFloat4x4(&m_PreTransformMatrix);
	_matrix parentWorld = m_pParentTransformCom->Get_XMWorldMatrix();
	_matrix socketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	_matrix socketWorld = socketMatrix * parentWorld;
	_matrix xmAnchorMatricesWorld[ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END)] = { };
	for (_uint iBoneIndex = 0; iBoneIndex < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++iBoneIndex) {
		xmAnchorMatricesWorld[iBoneIndex] = XMLoadFloat4x4(m_RobeJointAnchorMatrices[iBoneIndex]) * PreTransformMatrix * parentWorld;
	}

	{ // 
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_Leg"), (CComponent**)&m_pLeftLeg, &Desc))) {
			return E_FAIL;
		}
	}
	{ // 
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_Leg"), (CComponent**)&m_pRightLeg, &Desc))) {
			return E_FAIL;
		}
	}
	{ // RobeSocket
		{ // Fixed_MainAnchor
			CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
			Desc.iSubKind = ENUM_CLASS(PXOBJECT::JOINT_ANCHOR);
			Desc.bAutoOwnerTranslation = false;
			if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ANCHOR"), (CComponent**)&m_pRobeMainAnchor, &Desc))) {
				return E_FAIL;
			}
			m_pRobeMainAnchor->Set_Kinematic(true);
			m_pRobeMainAnchor->Set_Transform(socketWorld);
#ifdef _DEBUG
			m_pRobeMainAnchor->Set_Name("MainAnchor");
#endif // _DEBUG
		}
		// SubAnchors
		for (_int i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i) {
			CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
			Desc.iSubKind = ENUM_CLASS(PXOBJECT::JOINT_ANCHOR);
			Desc.bAutoOwnerTranslation = false;
			if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ANCHOR"), (CComponent**)&m_pRobeJointAnchor[i], &Desc))) {
				return E_FAIL;
			}

			m_pRobeJointAnchor[i]->Set_Transform(xmAnchorMatricesWorld[i]);
#ifdef _DEBUG
			m_pRobeJointAnchor[i]->Set_Name(PLAYER_JOINT_BONE_NAMES[i]);
#endif // _DEBUG
		}
	}
	{ // RobeRoute
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc_Body = {};
		ROUTE_DESC Desc_Route = {};
		Desc_Body.bAutoOwnerTranslation = false;
		Desc_Body.iSubKind = ENUM_CLASS(PXOBJECT::JOINT_ROUTE);
		/*Origin_Linked_Routes*/
		{
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::ORIGIN_TO_RUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3]);
#ifdef _DEBUG
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_Name("Origin_Linked_Routes");
#endif // _DEBUG
				{ // Joint
					m_pDynamicJoints[Desc_Route.iStartJointIndex]	= m_pGameInstance->Create_PxD6Joint(m_pRobeMainAnchor->Get_Actor(), m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeMainAnchor->Get_GlobalPosition());
					m_pDynamicJoints[Desc_Route.iEndJointIndex]	= m_pGameInstance->Create_PxD6Joint(m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_GlobalPosition());
				}
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::ORIGIN_TO_TUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_TUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3]);
#ifdef _DEBUG
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_Name("Origin_Linked_Routes");
#endif // _DEBUG
				{ // Joint
					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_PxD6Joint(m_pRobeMainAnchor->Get_Actor(), m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeMainAnchor->Get_GlobalPosition());
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_PxD6Joint(m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_GlobalPosition());
				}
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::ORIGIN_TO_LUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_LUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3]);
#ifdef _DEBUG
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_Name("Origin_Linked_Routes");
#endif // _DEBUG
				{ // Joint
					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_PxD6Joint(m_pRobeMainAnchor->Get_Actor(), m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeMainAnchor->Get_GlobalPosition());
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_PxD6Joint(m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor(), m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_GlobalPosition());
				}
			}
		}
		/*UP_TO_MIDDLE*/
		{
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::RUP_TO_RMIDDLE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHT);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::RUP_TO_RMIDDLE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::TUP_TO_TMIDDLE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::TUP_TO_TMIDDLE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::LUP_TO_LMIDDLE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LUP_TO_LMIDDLE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
		}
		/*MIDDLE_TO_DOWN*/
		{
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::RMIDDLE_TO_RR);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHT);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTDOWNRIGHT);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::RMIDDLE_TO_RR_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::RMIDDLE_TO_R);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHT);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::RMIDDLE_TO_R_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::TMIDDLE_TO_TR);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILRIGHTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::TMIDDLE_TO_TR_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::TMIDDLE_TO_TL);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILLEFTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::TMIDDLE_TO_TL_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::LMIDDLE_TO_L);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LMIDDLE_TO_L_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::LMIDDLE_TO_LL);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTDOWNLEFT);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LMIDDLE_TO_LL_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
		}
	}
	return S_OK;
}

HRESULT CPlayerRobe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerRobe::Initialize(void* pArg)
{
	PlayerRobe_DESC* pDesc = static_cast<PlayerRobe_DESC*> (pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_pModelCom = pDesc->pModel;

	m_pLeftFootLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("LeftFoot");
	m_pLeftLegLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("LeftLeg");
	m_pRightFootLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("RightFoot");
	m_pRightLegLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("RightLeg");

	m_PreTransformMatrix = *m_pModelCom->Get_PreTransformMatrixPtr();
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i) {
		m_RobeJointAnchorMatrices[i] = m_pModelCom->Get_BoneLocalMatrixPtr(PLAYER_JOINT_BONE_NAMES[i]);
	}

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}
#ifdef _DEBUG
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 0.05f, 22, false, false));
	m_pMainShape = (GeometricPrimitive::CreateCylinder(m_pContext, 0.3f, 0.075f, 22, false));
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;

	HRESULT result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilStateNone);
	if (FAILED(result)){
		return E_FAIL;
	}
#endif // _DEBUG
	return S_OK;
}

CPlayerRobe* CPlayerRobe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerRobe* pInstance = new CPlayerRobe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerRobe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerRobe::Clone(void* pArg, CGameObject* pOwner)
{
	CPlayerRobe* pInstance = new CPlayerRobe(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerRobe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPlayerRobe::Free()
{
	__super::Free();
#ifdef _DEBUG
	SAFE_RELEASE(m_pDepthStencilStateNone);
#endif // _DEBUG
	SAFE_RELEASE(m_pRightLeg);
	SAFE_RELEASE(m_pLeftLeg);
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::END); ++i) {
		SAFE_RELEASE(m_pRobeJointRoute[i]);
	}
}
#ifdef _DEBUG

void CPlayerRobe::Describe_Entity()
{
}

#endif // _DEBUG
