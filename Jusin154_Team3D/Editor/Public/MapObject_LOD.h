#pragma once

#include "Editor_Define.h"
#include "MapObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Editor)

class CMapObject_LOD final : public CMapObject // LOD 있는 애들
{
private:
	CMapObject_LOD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_LOD(const CMapObject_LOD& Prototype);
	virtual ~CMapObject_LOD() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _wstring Get_PrototypeTag(_uint iLodIndex = 0);
	void Get_BoundingBox(_float3* pMinPosition, _float3* pMaxPosition);
	HRESULT Add_LodModel(const _tchar* pModelPrototypeTag);
	void Set_KeyIndex(_uint iIndex) {
		m_iKeyIndex = iIndex;
	}

private:
	_bool					m_isVisible = { true };
	_bool					m_isVisibleSelect = { true };
	MAPOBJECT_RENDER_TYPE	m_Type = {};
	CShader*				m_pShaderCom = { nullptr };
	vector<CModel*>			m_pModelComs;

	_uint					m_iLodIndex = {};
	_uint					m_iShaderPass = {};
	_float					m_fGlassRatio = {0.5f};

	CTexture*				m_pDefaultGlassTextureCom = { nullptr };

	vector<_wstring>		m_ModelPrototypeTags;
	vector<_uint>			m_ModelPathIndices = { };
	_float3					m_CombinedExtendPosition = {};


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject_LOD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
