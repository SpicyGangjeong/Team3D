#include "pch.h"
#include "PlayerRobe.h"
#include "GameInstance.h"

CPlayerRobe::CPlayerRobe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject(pDevice, pContext) { }
CPlayerRobe::CPlayerRobe(const CPlayerRobe& Prototype) : CPartObject(Prototype) { }
HRESULT CPlayerRobe::Helper_RouteJointGenerater(PHYSX_JOINT eType, CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC& Desc_Body, ROUTE_DESC& Desc_Route, _matrix* xmAnchorMatricesWorld)
{
	if (PHYSX_JOINT::D6 == eType) {
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
			return E_FAIL;
		}
		m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(xmAnchorMatricesWorld[Desc_Route.iStartBoneIndex].r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);

		PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iStartBoneIndex]->Get_Actor();
		PSX::PxRigidDynamic* routeRigidActor = m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
		PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

		PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
		PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

		m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
		m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
	}
	//else if (PHYSX_JOINT::FIXED == eType) {
	//	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
	//		return E_FAIL;
	//	}
	//	m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(xmAnchorMatricesWorld[Desc_Route.iStartBoneIndex].r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);

	//	PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iStartBoneIndex]->Get_Actor();
	//	PSX::PxRigidDynamic* routeRigidActor = m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
	//	PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

	//	PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
	//	PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

	//	m_pDynamicJoints[Desc_Route.iStartJointIndex]	= m_pGameInstance->Create_BasicPxFixedJoint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
	//	m_pDynamicJoints[Desc_Route.iEndJointIndex]		= m_pGameInstance->Create_BasicPxFixedJoint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
	//}

	return S_OK;
}
HRESULT CPlayerRobe::Ready_Components()
{
	__super::Ready_Components(nullptr);

	_matrix socketWorld = XMLoadFloat4x4(m_pSocketMatrix) * m_pParentTransformCom->Get_XMWorldMatrix();
	_matrix xmAnchorMatricesWorld[ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END)] = { };
	for (_uint iBoneIndex = 0; iBoneIndex < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++iBoneIndex) {
		xmAnchorMatricesWorld[iBoneIndex] = XMLoadFloat4x4(&m_ReconstructedJointAnchorMatirces[iBoneIndex]) * socketWorld;
	}
#pragma region BASIC_COMP
	{ // LLEG
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_Leg"), (CComponent**)&m_pLeftLeg, &Desc))) {
			return E_FAIL;
		}
	}
	{ // RLEG
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_Leg"), (CComponent**)&m_pRightLeg, &Desc))) {
			return E_FAIL;
		}
	}

	{ // LTHIGH
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_Leg"), (CComponent**)&m_pLeftThigh, &Desc))) {
			return E_FAIL;
		}
	}
	{ // RTHIGH
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_Leg"), (CComponent**)&m_pRightThigh, &Desc))) {
			return E_FAIL;
		}
	}
	{ // L0BOUND
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_LowerBound"), (CComponent**)&m_pLower0Bound, &Desc))) {
			return E_FAIL;
		}
	}
	{ // L1BOUND
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::LEG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_Player_LowerBound"), (CComponent**)&m_pLower1Bound, &Desc))) {
			return E_FAIL;
		}
	}
#pragma endregion
#pragma region SOCKET
#pragma region SOCKET_DECLARE
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
#pragma endregion
	{ // RobeRoute
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc_Body = {};
		ROUTE_DESC Desc_Route = {};
		Desc_Body.bAutoOwnerTranslation = false;
		Desc_Body.iSubKind = ENUM_CLASS(PXOBJECT::JOINT_ROUTE);
		/*Origin_Linked_Routes*/
		{
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::ORIGIN_TO_RUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
				}
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::ORIGIN_TO_TUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_TUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
				}
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::ORIGIN_TO_LUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_LUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
				}
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::ORIGIN_TO_RSIDEUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTSIDEUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_RSIDEUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
				}
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::ORIGIN_TO_LSIDEUP);
				Desc_Route.iStartBoneIndex = -1;
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTSIDEUP);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::ORIGIN_TO_LSIDEUP_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), (CComponent**)&m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex], &Desc_Body))) {
					return E_FAIL;
				}
				m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Set_CenterTransform(socketWorld.r[3], xmAnchorMatricesWorld[Desc_Route.iEndBoneIndex].r[3], true);
				{ // Joint
					PSX::PxRigidDynamic* startAnchorRigidActor = m_pRobeMainAnchor->Get_Actor();
					PSX::PxRigidDynamic* routeRigidActor = m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_Actor();
					PSX::PxRigidDynamic* endAnchorRigidActor = m_pRobeJointAnchor[Desc_Route.iEndBoneIndex]->Get_Actor();

					PSX::PxTransform startJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), false); // start y--
					PSX::PxTransform endJointWorldPose = CMyTools::Calc_JointPosFromRoute(routeRigidActor, m_pRobeVerticalJointRoute[Desc_Route.iTargetRouteIndex]->Get_HalfGeometryInfo(), true); // end y++

					m_pDynamicJoints[Desc_Route.iStartJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(startAnchorRigidActor, routeRigidActor, startJointWorldPose);
					m_pDynamicJoints[Desc_Route.iEndJointIndex] = m_pGameInstance->Create_BasicPxD6Joint(routeRigidActor, endAnchorRigidActor, endJointWorldPose);
				}
			}
		}
		/*UP_TO_MIDDLE*/
		{
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::RUP_TO_RMIDDLE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHT);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::RUP_TO_RMIDDLE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::TUP_TO_TMIDDLE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::TUP_TO_TMIDDLE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::LUP_TO_LMIDDLE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LUP_TO_LMIDDLE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::RSIDEUP_TO_RSIDE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTSIDEUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTSIDE);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::RSIDEUP_TO_RSIDE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::LSIDEUP_TO_LSIDE);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTSIDEUP);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTSIDE);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LSIDEUP_TO_LSIDE_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
		}
		/*MIDDLE_TO_DOWN*/
		{
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::RMIDDLE_TO_RR);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHT);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::RMIDDLE_TO_RR_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::RSIDE_TO_R);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTSIDE);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTDOWNSIDE);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::RSIDE_TO_R_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::TMIDDLE_TO_TR);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILRIGHTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::TMIDDLE_TO_TR_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::TMIDDLE_TO_TL);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAILLEFTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::TMIDDLE_TO_TL_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::LSIDE_TO_L);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTSIDE);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTDOWNSIDE);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LSIDE_TO_L_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::LMIDDLE_TO_LL);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LMIDDLE_TO_LL_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
			{
				Desc_Route.iTargetRouteIndex = ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::LMIDDLE_TO_LL);
				Desc_Route.iStartBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
				Desc_Route.iEndBoneIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTDOWN);
				Desc_Route.iStartJointIndex = ENUM_CLASS(PLAYER_JOINT_ORDER::LMIDDLE_TO_LL_START);
				Desc_Route.iEndJointIndex = Desc_Route.iStartJointIndex + 1;
				Helper_RouteJointGenerater(PHYSX_JOINT::D6, Desc_Body, Desc_Route, xmAnchorMatricesWorld);
			}
		}
	}
#pragma endregion
	return S_OK;
}
HRESULT CPlayerRobe::Initialize_Prototype() { return S_OK; }
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

	// Leg-Thigh
	m_pLeftFootLocalMatrix		= m_pModelCom->Get_BoneMatrixPtr("LeftFoot");
	m_pLeftLegLocalMatrix		= m_pModelCom->Get_BoneMatrixPtr("LeftLeg");
	m_pRightFootLocalMatrix		= m_pModelCom->Get_BoneMatrixPtr("RightFoot");
	m_pRightLegLocalMatrix		= m_pModelCom->Get_BoneMatrixPtr("RightLeg");
	m_pLeftLegUpLocalMatrix		= m_pModelCom->Get_BoneMatrixPtr("LeftUpLeg");
	m_pRightLegUpLocalMatrix	= m_pModelCom->Get_BoneMatrixPtr("RightUpLeg");

	// Circle
	m_pLower1BoundMatrix		= m_pLower0BoundMatrix	= m_pModelCom->Get_BoneMatrixPtr("Hips");

#pragma region READY_INITIAL_COMBINED_TRANSFORMATIONMATRIX
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
		case PLAYER_JOINT_BONE_ORDER::RIGHTSIDEUP:
		case PLAYER_JOINT_BONE_ORDER::LEFTSIDEUP:
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
		case PLAYER_JOINT_BONE_ORDER::RIGHTSIDE:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTSIDEUP);
			break;
		case PLAYER_JOINT_BONE_ORDER::LEFTSIDE:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTSIDEUP);
			break;

		case PLAYER_JOINT_BONE_ORDER::RIGHTDOWN:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHT);
			break;
		case PLAYER_JOINT_BONE_ORDER::TAILRIGHTDOWN:
		case PLAYER_JOINT_BONE_ORDER::TAILLEFTDOWN:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::TAIL);
			break;
		case PLAYER_JOINT_BONE_ORDER::LEFTDOWN:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFT);
			break;
		case PLAYER_JOINT_BONE_ORDER::RIGHTDOWNSIDE:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::RIGHTSIDE);
			break;
		case PLAYER_JOINT_BONE_ORDER::LEFTDOWNSIDE:
			iParentIndex = ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::LEFTSIDE);
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
#pragma endregion

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

	Update_LegsPosition();
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
	SAFE_RELEASE(m_pLeftLeg);
	SAFE_RELEASE(m_pLeftThigh);
	SAFE_RELEASE(m_pRightThigh);
	SAFE_RELEASE(m_pLower0Bound);
	SAFE_RELEASE(m_pLower1Bound);
	SAFE_RELEASE(m_pRobeMainAnchor);
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::END); ++i) {
		SAFE_RELEASE(m_pRobeVerticalJointRoute[i]);
	}
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_ROUTE_VERTICAL_ORDER::END); ++i) {
		SAFE_RELEASE(m_pRobeFixedJointRoute[i]);
	}
	for (_uint i = 0; i < ENUM_CLASS(PLAYER_JOINT_BONE_ORDER::END); ++i) {
		SAFE_RELEASE(m_pRobeJointAnchor[i]);
	}
	if (nullptr != m_pMainShape) {
		m_pMainShape.release();
		m_pMainShape = nullptr;
	}
	if (nullptr != m_pSubShape) {
		m_pSubShape.release();
		m_pSubShape = nullptr;
	}
}
