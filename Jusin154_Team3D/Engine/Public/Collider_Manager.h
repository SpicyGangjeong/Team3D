#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCollider_Manager final : public CBase
{
	NO_COPY(CCollider_Manager)
private:
	CCollider_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Add_ColliderGroup(_uint iColliderGroup, class CCollider* pBounding);
	void	Refresh_Collider_Manager();
	void	Collide(_uint iCollideGroupA, _uint iCollideGroupB);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	_uint	m_iMaxColliderGroup = {};
	list<class CCollider*>* m_pColliderObjects = { nullptr };

private:
	HRESULT Initialize(_uint iMaxColliderGroup);

public:
	static CCollider_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iMaxColliderGroup);
	virtual void Free();
};

NS_END