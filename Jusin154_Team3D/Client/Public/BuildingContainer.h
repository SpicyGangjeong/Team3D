#pragma once

#include "Client_Define.h"
#include "MapContainer.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Box;
class COcclusionQuery;
NS_END

NS_BEGIN(Client)

class CBuildingContainer final : public CMapContainer // OCCLUSION CONTAINER
{
public:
	typedef struct tagMapContainerDesc : public CMapContainer::MAP_CONTAINER_DESC
	{
		_float3			vContainerMin;
		_float3			vContainerMax;
	}MAP_CONTAINER_DESC;

private:
	CBuildingContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuildingContainer(const CBuildingContainer& rhs);
	~CBuildingContainer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow()override;
	virtual HRESULT Render_BoundingBox() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CVIBuffer_Box*			m_pVIBufferCom = { nullptr };
	COcclusionQuery*		m_pOcclusionQueryCom = { nullptr };

	_float					m_fRadius = {};
	_float3					m_vExtentWorldPosition = {};

private:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Ready_Components(void* pArg) override;
	virtual HRESULT		Bind_ShaderResources()override;

public:
	static CBuildingContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner = nullptr)override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity()override;
#endif // _DEBUG

};

NS_END
