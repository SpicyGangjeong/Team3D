#pragma once

#include "Editor_Define.h"
#include "MapObject.h"

NS_BEGIN(Editor)

class CMapObject_Static final : public CMapObject // LOD 모델이 없는 애들
{
private:
	CMapObject_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Static(const CMapObject_Static& Prototype);
	virtual ~CMapObject_Static() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _wstring Get_PrototypeTag(_uint iLodIndex = 0);

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	_wstring			m_strModelPrototypeTag;
	_uint				m_iModelPathIndex = { UINT_MAX };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END