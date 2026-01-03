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
#ifdef _DEBUG
#ifdef 기무리
	//Update_RobeJoints();
#endif // 기무리

	Describe_Entity();
#endif // _DEBUG

}

void CPlayerRobe::Update(_float fTimeDelta)
{
	_matrix SocketMatrixFixed = (XMMatrixRotationZ(XM_PI) * XMLoadFloat4x4(m_pSocketMatrix)) *
		XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());

	m_pRobeMainAnchor->Move_Kinematic(SocketMatrixFixed, false);
	m_pRobeMainAnchor->Get_Actor()->wakeUp();

	Update_LegsPosition();
	Update_RobeSocketPosition();
}

void CPlayerRobe::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
}

HRESULT CPlayerRobe::Render()
{
#ifdef _DEBUG
#ifdef 기무리
	//Render_BonePhysX();
#endif // 기무리
#endif // _DEBUG
	return S_OK;
}

HRESULT CPlayerRobe::Update_RobeSocketPosition()
{
	{
		PSX::PxTransform pxTransform = {};
		_matrix WorldMatrix = {};
		_matrix OwnerInvWorldMatrix = m_pParentTransformCom->Get_WorldMatrixInv();
		//{
		//	pxTransform = m_pRobeMainAnchor->Get_GlobalPosition();
		//	WorldMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorZero(), XMLoadFloat4((_float4*)&pxTransform.q), XMVectorSetW(XMLoadFloat3((_float3*)&pxTransform.p), 1.f));
		//	_fmatrix LocalMatrix = WorldMatrix * OwnerInvWorldMatrix;
		//	m_pModelCom->Set_BoneCombinedTransformation("Hips_Cloth", LocalMatrix);
		//}
		for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i) {
			pxTransform = m_pRobeJointAnchor[i]->Get_GlobalPosition();
			WorldMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorZero(), XMLoadFloat4((_float4*)&pxTransform.q), XMVectorSetW(XMLoadFloat3((_float3*)&pxTransform.p), 1.f));
			_fmatrix LocalMatrix = WorldMatrix * OwnerInvWorldMatrix;
			XMStoreFloat4x4(&m_ReconstructedJointAnchorMatirces[i], LocalMatrix);
			m_pModelCom->Set_BoneCombinedTransformation(PLAYER_JOINT_BONE_NAMES[i], LocalMatrix);
		}
	}
	return S_OK;
}

HRESULT CPlayerRobe::Helper_RouteJointGenerater(CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC& Desc_Body, ROUTE_DESC& Desc_Route, _matrix* xmAnchorMatricesWorld)
{

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
		return E_FAIL;
	}
	m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(xmAnchorMatricesWorld[Desc_Route.iStartBoneIndex].r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);


	PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iStartBoneIndex]->Get_Actor();
	PSX::PxRigidDynamic* routeRigidActor = m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
	PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

	PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
	PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

	m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);

	m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);

	return S_OK;
}

HRESULT CPlayerRobe::Update_LegsPosition()
{
	_matrix WorldMatrix = m_pParentTransformCom->Get_XMWorldMatrix();
	{
		
		_vector vFootPos	= XMVector4Transform(XMLoadFloat4((_float4*)&m_pLeftFootLocalMatrix->m[3][0]), WorldMatrix);
		_vector vLegPos		= XMVector4Transform(XMLoadFloat4((_float4*)&m_pLeftLegLocalMatrix->m[3][0]), WorldMatrix);
		m_pLeftLeg->Set_CenterTransform(vFootPos, vLegPos, false);

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
		m_pRightLeg->Set_CenterTransform(vFootPos, vLegPos, false);

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
	m_pLower0Bound->Move_Kinematic(m_pParentTransformCom->Get_XMWorldMatrix(), false);
	m_pLower1Bound->Move_Kinematic(m_pParentTransformCom->Get_XMWorldMatrix(), false);

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
	m_pLower0Bound->Render([this]() {
		m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	m_pLower1Bound->Render([this]() {
		m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});

	return S_OK;
}
HRESULT CPlayerRobe::Render_BonePhysX()
{
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x00ff00, 1.f);
	_fmatrix OwnerMatrix = m_pParentTransformCom->Get_XMWorldMatrix();
	_cmatrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	ID3D11DepthStencilState* pPrevDSS = nullptr;
	UINT iPrevStencilRef = 0;
	m_pContext->OMGetDepthStencilState(&pPrevDSS, &iPrevStencilRef);

	for (_uint iBoneIndex = 0; iBoneIndex < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++iBoneIndex)
	{
		_matrix WorldMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr(PLAYER_JOINT_BONE_NAMES[iBoneIndex])) * OwnerMatrix;

		m_pSubShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
			[this]() {
				m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
			});
	}
	_matrix MainAnchorMatrix = /*XMMatrixRotationZ(XM_PIDIV2) * */XMLoadFloat4x4(m_pSocketMatrix) * OwnerMatrix;
	m_pSubShape->Draw(MainAnchorMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	m_pRobeMainAnchor->Render([this]() {
		m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	Render_Legs();
	_vector vRouteColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x0000ff, 1.f);
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

HRESULT CPlayerRobe::Update_RobeJoints()
{
	for (_uint i = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_START); i < ENUM_CLASS(PLAYER_JOINT_ORDER::END); ++i) {
		m_JointDescriptions[i].InitFromJoint(*m_pDynamicJoints[i]);
		switch (PLAYER_JOINT_ORDER(i))
		{
			/* From Origin */
		case PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_START:
		case PLAYER_JOINT_ORDER::ORIGIN_TO_TUP_START:
		case PLAYER_JOINT_ORDER::ORIGIN_TO_LUP_START:
			m_JointDescriptions[i].iMotionTwist = 0;
			m_JointDescriptions[i].iMotionSwing1 = 0;
			m_JointDescriptions[i].iMotionSwing2 = 0;
			m_JointDescriptions[i].fLinearLimit = 0.f;
			m_JointDescriptions[i].fTwistAbsDeg = 0.f;
			m_JointDescriptions[i].fSwingDeg = 5.f;
			break;
		case PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_END:
		case PLAYER_JOINT_ORDER::ORIGIN_TO_TUP_END:
		case PLAYER_JOINT_ORDER::ORIGIN_TO_LUP_END:
			m_JointDescriptions[i].iMotionTwist = 0;
			m_JointDescriptions[i].iMotionSwing1 = 0;
			m_JointDescriptions[i].iMotionSwing2 = 0;
			m_JointDescriptions[i].fLinearLimit = 0.f;
			m_JointDescriptions[i].fTwistAbsDeg = 0.f;
			m_JointDescriptions[i].fSwingDeg = 5.f;
			break;


			/* From_Middle */
		case PLAYER_JOINT_ORDER::RUP_TO_RMIDDLE_START:
		case PLAYER_JOINT_ORDER::TUP_TO_TMIDDLE_START:
		case PLAYER_JOINT_ORDER::LUP_TO_LMIDDLE_START:
			m_JointDescriptions[i].iMotionTwist = 0;
			m_JointDescriptions[i].iMotionSwing1 = 0;
			m_JointDescriptions[i].iMotionSwing2 = 0;
			m_JointDescriptions[i].fLinearLimit = 0.f;
			m_JointDescriptions[i].fTwistAbsDeg = 0.f;
			m_JointDescriptions[i].fSwingDeg = 10.f;
			break;

		case PLAYER_JOINT_ORDER::RUP_TO_RMIDDLE_END:
		case PLAYER_JOINT_ORDER::TUP_TO_TMIDDLE_END:
		case PLAYER_JOINT_ORDER::LUP_TO_LMIDDLE_END:
			m_JointDescriptions[i].iMotionTwist = 1;
			m_JointDescriptions[i].iMotionSwing1 = 1;
			m_JointDescriptions[i].iMotionSwing2 = 1;
			m_JointDescriptions[i].fLinearLimit = 0.f;
			m_JointDescriptions[i].fSwingDeg = 0.f;
			m_JointDescriptions[i].fTwistAbsDeg = 0.f;

			m_JointDescriptions[i].bEnableSlerpDrive = true;
			m_JointDescriptions[i].fLinearStiffness = 10.f;
			m_JointDescriptions[i].fSlerpDamping = 3.f;
			m_JointDescriptions[i].fSlerpForceLimit = 40.f;
			m_JointDescriptions[i].bSlerpAccel = true;
			break;


			/* From_Lower */
		case PLAYER_JOINT_ORDER::RMIDDLE_TO_RR_START:
		case PLAYER_JOINT_ORDER::RMIDDLE_TO_R_START:
			m_JointDescriptions[i].iMotionTwist = 1;
			m_JointDescriptions[i].iMotionSwing1 = 1;
			m_JointDescriptions[i].iMotionSwing2 = 1;
			m_JointDescriptions[i].fLinearLimit = 0.f;
			m_JointDescriptions[i].fTwistAbsDeg = 15.f;
			m_JointDescriptions[i].fSwingDeg = 15.f;
			break;

		case PLAYER_JOINT_ORDER::RMIDDLE_TO_R_END:
		case PLAYER_JOINT_ORDER::RMIDDLE_TO_RR_END:
			break;
		case PLAYER_JOINT_ORDER::TMIDDLE_TO_TR_START:
		case PLAYER_JOINT_ORDER::TMIDDLE_TO_TL_START:
			m_JointDescriptions[i].iMotionTwist = 1;
			m_JointDescriptions[i].iMotionSwing1 = 1;
			m_JointDescriptions[i].iMotionSwing2 = 1;
			m_JointDescriptions[i].fLinearLimit = 0.f;
			m_JointDescriptions[i].fTwistAbsDeg = 15.f;
			m_JointDescriptions[i].fSwingDeg = 15.f;
			break;

		case PLAYER_JOINT_ORDER::TMIDDLE_TO_TR_END:
		case PLAYER_JOINT_ORDER::TMIDDLE_TO_TL_END:
			break;
		case PLAYER_JOINT_ORDER::LMIDDLE_TO_L_START:
		case PLAYER_JOINT_ORDER::LMIDDLE_TO_LL_START:
			m_JointDescriptions[i].iMotionTwist = 1;
			m_JointDescriptions[i].iMotionSwing1 = 1;
			m_JointDescriptions[i].iMotionSwing2 = 1;
			m_JointDescriptions[i].fLinearLimit = 0.f;
			m_JointDescriptions[i].fTwistAbsDeg = 15.f;
			m_JointDescriptions[i].fSwingDeg = 15.f;
			break;

		case PLAYER_JOINT_ORDER::LMIDDLE_TO_L_END:
		case PLAYER_JOINT_ORDER::LMIDDLE_TO_LL_END:
			break;
		default:
			break;
		}
		m_JointDescriptions[i].ApplyToJoint(*m_pDynamicJoints[i]);
	}
	return S_OK;
}
HRESULT CPlayerRobe::Ready_Components()
{
	__super::Ready_Components(nullptr);

	_matrix socketWorld = /*XMMatrixRotationZ(XM_PIDIV2) * */XMLoadFloat4x4(m_pSocketMatrix) * m_pParentTransformCom->Get_XMWorldMatrix();
	_matrix xmAnchorMatricesWorld[ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END)] = { };
	for (_uint iBoneIndex = 0; iBoneIndex < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++iBoneIndex) {
		xmAnchorMatricesWorld[iBoneIndex] = XMLoadFloat4x4(&m_ReconstructedJointAnchorMatirces[iBoneIndex]) * socketWorld;
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
	{ // 
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_LowerBound"), (CComponent**)&m_pLower0Bound, &Desc))) {
			return E_FAIL;
		}
	}
	{ // 
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_LowerBound"), (CComponent**)&m_pLower1Bound, &Desc))) {
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
			m_pRobeMainAnchor->Set_Transform(socketWorld, true);
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
			
			m_pRobeJointAnchor[i]->Set_Transform(xmAnchorMatricesWorld[i], true);
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
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
#ifdef _DEBUG
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_Name("Origin_Linked_Routes");
#endif // _DEBUG
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
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
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
#ifdef _DEBUG
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_Name("Origin_Linked_Routes");
#endif // _DEBUG
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
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
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
#ifdef _DEBUG
				m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Set_Name("Origin_Linked_Routes");
#endif // _DEBUG
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
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

	m_ReconstructedJointAnchorMatirces.resize(ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END));

	m_iBoneNum = m_pModelCom->Get_MeshInfluencedBoneNum(ENUM_CLASS(PLAYER_MESH_ORDER::ROBE_CLOTH));
	m_PrevRobeBoneMatrices.resize(m_iBoneNum);
	if (FAILED(m_pModelCom->Capture_BoneMatrices(ENUM_CLASS(PLAYER_MESH_ORDER::ROBE_CLOTH), m_PrevRobeBoneMatrices))) {
		return E_FAIL;
	}

	m_pLeftFootLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("LeftFoot");
	m_pLeftLegLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("LeftLeg");
	m_pRightFootLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("RightFoot");
	m_pRightLegLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("RightLeg");
	
	XMStoreFloat4x4(&m_PreTransformMatrix, XMLoadFloat4x4(m_pModelCom->Get_PreTransformMatrixPtr()));

	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i) {
		m_RobeJointAnchorMatrices[i] = m_pModelCom->Get_BoneLocalMatrixPtr(PLAYER_JOINT_BONE_NAMES[i]);
	}
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i)
	{
		_uint iParentIndex = UINT_MAX;
		_bool bCopyPreTransform = false;

		switch ((PLAYER_JOINT_BONE_ORDER)i)
		{
		case PLAYER_JOINT_BONE_ORDER::RIGHTUP:
		case PLAYER_JOINT_BONE_ORDER::TAILUP:
		case PLAYER_JOINT_BONE_ORDER::LEFTUP:
			bCopyPreTransform = true;
			break;

		case PLAYER_JOINT_BONE_ORDER::RIGHT:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTUP);
			break;
		case PLAYER_JOINT_BONE_ORDER::TAIL:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILUP);
			break;
		case PLAYER_JOINT_BONE_ORDER::LEFT:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTUP);
			break;
		case PLAYER_JOINT_BONE_ORDER::RIGHTDOWNRIGHT:
		case PLAYER_JOINT_BONE_ORDER::RIGHTDOWN:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHT);
			break;
		case PLAYER_JOINT_BONE_ORDER::TAILRIGHTDOWN:
		case PLAYER_JOINT_BONE_ORDER::TAILLEFTDOWN:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
			break;
		case PLAYER_JOINT_BONE_ORDER::LEFTDOWN:
		case PLAYER_JOINT_BONE_ORDER::LEFTDOWNLEFT:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
			break;
		default:
			assert(false);
			break;
		}

		if (true == bCopyPreTransform)
		{
			_matrix constructedMatrix = XMLoadFloat4x4(m_RobeJointAnchorMatrices[i]) * XMLoadFloat4x4(&m_PreTransformMatrix);

			XMStoreFloat4x4(&m_ReconstructedJointAnchorMatirces[i], constructedMatrix);
			continue;
		}

		_matrix constructedMatrix = XMLoadFloat4x4(m_RobeJointAnchorMatrices[i]) 
			* XMLoadFloat4x4(&m_ReconstructedJointAnchorMatirces[iParentIndex]);

		XMStoreFloat4x4(&m_ReconstructedJointAnchorMatirces[i], constructedMatrix);
	}

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	D6JOINTDESC JointDesc = D6JOINTDESC();
	for (_uint i = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_START); i < ENUM_CLASS(PLAYER_JOINT_ORDER::END); ++i) {
		JointDesc.ApplyToJoint(*m_pDynamicJoints[i]);
		m_JointDescriptions[i] = JointDesc;
	}
	Update_RobeJoints();

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
	SAFE_RELEASE(m_pLower0Bound);
	SAFE_RELEASE(m_pLower1Bound);
	SAFE_RELEASE(m_pLeftLeg);
	SAFE_RELEASE(m_pRobeMainAnchor);
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_ROUTE_ORDER::END); ++i) {
		SAFE_RELEASE(m_pRobeJointRoute[i]);
	}
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i) {
		SAFE_RELEASE(m_pRobeJointAnchor[i]);
	}
}
#ifdef _DEBUG

void CPlayerRobe::Describe_Entity()
{
	GUI::Begin("PhysX");
	if (GUI::CollapsingHeader("Robe")) {
		for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_ORDER::END); ++i) {
			_uint iActor0 = i;
			if (GUI::TreeNode(("Child ID: " + to_string(iActor0)).c_str())) {
				GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
				PSX::PxTransform pxDrivePos0 = m_pDynamicJoints[iActor0]->getDrivePosition();
				if (true == CMyTools::DescribePxTransform(pxDrivePos0, iActor0)) {
					m_pDynamicJoints[iActor0]->setDrivePosition(pxDrivePos0);
				}
				{
					_bool bModified = false;
					if (CMyTools::Modify_D6Joint(*m_pDynamicJoints[iActor0], iActor0)) {
						bModified = true;
					}
					GUI::TreePop();
				}
			}
		}
		m_pLower0Bound->Describe_Entity();
		m_pLower1Bound->Describe_Entity();
	}
	GUI::End();
}

#endif // _DEBUG
