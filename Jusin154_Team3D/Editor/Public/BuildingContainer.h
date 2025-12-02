#pragma once

#include "Editor_Define.h"
#include "MapContainer.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Box;
class COcclusionQuery;
class CCollider;
NS_END

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
	virtual HRESULT Render_BoundingBox() override;

	_float3 Get_BoundingBox_Min() { return vContainerMin; }
	_float3 Get_BoundingBox_Max() { return vContainerMax; }
	void	Set_BoundingBox();
	_bool	Is_OcclusionPassed() { return m_bOcclusionPassed; }
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
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Box*		m_pVIBufferCom = { nullptr };
	COcclusionQuery*	m_pOcclusionQueryCom = { nullptr };
	CCollider*			m_pCollider = { nullptr };

	_bool				m_bOcclusionPassed = { true };
	_float4x4			m_BoundingBoxWorldMatirx = {};
	_float3				m_vBoxCorners[8] = {};

	_float3				m_vExtentPosition = {};
	_float3				m_vExtentRadius = {};
	_float3				vContainerMin = { 999.f, 999.f, 999.f };
	_float3				vContainerMax = { -999.f, -999.f, -999.f };

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
