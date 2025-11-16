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
	static _float4 ColorRGB_A_HEXtoFLOAT4(_uint hexCode, _float alpha) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, false); a = alpha;
		return _float4(r, g, b, a);
	}

	// [(255, 255, 255, 255)] -> [(0~1, 0~1, 0~1, 0~1)]
	// 0xRRGGBBAA -> _float4( 0~1, 0~1, 0~1, 0~1 )
	static _float4 ColorRGBA_HEXtoFLOAT4(_uint hexCode) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, true);
		return _float4(r, g, b, a);
	}

	// [(255, 255, 255, xx), alpha] -> [(0~1, 0~1, 0~1, alpha)]
	// 0xRRGGBBxx, alpha -> _vector( 0~1, 0~1, 0~1, alpha )
	static _vector ColorRGB_A_HEXtoVECTOR(_uint hexCode, _float alpha) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, false); a = alpha;
		return XMVectorSet(r, g, b, a);
	}

	// [(255, 255, 255, 255)] -> [(0~1, 0~1, 0~1, 0~1)]
	// 0xRRGGBBAA -> _vector( 0~1, 0~1, 0~1, 0~1 )
	static _vector ColorRGBA_HEXtoVECTOR(_uint hexCode) {
		float r, g, b, a; DecodeRGBA(hexCode, r, g, b, a, true);
		return XMVectorSet(r, g, b, a);
	}
#pragma endregion

	// wstring --> string 변환
	static _string ToString(const _wstring& var)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, var.c_str(),
			(int)var.size(), nullptr, 0, nullptr, nullptr);

		string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, var.c_str(),
			(int)var.size(), &strTo[0], size_needed, nullptr, nullptr);

		return strTo;
	}

	// string --> wstring 변환
	static _wstring ToWstring(const _string& var)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, var.c_str(),
			(int)var.size(), nullptr, 0);

		wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, var.c_str(),
			(int)var.size(), &wstrTo[0], size_needed);

		return wstrTo;
	}

	// ratio로 현재 value 가져오기
	static _float Lerp_f1D(_float fA, _float fB, _float fRatio)
	{
		return fA + (fB - fA) * fRatio;
	}

	// value로 현재 ratio 가져오기
	static _float InverseLerp_f1D(_float fA, _float fB, _float fValue)
	{
		if (fA == fB) return 0.0f;
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

	// dx9에 있던 그거
	static _bool IntersectTri(
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
		if (u < 0.f || u > 1.f) return false;
		_vector q = XMVector3Cross(tv, edge1);
		v = XMVectorGetX(XMVector3Dot(vDir, q)) * inverseDet;
		if (v < 0.f || u + v > 1.f) return false;
		t = XMVectorGetX(XMVector3Dot(edge2, q)) * inverseDet;

		return t >= 0.f;
	}

	// source에 keyword가 있는지 검사
	static _bool ContainsString(const char* source, const char* keyword) {
		if (!source || !keyword || *keyword == '\0') {
			return false;
		}
		return (strstr(source, keyword) != nullptr);
	}

	// 매트릭스 럴프
	static void MatrixLerp(	_In_ _float4x4* pMatOrigin, // Origin -> 0
							_In_ _float4x4* pMatTarget, // Target -> 1
							_Out_ _float4x4& matOut, _float fRatio) {
		if (0.f > fRatio) {
			fRatio = 0.f;
		}
		if (1.f < fRatio) {
			fRatio = 1.f;
		}
		_vector vTargetScale{}, vTargetRotq{}, vTargetTrans{};
		_vector vOriginScale{}, vOriginRotq{}, vOriginTrans{};
		XMMatrixDecompose(&vTargetScale, &vTargetRotq, &vTargetTrans, XMLoadFloat4x4(pMatTarget));
		XMMatrixDecompose(&vOriginScale, &vOriginRotq, &vOriginTrans, XMLoadFloat4x4(pMatOrigin));

		vTargetScale = XMVectorLerp(vOriginScale, vTargetScale, fRatio);
		vTargetRotq = XMQuaternionSlerp(vOriginRotq, vTargetRotq, fRatio);
		vTargetTrans = XMVectorLerp(vOriginTrans, vTargetTrans, fRatio);

		XMStoreFloat4x4(&matOut, (XMMatrixAffineTransformation(vTargetScale, XMVectorZero(), vTargetRotq, vTargetTrans)));
	}

	// 비슷하면 1 직교하면 0 완전 반대방향일수록 -1
	static _float DirectionCompare(_vector a, _vector b)
	{
		_vector vNormalA = XMVector3Normalize(a);
		_vector vNormalB = XMVector3Normalize(b);
		return XMVectorGetX(XMVector3Dot(vNormalA, vNormalB));
	}

	// hlsl에 있는 Saturate 그거임
	static _float Saturate(_float fValue) {
		if (fValue > 1.f) {
			fValue = 1.f;
		}
		else if (fValue < 0.f) {
			fValue = 0.f;
		}
		return fValue;
	}

	// 입력 라디안이 들어오면 -PI ~ +PI로 정규화된 라디안으로 변경
	static _float NormalizeRadian(_float fRadian)
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
