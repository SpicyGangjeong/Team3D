#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CTroll_Weapon final : public CPartObject
{
public:
	typedef struct tagTrollWeapon : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}TROLLWEAPON_DESC;
private:
	CTroll_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTroll_Weapon(const CTroll_Weapon& Prototype);
	virtual ~CTroll_Weapon() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	virtual _vector Get_WorldPostion() override;
	_matrix			Get_WorldMatrix();
	virtual const _float4* Get_HammerPosition();

private:
	const _float4x4* m_pSocketMatrices = {  };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_float4x4				m_HammerMatrix = { };
	_float3					m_Offset = {};
#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pGripShape = { nullptr };
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
#endif // _DEBUG

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTroll_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
