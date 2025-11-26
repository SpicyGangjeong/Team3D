#pragma once

#include "Client_Define.h"
#include "MapPartObject.h"

NS_BEGIN(Client)

class CMapObject_Collision final : public CMapPartObject // 충돌용 Mesh
{
private:
	CMapObject_Collision(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Collision(const CMapObject_Collision& Prototype);
	virtual ~CMapObject_Collision() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _wstring Get_PrototypeTag(_uint iLodIndex = 0);

private:
	CShader* m_pShaderCom = { nullptr };
	vector<CModel*>		m_pModelComs;

	_uint				m_iLodIndex = {};

	vector<_wstring>	m_ModelPrototypeTags;
	vector<_uint>		m_ModelPathIndices = { };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject_Collision* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
