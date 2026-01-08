#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)
class CMyTools {
private:
	CMyTools() = delete;
	~CMyTools() = delete;
public:

#pragma region Color
	// [(255, 255, 255, xx), alpha] -> [(0~1, 0~1, 0~1, alpha)]
	// 0xRRGGBBxx, alpha -> _float4( 0~1, 0~1, 0~1, alpha )
	inline static _float4 ColorRGB_A_HEXtoFLOAT4(_uint hexCode, _float alpha) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, false); a = alpha;
		return _float4(r, g, b, a);
	}

	// [(255, 255, 255, 255)] -> [(0~1, 0~1, 0~1, 0~1)]
	// 0xRRGGBBAA -> _float4( 0~1, 0~1, 0~1, 0~1 )
	inline static _float4 ColorRGBA_HEXtoFLOAT4(_uint hexCode) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, true);
		return _float4(r, g, b, a);
	}

	// [(255, 255, 255, xx), alpha] -> [(0~1, 0~1, 0~1, alpha)]
	// 0xRRGGBBxx, alpha -> _vector( 0~1, 0~1, 0~1, alpha )
	inline static _vector ColorRGB_A_HEXtoVECTOR(_uint hexCode, _float alpha) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, false); a = alpha;
		return XMVectorSet(r, g, b, a);
	}

	// [(255, 255, 255, 255)] -> [(0~1, 0~1, 0~1, 0~1)]
	// 0xRRGGBBAA -> _vector( 0~1, 0~1, 0~1, 0~1 )
	inline static _vector ColorRGBA_HEXtoVECTOR(_uint hexCode) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, true);
		return XMVectorSet(r, g, b, a);
	}
#pragma endregion
#pragma region String
	// wstring --> string 변환
	inline static _string ToString(const _wstring& var)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, var.c_str(),
			(int)var.size(), nullptr, 0, nullptr, nullptr);

		string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, var.c_str(),
			(int)var.size(), &strTo[0], size_needed, nullptr, nullptr);

		return strTo;
	}

	// string --> wstring 변환
	inline static _wstring ToWstring(const _string& var)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, var.c_str(),
			(int)var.size(), nullptr, 0);

		wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, var.c_str(),
			(int)var.size(), &wstrTo[0], size_needed);

		return wstrTo;
	}

	// source에 keyword가 있는지 검사
	inline static _bool ContainsString(const char* source, const char* keyword) {
		if (!source || !keyword || *keyword == '\0') {
			return false;
		}
		return (strstr(source, keyword) != nullptr);
	}

#pragma endregion
#pragma region Mathematics
	// ratio로 현재 value 가져오기
	inline static _float Lerp_f1D(_float fA, _float fB, _float fRatio)
	{
		return fA + (fB - fA) * fRatio;
	}

	// value로 현재 ratio 가져오기
	inline static _float InverseLerp_f1D(_float fA, _float fB, _float fValue)
	{
		if (fA == fB) {
			return 0.0f;
		}
		return (fValue - fA) / (fB - fA);
	}
	
	// A -> B, A- >C의 외적이 업벡터 방향으로 외적되도록 B C를 교정
	static void CorrectTriangleToUpHead(_float3& dotA, _float3& dotB, _float3& dotC) {
		_vector vDotA = XMLoadFloat3(&dotA);
		_vector vDotB = XMLoadFloat3(&dotB);
		_vector vDotC = XMLoadFloat3(&dotC);
		_vector vCross = XMVector3Cross(vDotB - vDotA, vDotC - vDotA);
		if (0 > XMVectorGetY(vCross)) {
			swap(dotB, dotC);
		}
	}

	// dx9에 있던 IntersectTri
	inline static _bool IntersectTri(
		const _fvector& vPos, const _fvector& vDir,
		const _fvector& vertexA, const _gvector& vertexB, const _hvector& vertexC,
		_float& t, _float& u, _float& v)
	{
		_vector edge1 = vertexB - vertexA;
		_vector edge2 = vertexC - vertexA;
		_vector ray_cross_edge2 = XMVector3Cross(vDir, edge2);
		_float det = XMVectorGetX(XMVector3Dot(edge1, ray_cross_edge2));

		if (fabsf(det) <= FLT_EPSILON) {
			return false;
		}
		_float inverseDet = 1.f / det;
		_vector tv = XMVectorSubtract(vPos, vertexA);

		u = XMVectorGetX(XMVector3Dot(tv, ray_cross_edge2)) * inverseDet;
		if (u < 0.f || u > 1.f) {
			return false;
		}
		_vector q = XMVector3Cross(tv, edge1);
		v = XMVectorGetX(XMVector3Dot(vDir, q)) * inverseDet;
		if (v < 0.f || u + v > 1.f) {
			return false;
		}
		t = XMVectorGetX(XMVector3Dot(edge2, q)) * inverseDet;

		return t >= 0.f;
	}
	// 매트릭스 럴프
	inline static void MatrixLerp(	_In_ _float4x4* pMatOrigin, // Origin -> 0
							_In_ _float4x4* pMatTarget, // Target -> 1
							_Out_ _float4x4& matOut, _float fRatio) {
		_matrix matOrigin = XMLoadFloat4x4(pMatOrigin);
		_matrix matTarget = XMLoadFloat4x4(pMatTarget);
		MatrixLerp(&matOrigin, &matTarget, matOut, fRatio);
	}
	
	inline static void MatrixLerp(	_In_ _matrix* pMatOrigin, // Origin -> 0
							_In_ _matrix* pMatTarget, // Target -> 1
							_Out_ _float4x4& matOut, _float fRatio) {
		fRatio = Saturate(fRatio);

		_vector vTargetScale{}, vTargetRotq{}, vTargetTrans{};
		_vector vOriginScale{}, vOriginRotq{}, vOriginTrans{};
		XMMatrixDecompose(&vTargetScale, &vTargetRotq, &vTargetTrans, *pMatTarget);
		XMMatrixDecompose(&vOriginScale, &vOriginRotq, &vOriginTrans, *pMatOrigin);

		vTargetScale = XMVectorLerp(vOriginScale, vTargetScale, fRatio);
		vTargetRotq = XMQuaternionSlerp(vOriginRotq, vTargetRotq, fRatio);
		vTargetTrans = XMVectorLerp(vOriginTrans, vTargetTrans, fRatio);

		XMStoreFloat4x4(&matOut, (XMMatrixAffineTransformation(vTargetScale, XMVectorZero(), vTargetRotq, vTargetTrans)));
	}

	// 비슷하면 1 직교하면 0 완전 반대방향일수록 -1
	inline static _float DirectionCompare(_vector a, _vector b)
	{
		_vector vNormalA = XMVector3Normalize(a);
		_vector vNormalB = XMVector3Normalize(b);
		return XMVectorGetX(XMVector3Dot(vNormalA, vNormalB));
	}

	// hlsl에 있는 Saturate 
	inline static _float Saturate(_float fValue) {
		if (fValue > 1.f) {
			fValue = 1.f;
		}
		else if (fValue < 0.f) {
			fValue = 0.f;
		}
		return fValue;
	}

	// 입력 라디안이 들어오면 -PI ~ +PI로 정규화된 라디안으로 변경
	inline static _float NormalizeRadian(_float fRadian)
	{
		fRadian = fmodf(fRadian, XM_2PI);
		if (fRadian <= -XM_PI) {
			fRadian += XM_2PI;
		}
		else if (fRadian > XM_PI) {
			fRadian -= XM_2PI;
		}
		return fRadian;
	}

	// 입력라디안에서 출력라디안까지 로테이션 스피드만큼 돌려주고 보정해서 SrcRadian에 넣어줌
	inline static void	 RotateRadianTowards(_float& fSrcRadian, const _float& fDstRadian, _float fRotateSpeed) {
		_float fDiffRadian = fmodf(fSrcRadian - fDstRadian, XM_2PI);
		if (fDiffRadian < 0) {
			fDiffRadian += XM_2PI;
		}

		if (fDiffRadian < (fRotateSpeed * XM_PI) / 180.f) {
			fSrcRadian = fDstRadian;
			return;
		}
		if (fDiffRadian < XM_PI) {
			fSrcRadian -= (fRotateSpeed * XM_PI) / 180.f;
		}
		else if (fDiffRadian > XM_PI) {
			fSrcRadian += (fRotateSpeed * XM_PI) / 180.f;
		}
	}
	
	// 2D 평면에 사상된 방향을 교정해서 리턴
	// float -> Radian값
	// - 시계방향
	// + 반시계방향
	inline static _float Get_Direction2D(_float2 vOrigianlDir, _float2 vInputDir) {
		_vector vOriginal	= XMVector2Normalize(XMLoadFloat2(&vOrigianlDir));
		_vector vInput		= XMVector2Normalize(XMLoadFloat2(&vInputDir));
		
		_float fDiffAngle	= XMVectorGetX(XMVector2AngleBetweenNormals(vOriginal, vInput));
		_float fCross		= XMVectorGetZ(XMVector2Cross(vOriginal, vInput));

		return fDiffAngle * (fCross < 0 ? -1.f : 1.f);
	}
	inline static _float Get_Direction2D(_vector vOriginalDir, _vector vInputDir) {
		_vector vOriginal = XMVector2Normalize(XMVectorSet(XMVectorGetX(vOriginalDir), XMVectorGetZ(vOriginalDir), 0.f, 0.f));
		_vector vInput = XMVector2Normalize(XMVectorSet(XMVectorGetX(vInputDir), XMVectorGetZ(vInputDir), 0.f, 0.f));

		_float fDiffAngle = XMVectorGetX(XMVector2AngleBetweenNormals(vOriginal, vInput));
		_float fCross = XMVectorGetZ(XMVector2Cross(vOriginal, vInput));

		return fDiffAngle * (fCross < 0 ? -1.f : 1.f);
	}
	// yaw 360도 자동 커트
	// pitch 80도 클램프
	inline static void AdjustAccumulateDegreePitchYawDegree(_float2& vAccumulateDegreePitchYaw, float fPitchLimitLowerDegree = 60.f, float fPitchLimitUpperDegree = 60.f)
	{
		if (vAccumulateDegreePitchYaw.y > 360.f)
		{
			vAccumulateDegreePitchYaw.y -= 360.f;
		}
		else if (vAccumulateDegreePitchYaw.y < -360.f)
		{
			vAccumulateDegreePitchYaw.y += 360.f;
		}

		if (vAccumulateDegreePitchYaw.x > fPitchLimitUpperDegree)
		{
			vAccumulateDegreePitchYaw.x = fPitchLimitUpperDegree;
		}
		else if (vAccumulateDegreePitchYaw.x < -fPitchLimitLowerDegree)
		{
			vAccumulateDegreePitchYaw.x = -fPitchLimitLowerDegree;
		}
	}

	// yaw 360도 자동 커트
	// pitch 80도 클램프
	inline void AdjustAccumulateDegreePitchYawRadian(_float2& vAccumulateRadianPitchYaw, float fPitchLimitRadian = XMConvertToRadians(80.f))
	{
		if (vAccumulateRadianPitchYaw.y > XM_PI)
		{
			vAccumulateRadianPitchYaw.y -= XM_PI;
		}
		else if (vAccumulateRadianPitchYaw.y < -XM_PI)
		{
			vAccumulateRadianPitchYaw.y += XM_PI;
		}

		if (vAccumulateRadianPitchYaw.x > fPitchLimitRadian)
		{
			vAccumulateRadianPitchYaw.x = fPitchLimitRadian;
		}
		else if (vAccumulateRadianPitchYaw.x < -fPitchLimitRadian)
		{
			vAccumulateRadianPitchYaw.x = -fPitchLimitRadian;
		}
	}
	static inline _vector MakeQuaternionFromTo(_vector vAOrigin, _vector vBDest)
	{
		vAOrigin = XMVector3Normalize(vAOrigin);
		vBDest = XMVector3Normalize(vBDest);

		_float fDot = XMVectorGetX(XMVector3Dot(vAOrigin, vBDest));

		if ( 1.f - fDot < FLT_EPSILON) { // 같은 방향 제외
			return XMQuaternionIdentity();
		}
		if (fDot < -1.f + FLT_EPSILON) { // 반대방향 제외
			_vector vTempAxis = XMVector3Cross(vAOrigin, XMVectorSet(1.f, 0.f, 0.f, 0.f));
			_float fTempLength = XMVectorGetX(XMVector3Length(vTempAxis));

			if (fTempLength < FLT_EPSILON5) {
				vTempAxis = XMVector3Cross(vAOrigin, XMVectorSet(0.f, 0.f, 1.f, 0.f));
			}

			vTempAxis = XMVector3Normalize(vTempAxis);
			return XMQuaternionRotationAxis(vTempAxis, XM_PI);
		}

		_vector vRotAxis = XMVector3Cross(vAOrigin, vBDest);
		_float  fRotLength = XMVectorGetX(XMVector3Length(vRotAxis));
		if (fRotLength < FLT_EPSILON5) {
			return XMQuaternionIdentity();
		}
		vRotAxis = XMVector3Normalize(vRotAxis);

		_float fRotAngle = acosf(clamp(fDot, -1.f, 1.f));
		_vector vRotQ = XMQuaternionRotationAxis(vRotAxis, fRotAngle);
		return XMQuaternionNormalize(vRotQ);
	}


	inline static void SortHitsByDistance(vector<PSX::PxRaycastHit>& hits)
	{
		sort(hits.begin(), hits.end(),
			[](const PSX::PxRaycastHit& hitLeft, const PSX::PxRaycastHit& hitRight)
			{
				return hitLeft.distance < hitRight.distance;
			});
	}

	inline static void SortHitsByDistance(vector<PSX::PxSweepHit>& hits)
	{
		sort(hits.begin(), hits.end(),
			[](const PSX::PxSweepHit& hitLeft, const PSX::PxSweepHit& hitRight)
			{
				return hitLeft.distance < hitRight.distance;
			});
	}
	

	inline static _uint AlphabetToInt(_tchar Alphabet)
	{
		Alphabet = towupper(Alphabet);

		if (Alphabet < 'A' || Alphabet > 'Z')
			return 0;

		return (Alphabet - 'A');
	}
#ifdef _DEBUG

	inline static _bool DescribePxTransform(PSX::PxTransform& pxTransform, size_t iID)
	{
		PSX::PxVec3 vAxis = {};
		_float fAngle = {};
		pxTransform.q.toRadiansAndUnitAxis(fAngle, vAxis);
		GUI::Text((to_string((_int)iID) + "Pos : %.2f %.2f %.2f").c_str(), pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
		GUI::Text((to_string((_int)iID) + "fAngle vAxis : %.2f %.2f %.2f %.2f").c_str(), fAngle, vAxis.x, vAxis.y, vAxis.z);
		if (GUI::DragFloat3((to_string((_int)iID) + " Pos").c_str(), (_float*)&pxTransform.p, 0.01f, -2.f, 2.f, "%.2f")) {
			return true;
		}
		if (GUI::DragFloat((to_string((_int)iID) + " fAngle").c_str(), &fAngle, XMConvertToRadians(1.f), XMConvertToRadians(0.1f), 2.f * XM_2PI, "%.2f")) {
			pxTransform.q = PSX::PxQuat(fAngle, vAxis);
			return true;
		}
		return false;
	}

#endif // _DEBUG

	// 루트의 끝에 있는 조인트 위치를 구함
	inline static PSX::PxTransform Calc_JointPosFromRoute(PSX::PxRigidActor* pRouteActor, _float3 vCapsuleHalfLengths, bool bIsHead)
	{
		PSX::PxShape* shapeArray[1] = {};
		pRouteActor->getShapes(shapeArray, 1);

		PSX::PxShape* routeShape = shapeArray[0];
		PSX::PxTransform pxRouteWorld = PSX::PxShapeExt::getGlobalPose(*routeShape, *pRouteActor);
		PSX::PxVec3 capsuleAxisWorld = pxRouteWorld.q.rotate(PSX::PxVec3(1.f, 0.f, 0.f));

		PSX::PxTransform pxResult;
		pxResult.q = pxRouteWorld.q;
		pxResult.p = pxRouteWorld.p + (bIsHead ? 1.f : -1.f) * capsuleAxisWorld * (vCapsuleHalfLengths.y + vCapsuleHalfLengths.x);

		return pxResult;
	}
	static _bool Modify_D6Joint(PSX::PxD6Joint& joint, _uint iID)
	{
		_bool bIsModified = false;

		const char* motionItems[] = { "Locked", "Limited", "Free" };

		struct AxisRow { PSX::PxD6Axis::Enum axis; const char* label; };
		const AxisRow axisRows[] =
		{
			{ PSX::PxD6Axis::eX,      "X Motion"      },
			{ PSX::PxD6Axis::eY,      "Y Motion"      },
			{ PSX::PxD6Axis::eZ,      "Z Motion"      },
			{ PSX::PxD6Axis::eTWIST,  "Twist Motion"  },
			{ PSX::PxD6Axis::eSWING1, "Swing1 Motion" },
			{ PSX::PxD6Axis::eSWING2, "Swing2 Motion" },
		};

		for (const AxisRow& row : axisRows)
		{
			const PSX::PxD6Motion::Enum pxCurMotion = joint.getMotion(row.axis);

			int iCurIndex = 0;
			if (pxCurMotion == PSX::PxD6Motion::eLOCKED)  iCurIndex = 0;
			else if (pxCurMotion == PSX::PxD6Motion::eLIMITED) iCurIndex = 1;
			else                                               iCurIndex = 2; // eFREE

			int iNewIndex = iCurIndex;
			std::string uiId = std::string(row.label) + " ##" + std::to_string(iID);

			if (GUI::Combo(uiId.c_str(), &iNewIndex, motionItems, IM_ARRAYSIZE(motionItems)) && iNewIndex != iCurIndex)
			{
				PSX::PxD6Motion::Enum pxNewMotion =
					(iNewIndex == 0) ? PSX::PxD6Motion::eLOCKED :
					(iNewIndex == 1) ? PSX::PxD6Motion::eLIMITED :
					PSX::PxD6Motion::eFREE;

				joint.setMotion(row.axis, pxNewMotion);
				bIsModified = true;
			}
		}

		const _bool bIsLinearLimited =
			(joint.getMotion(PSX::PxD6Axis::eX) == PSX::PxD6Motion::eLIMITED) ||
			(joint.getMotion(PSX::PxD6Axis::eY) == PSX::PxD6Motion::eLIMITED) ||
			(joint.getMotion(PSX::PxD6Axis::eZ) == PSX::PxD6Motion::eLIMITED);

		const _bool bIsSwingLimited =
			(joint.getMotion(PSX::PxD6Axis::eSWING1) == PSX::PxD6Motion::eLIMITED) ||
			(joint.getMotion(PSX::PxD6Axis::eSWING2) == PSX::PxD6Motion::eLIMITED);

		const _bool bIsTwistLimited =
			(joint.getMotion(PSX::PxD6Axis::eTWIST) == PSX::PxD6Motion::eLIMITED);

		PSX::PxJointLinearLimit pxLinearLimit = joint.getLinearLimit();
		_float fLinearLimit = pxLinearLimit.value;

		if (GUI::DragFloat((std::string("LinearLimit ##") + std::to_string(iID)).c_str(), &fLinearLimit, 0.01f, 0.f, 10.f, "%.3f"))
			bIsModified = true;

		PSX::PxJointLimitCone pxSwingLimit = joint.getSwingLimit();
		PSX::PxJointAngularLimitPair pxTwistLimit = joint.getTwistLimit();

		_float fSwingDeg = XMConvertToDegrees(pxSwingLimit.yAngle);
		_float fTwistAbsDeg = XMConvertToDegrees(PSX::PxMax(PSX::PxAbs(pxTwistLimit.lower), PSX::PxAbs(pxTwistLimit.upper)));

		if (GUI::DragFloat((std::string("SwingDeg ##") + std::to_string(iID)).c_str(), &fSwingDeg, 1.f, 0.f, 180.f, "%.2f"))
			bIsModified = true;

		if (GUI::DragFloat((std::string("TwistAbsDeg ##") + std::to_string(iID)).c_str(), &fTwistAbsDeg, 1.f, 0.f, 180.f, "%.2f"))
			bIsModified = true;

		PSX::PxD6JointDrive pxSlerpDrive = joint.getDrive(PSX::PxD6Drive::eSLERP);
		_bool bIsSlerpDriveEnabled = (pxSlerpDrive.stiffness > 0.f) || (pxSlerpDrive.damping > 0.f) || (pxSlerpDrive.forceLimit > 0.f);

		if (GUI::Checkbox((std::string("Enable SLERP Drive ##") + std::to_string(iID)).c_str(), &bIsSlerpDriveEnabled))
			bIsModified = true;

		if (GUI::SliderFloat((std::string("SLERP stiffness ##") + std::to_string(iID)).c_str(), &pxSlerpDrive.stiffness, 0.f, 100.f, "%.2f"))
			bIsModified = true;

		GUI::SameLine();

		if (GUI::SliderFloat((std::string("SLERP damping ##") + std::to_string(iID)).c_str(), &pxSlerpDrive.damping, 0.f, 100.f, "%.2f"))
			bIsModified = true;

		PSX::PxD6JointDrive pxLinearDriveX = joint.getDrive(PSX::PxD6Drive::eX);
		_bool bIsLinearDriveEnabled = (pxLinearDriveX.stiffness > 0.f) || (pxLinearDriveX.damping > 0.f) || (pxLinearDriveX.forceLimit > 0.f);

		if (GUI::Checkbox((std::string("Enable Linear Drive(XYZ) ##") + std::to_string(iID)).c_str(), &bIsLinearDriveEnabled))
			bIsModified = true;

		if (GUI::SliderFloat((std::string("Linear stiffness ##") + std::to_string(iID)).c_str(), &pxLinearDriveX.stiffness, 0.f, 100.f, "%.2f"))
			bIsModified = true;

		GUI::SameLine();

		if (GUI::SliderFloat((std::string("Linear damping ##") + std::to_string(iID)).c_str(), &pxLinearDriveX.damping, 0.f, 100.f, "%.2f"))
			bIsModified = true;

		if (bIsModified)
		{
			pxLinearLimit.value = PSX::PxMax(0.f, fLinearLimit);
			joint.setLinearLimit(pxLinearLimit);

			const _float fSwingRad = XMConvertToRadians(fSwingDeg);
			pxSwingLimit.yAngle = fSwingRad;
			pxSwingLimit.zAngle = fSwingRad;
			joint.setSwingLimit(pxSwingLimit);

			const _float fTwistAbsRad = XMConvertToRadians(fTwistAbsDeg);
			pxTwistLimit.lower = -fTwistAbsRad;
			pxTwistLimit.upper = +fTwistAbsRad;
			joint.setTwistLimit(pxTwistLimit);

			if (!bIsSlerpDriveEnabled)
				joint.setDrive(PSX::PxD6Drive::eSLERP, PSX::PxD6JointDrive(0.f, 0.f, 0.f, pxSlerpDrive.flags & PSX::PxD6JointDriveFlag::eACCELERATION));
			else
				joint.setDrive(PSX::PxD6Drive::eSLERP, pxSlerpDrive);

			if (!bIsLinearDriveEnabled)
			{
				const PSX::PxD6JointDrive pxOff(0.f, 0.f, 0.f, pxLinearDriveX.flags & PSX::PxD6JointDriveFlag::eACCELERATION);
				joint.setDrive(PSX::PxD6Drive::eX, pxOff);
				joint.setDrive(PSX::PxD6Drive::eY, pxOff);
				joint.setDrive(PSX::PxD6Drive::eZ, pxOff);
			}
			else
			{
				joint.setDrive(PSX::PxD6Drive::eX, pxLinearDriveX);
				joint.setDrive(PSX::PxD6Drive::eY, pxLinearDriveX);
				joint.setDrive(PSX::PxD6Drive::eZ, pxLinearDriveX);
			}
		}

		return bIsModified;
	}


	// 힌지 월드축을 가져와서 레볼루트 조인트의 회전 쿼터니언을 구함
	inline static PSX::PxQuat Calc_RevJointPosFromWorldHingeAxis(const PSX::PxVec3& pxHingeWorldAxis)
	{
		PSX::PxVec3 pxBasicAxis = PSX::PxVec3(0.f, 1.f, 0.f);
		if (PSX::PxAbs(pxHingeWorldAxis.dot(pxBasicAxis)) > 0.99f)
		{
			pxBasicAxis = PSX::PxVec3(0.f, 0.f, 1.f);
		}

		PSX::PxVec3 zAxis = pxHingeWorldAxis.cross(pxBasicAxis).getNormalized();
		PSX::PxVec3 yAxis = zAxis.cross(pxHingeWorldAxis).getNormalized();

		PSX::PxMat33 pxResultMatrix(pxHingeWorldAxis, yAxis, zAxis);
		return PSX::PxQuat(pxResultMatrix);
	}

#pragma endregion
#pragma region FileSystem
	//static void Folder_Func(/* 재귀적으로 탐색할지		*/	_In_	_bool											bRecursive,
	//						/* 탐색할 폴더 경로			*/	_In_	const _wstring&									TargetPath,
	//						/* 탐색할 때마다 실행할 로직*/	_In_	function<void(filesystem::directory_entry)>		funcLogic,
	//						/* 탐색할 확장자들			*/	_In_	const vector<_wstring>&							targetExtentions,
	//						/* 결과 경로들				*/	_Out_	vector<_wstring>&								Paths ) 
	//{
	//	// 경로 예시 -> "../Bin/Resources/Models/Environment/Hogwarts/SUB_ClockTowerCourtyard/Static_Mesh_EXT"
	//	filesystem::path path = TargetPath;
	//	for (const filesystem::directory_entry& file : filesystem::directory_iterator(path, filesystem::directory_options::skip_permission_denied)) 
	//	{
	//		if (file.is_directory()){
	//			if (true == bRecursive) {
	//				Folder_Func(bRecursive, file.path().wstring(), funcLogic, targetExtentions, Paths);
	//			}
	//			continue;
	//		}
	//		if (true == targetExtentions.empty()) { // 확장자 제한 없음
	//			Paths.emplace_back(file.path().wstring());
	//			funcLogic(file);
	//			continue;
	//		}
	//		for (const _wstring& ext : targetExtentions) { // 확장자 제한 있음
	//			if (file.path().extension() == ext) {
	//				Paths.emplace_back(file.path().wstring());
	//				funcLogic(file);
	//				break;
	//			}
	//		}
	//		// file.path().extension(); // 확장자
	//		// file.path().filename();  // 파일명(확장자포함)
	//		// file.path().stem();      // 파일명(확장자제외)
	//		// file.path().parent_path(); // 상위 폴더 경로
	//		// file.path().wstring(); // 전체 경로
	//		// file.path().string();  // 전체 경로 (string)
	//	}
	//}
#pragma endregion

private:
	inline static void DecodeRGBA(_uint hex, float& r, float& g, float& b, float& a, bool hasAlpha)
	{
		if (hasAlpha) { // 0xRRGGBBAA
			r = ((hex >> 24) & 0xFF) / 255.0f;
			g = ((hex >> 16) & 0xFF) / 255.0f;
			b = ((hex >> 8) & 0xFF) / 255.0f;
			a = (hex & 0xFF) / 255.0f;
		}
		else { // 0xRRGGBB
			r = ((hex >> 16) & 0xFF) / 255.0f;
			g = ((hex >> 8) & 0xFF) / 255.0f;
			b = (hex & 0xFF) / 255.0f;
			a = 1.0f;
		}
	}
};
NS_END
