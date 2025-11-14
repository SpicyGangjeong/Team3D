#pragma once

#include "Editor_Define.h"
#include "MapContainer.h"

NS_BEGIN(Editor)

class CBuildingContainer final : public CMapContainer
{
public:
	CBuildingContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuildingContainer(const CBuildingContainer& rhs);
	~CBuildingContainer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	template<typename T>
	HRESULT Add_Part(const _string& strPartKey, _uint iPrototypeLevelIndex, T** ppOut, void* pArg = nullptr) {
		return Add_PartObject<T>(strPartKey, iPrototypeLevelIndex, ppOut, pArg);
	}
	template<typename T>
	HRESULT Add_Collision(_uint iPrototypeLevelIndex, void* pArg = nullptr)
	{
		T* pPartObject = m_pGameInstance->Clone_Prototype<T>(iPrototypeLevelIndex, pArg, this);

		if (nullptr == pPartObject) {
			return E_FAIL;
		}
		
		m_ColiisonPartObjects.push_back(pPartObject);

		return S_OK;
	}

	vector<class CPartObject*>* Get_Collision() { return &m_ColiisonPartObjects; }



private:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Ready_Components(void* pArg) override;
	virtual HRESULT		Bind_ShaderResources()override;

public:
	static CBuildingContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner = nullptr)override;
	virtual void Free() override;
	virtual void Describe_Entity()override;
};

NS_END
