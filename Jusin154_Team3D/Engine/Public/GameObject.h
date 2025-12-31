#pragma once

#include "Transform.h"
#include "GameInstance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract
	: public CBase
{

protected:
	CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual HRESULT Render();
	virtual HRESULT Render_Shadow(SHADOW eType) { return S_OK; }
	virtual HRESULT Render_Blur() { return S_OK; }
	virtual HRESULT Render_Bloom() { return S_OK; }
	virtual HRESULT Render_OutLine() { return S_OK; }
	virtual HRESULT Render_BoundingBox() { return S_OK; }

	virtual _float Get_Depth(); // z 소팅 할 때 쓰시면 됨

	// 내가 맞음 ( Collider_Manager가 부름 )
	virtual void OnCollision(CGameObject* pOther = nullptr ,void* pDesc = nullptr);

	// 내가 때림 ( 히트박스가 부름 )
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr);

	virtual _vector Get_WorldPostion();

	_bool	isDead() const { return m_bDead; }
	void	Set_Dead() { m_bDead = true; }

	_uint	Get_ObjectTag() const { return m_iObjectTag; }
	_uint	Get_CollisionID() const { return m_iCollisionID; }
	_bool	Get_Visible() const { return m_bVisible; }
	void	Set_Visible(_bool bValue) { m_bVisible = bValue; }
	CGameObject* Get_Owner() const { return m_pOwner; }

public:
	HRESULT Add_Asset_Component(_uint iTargetLevel, const _wstring strPrototypeTag, CComponent** ppOut, void* pArg = nullptr);
	template<typename T>
	HRESULT Add_Component(_uint iPrototypeLevelIndex, T** ppOut, void* pArg = nullptr) {
		T* pComponent = m_pGameInstance->Clone_Prototype<T>(iPrototypeLevelIndex, pArg, this);

		if (nullptr == pComponent) {
			return E_FAIL;
		}

		m_Components.push_back(pComponent);
		*ppOut = pComponent;
		SAFE_ADDREF(pComponent);

		return S_OK;
	}
	template<typename T>
	T* Get_Component() {
		for (auto& pComponent : m_Components) {
			if (typeid(*pComponent) == typeid(T))
			{
				return dynamic_cast<T*>(pComponent);
			}
		}
		return nullptr;
	}


	template<typename T>
	void Remove_Component()
	{
		for (auto it = m_Components.begin(); it != m_Components.end(); )
		{
			if (dynamic_cast<T*>(*it))
			{
				SAFE_RELEASE(*it);
				it = m_Components.erase(it);
			}
			else
				++it;
		}
	}

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Compute_Depth();

protected:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameObject*		m_pOwner = { nullptr };
	class CTransform*		m_pTransformCom = { nullptr };

	list<class CComponent*> m_Components = {};
	_uint	m_iCollisionID = { 0 };
	_uint	m_iObjectTag = { 0 };
	_float	m_fCamDepth = { FLT_MAX };
	_bool	m_bDead = { false };
	_bool   m_bVisible = { true };
	_bool   m_bCloned = { false };
	_ubyte	m_iShadow = { 0 };
protected:
	void Set_Shadow(pair<_bool, _ubyte> shadowResult);
	virtual HRESULT		Bind_ShaderResources()PURE;
	virtual HRESULT		Ready_Components(void* pArg);
public:
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner = nullptr)PURE;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity()PURE;
#endif // _DEBUG

};

NS_END
