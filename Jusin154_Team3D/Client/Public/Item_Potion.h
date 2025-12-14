#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CItem_Potion final : public CPartObject
{
public:
	typedef struct tagPotion : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}POTION_DESC;
private:
	CItem_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Potion(const CItem_Potion& Prototype);
	virtual ~CItem_Potion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Attach(_bool Attach);
public:

private:
	const _float4x4* m_pSocketMatrices = {  };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_float4x4			 m_pWandTipMatrix = {};
	_float3				 m_Offset = {};
	_bool				 m_bAttach = { true };
	_float m_fTimer = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItem_Potion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
