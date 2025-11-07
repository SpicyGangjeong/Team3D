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

NS_END