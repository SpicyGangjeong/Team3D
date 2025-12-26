#pragma once

NS_BEGIN(Engine)


template<typename T>
void	Safe_Delete(T& Pointer)
{
	if (nullptr != Pointer)
	{
		delete Pointer;
		Pointer = nullptr;
	}
}

template<typename T>
void	Safe_Delete_Array(T& Pointer)
{
	if (nullptr != Pointer)
	{
		delete[] Pointer;
		Pointer = nullptr;
	}
}

template<typename T>
unsigned long Safe_AddRef(T& pInstance)
{
	unsigned long		dwRefCnt = 0;

	if (nullptr != pInstance){
		dwRefCnt = pInstance->AddRef();
	}

	return dwRefCnt;
}

template<typename T>
unsigned long Safe_Release(T& pInstance)
{
	unsigned long		dwRefCnt = 0;

	if (nullptr != pInstance) {
		dwRefCnt = pInstance->Release();
	}
	pInstance = nullptr;
	return dwRefCnt;
}

static PSX::PxTransform XMWorldToPx_NoScale(const _fmatrix& WorldMatrix)
{
	_vector vPos, vRotq, vScale;
	XMMatrixDecompose(&vScale, &vRotq, &vPos, WorldMatrix);

	vRotq = XMQuaternionNormalize(vRotq);

	PSX::PxTransform out;
	XMStoreFloat3((_float3*)&out.p, vPos);
	XMStoreFloat4((_float4*)&out.q, vRotq);
	return out;
}
// 주의, XMWorldToPx_NoScale 만들었을 때는 lh rh 변환을 생각안하고 했었음
// 지금 쓰는 이 함수는 lh -> rh 자동 변환임
static _matrix PxToXMWorld_NoScale(const PSX::PxTransform& pxTransform)
{
	_vector vPos = XMVectorSet(
		pxTransform.p.x,
		pxTransform.p.y,
		pxTransform.p.z,
		1.f
	);

	_vector vRotQ = XMVectorSet(
		-pxTransform.q.x,
		-pxTransform.q.y,
		pxTransform.q.z,
		pxTransform.q.w
	);

	vRotQ = XMQuaternionNormalize(vRotQ);

	_matrix rotationMatrix = XMMatrixRotationQuaternion(vRotQ);
	_matrix translationMatrix = XMMatrixTranslationFromVector(vPos);

	_matrix worldMatrix = rotationMatrix * translationMatrix;
	return worldMatrix;
}


static void GUIHelpMarker(const char* desc)
{
	GUI::SameLine();
	GUI::TextDisabled("(?)");
	if (GUI::BeginItemTooltip())
	{
		GUI::PushTextWrapPos(GUI::GetFontSize() * 35.0f);
		GUI::TextUnformatted((_string("u8") + desc).c_str());
		GUI::PopTextWrapPos();
		GUI::EndTooltip();
	}
}

NS_END
