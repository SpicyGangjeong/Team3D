#pragma once

#include "Client_Define.h"
#include "MapElement.h"

NS_BEGIN(Client)

class CMapElement_Static : public CMapElement
{
private:
	CMapElement_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Static(const CMapElement_Static& rhs);
	virtual ~CMapElement_Static() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool		m_bReadyToCreatePhysX = { false };
	_bool		m_bConverted = { false };
	_float		m_fRadius = {};

	vector<vector<CRigidBody_Static*>> m_RigidBodies;
	
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

	void			ReadyForPhysX();
	void			ConvertToPhysX();

public:
	static CMapElement_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
