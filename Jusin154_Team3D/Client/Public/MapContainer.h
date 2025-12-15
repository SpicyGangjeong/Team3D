#pragma once

#include "Client_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Client)

class CMapContainer abstract : public CContainerObject
{
public:
	typedef struct tagMapContainerDesc
	{
		_float3			vPosition;
		_float3			vScale;
		_float3			vRotation;
	}MAP_CONTAINER_DESC;

protected:
	CMapContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapContainer(const CMapContainer& rhs);
	~CMapContainer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;

	void ReadyForPhysX();
	void ConvertToPhysX();

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
		m_bHasCollisionMesh = true;
		m_ColiisonPartObjects.push_back(pPartObject);

		return S_OK;
	}

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Ready_Components(void* pArg) override;

protected:
	_uint								m_iMaxLodLevel = {};
	_bool								m_bHasCollisionMesh = { false };
	vector<class CPartObject*>			m_ColiisonPartObjects = {};

public:
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
