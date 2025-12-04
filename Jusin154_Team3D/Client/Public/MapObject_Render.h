#pragma once

#include "Client_Define.h"
#include "MapPartObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)

class CMapObject_Render final : public CMapPartObject // Render PartObject
{
private:
	CMapObject_Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Render(const CMapObject_Render& Prototype);
	virtual ~CMapObject_Render() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual _wstring Get_PrototypeTag(_uint iLodIndex = 0);
	HRESULT Add_LodModel(const _tchar* pModelPrototypeTag);

private:
	MAPOBJECT_RENDER_TYPE	m_Type = {};
	CShader*				m_pShaderCom = { nullptr };
	vector<CModel*>			m_pModelComs;

	_uint					m_iLodIndex = {};
	_uint					m_iShaderPass = {};
	_uint					m_iNumMeshe = {};

	_float					m_fRadius = { 0.f };


	vector<_wstring>		m_ModelPrototypeTags;
	vector<_uint>			m_ModelPathIndices = { };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject_Render* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
