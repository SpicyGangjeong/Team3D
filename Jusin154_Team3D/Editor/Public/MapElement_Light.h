#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Editor)

class CMapElement_Light : public CMapElement
{
private:
	CMapElement_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Light(const CMapElement_Light& rhs);
	virtual ~CMapElement_Light() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CLight*		m_pLightCom = { nullptr };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CMapElement_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
