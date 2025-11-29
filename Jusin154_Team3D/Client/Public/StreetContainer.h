#pragma once

#include "Client_Define.h"
#include "MapContainer.h"
NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Box;
class COcclusionQuery;
NS_END

NS_BEGIN(Client)

class CStreetContainer final : public CMapContainer
{
	typedef struct tagStreetContainerDesc : public MAP_CONTAINER_DESC
	{
		_float3		vExtentRadius = {};
		_float3		vExtentPosition = {};
	}STREET_CONTAINER_DESC;

private:
	CStreetContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStreetContainer(const CStreetContainer& rhs);
	~CStreetContainer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_BoundingBox() override;

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

private:
	CShader*					m_pShaderCom = { nullptr };
	CVIBuffer_Box*				m_pVIBufferCom = { nullptr };
	COcclusionQuery*			m_pOcclusionQueryCom = { nullptr };

	_float3						m_vExtentRadius = {};
	_float3						m_vExtentPosition = {};
	_float4x4					m_BoundingBoxWorldMatirx;


private:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Ready_Components(void* pArg) override;
	virtual HRESULT		Bind_ShaderResources()override;

public:
	static CStreetContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner = nullptr)override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity()override;
#endif // _DEBUG

};

NS_END
