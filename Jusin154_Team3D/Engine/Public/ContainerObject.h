#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
public:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& rhs);
	~CContainerObject() = default;
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	template<typename T>
	T* Get_PartObject(const _string& strPartKey) {
		unordered_map<_string, class CPartObject*>::iterator iter = m_PartObjects.find(strPartKey);

		if (iter != m_PartObjects.end() && typeid(*(*iter).second) == typeid(T)) 
		{
			return dynamic_cast<T*>((*iter).second);
		}
		return nullptr;
	}

	template<typename T>
	T* Get_PartObject() {
		
		for (auto& pPart : m_PartObjects)
		{
			if (typeid((*pPart.second)) == typeid(T))
			{
				return dynamic_cast<T*>(pPart.second);
			}
			
		}
		return nullptr;
	}

protected:
	unordered_map<_string, class CPartObject*>			m_PartObjects;
	
protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Ready_Components(void* pArg) override;
	template<typename T>
	HRESULT Add_PartObject(const _string& strPartKey, _uint iPrototypeLevelIndex, T** ppOut, void* pArg = nullptr) {
		if (nullptr != Get_PartObject<T>(strPartKey)) {
			return E_FAIL;
		}

		T* pPartObject = m_pGameInstance->Clone_Prototype<T>(iPrototypeLevelIndex, pArg, this);

		if (nullptr == pPartObject){
			return E_FAIL;
		}
		pair<unordered_map<_string, class CPartObject*>::iterator, _bool> pairResult = m_PartObjects.emplace(strPartKey, pPartObject);
		if (true != pairResult.second) { // 삽입 실패
			assert(false); // 해당 string은 이미 차지되어 있습니다.
			SAFE_RELEASE(pPartObject);
			return E_FAIL;
		}
		if (nullptr != ppOut) {
			*ppOut = pPartObject;
			SAFE_ADDREF(pPartObject);
		}
		return S_OK;
	}

public:
	virtual void Free() override;
};

NS_END
