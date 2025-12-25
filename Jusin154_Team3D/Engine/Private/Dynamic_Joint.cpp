#include "pch.h"
#include "Dynamic_Joint.h"

CDynamic_Joint::CDynamic_Joint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CDynamic_Joint::CDynamic_Joint(const CDynamic_Joint& rhs)
	: CComponent(rhs)
{
}

HRESULT CDynamic_Joint::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CDynamic_Joint::Initialize(void* pArg)
{
	return E_NOTIMPL;
}
#ifdef _DEBUG

HRESULT CDynamic_Joint::Add_DebugShape()
{
	return E_NOTIMPL;
}
HRESULT CDynamic_Joint::Render()
{
	return E_NOTIMPL;
}

#endif // _DEBUG


CDynamic_Joint* CDynamic_Joint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CComponent* CDynamic_Joint::Clone(void* pArg, CGameObject* pOwner)
{
	return nullptr;
}

void CDynamic_Joint::Free()
{
}
#ifdef _DEBUG

void CDynamic_Joint::Describe_Entity()
{
}

#endif // _DEBUG
