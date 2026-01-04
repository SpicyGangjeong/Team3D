#include "pch.h"
#include "PlayerRobe.h"
#include "GameInstance.h"

void CPlayerRobe::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
#ifdef 기무리
	Describe_Entity();
	Update_RobeJoints();
#endif // 기무리
#endif // _DEBUG

}

void CPlayerRobe::Update(_float fTimeDelta)
{
	_matrix SocketMatrixFixed = (XMMatrixRotationZ(XM_PI) * XMLoadFloat4x4(m_pSocketMatrix)) 
		* XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr());

	m_pRobeMainAnchor->Move_Kinematic(SocketMatrixFixed, false);
	//m_pRobeMainAnchor->Get_Actor()->wakeUp();

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
	if (m_pGameInstance->Key_Up(DIK_8)) {
		m_bRenderDebug = !m_bRenderDebug;
	}
	if (m_bRenderDebug) {
		Render_BonePhysX();
	}
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
	{
		
		_vector vLegPos	= XMVector4Transform(XMLoadFloat4((_float4*)&m_pLeftLegLocalMatrix->m[3][0]), WorldMatrix);
		_vector vLegUp	= XMVector4Transform(XMLoadFloat4((_float4*)&m_pLeftLegUpLocalMatrix->m[3][0]), WorldMatrix);
		m_pLeftThigh->Set_CenterTransform(vLegPos, vLegUp, false);

#ifdef _DEBUG
		{
			_vector vCenterWorldPos = (vLegPos + vLegUp ) * 0.5f;
			_vector vRotQ = XMQuaternionIdentity();
			_vector vDir = XMVectorSetW(vLegUp - vLegPos, 0.f);
			_float vLength = XMVectorGetX(XMVector3Length(vDir));
			if (vLength > FLT_EPSILON5) {
				vDir = XMVector3Normalize(vDir);
				vRotQ = CMyTools::MakeQuaternionFromTo(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
			}
			XMStoreFloat4x4(&m_LeftThighMatrix, XMMatrixRotationQuaternion(vRotQ) * XMMatrixTranslationFromVector(vCenterWorldPos));
		}
#endif // _DEBUG

		vLegPos	= XMVector4Transform(XMLoadFloat4((_float4*)&m_pRightLegLocalMatrix->m[3][0]), WorldMatrix);
		vLegUp		= XMVector4Transform(XMLoadFloat4((_float4*)&m_pRightLegUpLocalMatrix->m[3][0]), WorldMatrix);
		m_pRightThigh->Set_CenterTransform(vLegPos, vLegUp, false);

#ifdef _DEBUG
		{
			_vector vCenterWorldPos = (vLegPos + vLegUp) * 0.5f;
			_vector vRotQ = XMQuaternionIdentity(); 
			_vector vDir = XMVectorSetW(vLegUp - vLegPos, 0.f);
			_float vLength = XMVectorGetX(XMVector3Length(vDir));
			if (vLength > FLT_EPSILON5) {
				vDir = XMVector3Normalize(vDir);
				vRotQ = CMyTools::MakeQuaternionFromTo(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
			}
			XMStoreFloat4x4(&m_RightThighMatrix, XMMatrixRotationQuaternion(vRotQ) * XMMatrixTranslationFromVector(vCenterWorldPos));
		}
#endif // _DEBUG
	}
	m_pLower0Bound->Move_Kinematic(XMMatrixTranslationFromVector(XMVectorSet(0.f, -0.2f, 0.f, 0.f)) * XMLoadFloat4x4(m_pLower0BoundMatrix) * WorldMatrix, false);
	m_pLower1Bound->Move_Kinematic(XMMatrixTranslationFromVector(XMVectorSet(0.f, -0.7f, 0.f, 0.f)) * XMLoadFloat4x4(m_pLower1BoundMatrix) * WorldMatrix, false);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPlayerRobe::Render_Legs(_fmatrix LowerMatrix)
{
	_float fHeight = 0.25f;

	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0xff0000, 1.f);

	m_pMainShape->Draw(LowerMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	_matrix WorldUp = XMMatrixTranslationFromVector(LowerMatrix.r[1] * fHeight);
	_matrix WorldDown = XMMatrixTranslationFromVector(LowerMatrix.r[1] * -fHeight);
	m_pSubShape->Draw(WorldUp * LowerMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
			m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
		});
	m_pSubShape->Draw(WorldDown * LowerMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true,
		[this]() {
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
	Render_Legs(XMLoadFloat4x4(&m_LeftLegMatrix));
	Render_Legs(XMLoadFloat4x4(&m_RightLegMatrix));
	Render_Legs(XMLoadFloat4x4(&m_LeftThighMatrix));
	Render_Legs(XMLoadFloat4x4(&m_RightThighMatrix));
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
	m_pLower0Bound->Render();
	m_pLower1Bound->Render();
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
			{
			case PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_START:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_TUP_START:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_LUP_START:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_RSIDEUP_START:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_LSIDEUP_START:
				m_JointDescriptions[i].iMotionTwist = 1;
				m_JointDescriptions[i].iMotionSwing1 = 1;
				m_JointDescriptions[i].iMotionSwing2 = 1;
				m_JointDescriptions[i].fLinearLimit = 0.f;
				m_JointDescriptions[i].fTwistAbsDeg = 0.f;
				m_JointDescriptions[i].fSwingDeg = 40.f;
				break;
			case PLAYER_JOINT_ORDER::ORIGIN_TO_RUP_END:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_TUP_END:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_LUP_END:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_RSIDEUP_END:
			case PLAYER_JOINT_ORDER::ORIGIN_TO_LSIDEUP_END:
				m_JointDescriptions[i].iMotionTwist = 1;
				m_JointDescriptions[i].iMotionSwing1 = 1;
				m_JointDescriptions[i].iMotionSwing2 = 1;
				m_JointDescriptions[i].fLinearLimit = 0.f;
				m_JointDescriptions[i].fTwistAbsDeg = 0.f;
				m_JointDescriptions[i].fSwingDeg = 40.f;
				break;
			}
			/* From_Middle */
			{
			case PLAYER_JOINT_ORDER::RUP_TO_RMIDDLE_START:
			case PLAYER_JOINT_ORDER::TUP_TO_TMIDDLE_START:
			case PLAYER_JOINT_ORDER::LUP_TO_LMIDDLE_START:
			case PLAYER_JOINT_ORDER::RSIDEUP_TO_RSIDE_START:
			case PLAYER_JOINT_ORDER::LSIDEUP_TO_LSIDE_START:
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
			case PLAYER_JOINT_ORDER::RSIDEUP_TO_RSIDE_END:
			case PLAYER_JOINT_ORDER::LSIDEUP_TO_LSIDE_END:
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
			}
			/* From_Lower */
			{
			case PLAYER_JOINT_ORDER::RMIDDLE_TO_RR_START:
			case PLAYER_JOINT_ORDER::RSIDE_TO_R_START:
				m_JointDescriptions[i].iMotionTwist = 1;
				m_JointDescriptions[i].iMotionSwing1 = 1;
				m_JointDescriptions[i].iMotionSwing2 = 1;
				m_JointDescriptions[i].fLinearLimit = 0.f;
				m_JointDescriptions[i].fTwistAbsDeg = 15.f;
				m_JointDescriptions[i].fSwingDeg = 15.f;
				break;

			case PLAYER_JOINT_ORDER::RSIDE_TO_R_END:
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

			case PLAYER_JOINT_ORDER::LSIDE_TO_L_START:
			case PLAYER_JOINT_ORDER::LMIDDLE_TO_LL_START:
				m_JointDescriptions[i].iMotionTwist = 1;
				m_JointDescriptions[i].iMotionSwing1 = 1;
				m_JointDescriptions[i].iMotionSwing2 = 1;
				m_JointDescriptions[i].fLinearLimit = 0.f;
				m_JointDescriptions[i].fTwistAbsDeg = 15.f;
				m_JointDescriptions[i].fSwingDeg = 15.f;
				break;

			case PLAYER_JOINT_ORDER::LSIDE_TO_L_END:
			case PLAYER_JOINT_ORDER::LMIDDLE_TO_LL_END:
				break;
			}
		default:
			break;
		}

		//m_JointDescriptions[i].iMotionTwist = 0;
		//m_JointDescriptions[i].iMotionSwing1 = 0;
		//m_JointDescriptions[i].iMotionSwing2 = 0;
		m_JointDescriptions[i].ApplyToJoint(*m_pDynamicJoints[i]);
	}
	return S_OK;
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
