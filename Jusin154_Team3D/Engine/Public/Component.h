#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Render() { return S_OK; }
	virtual class CGameObject* Get_Owner() { return m_pOwner; }

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CGameObject*		m_pOwner = { nullptr };
	_bool					m_bCloned = { false };

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr)PURE;
	virtual void Free();

#ifdef _DEBUG
	virtual void Describe_Entity()PURE; // IMGUI 전용
#endif

};

NS_END
