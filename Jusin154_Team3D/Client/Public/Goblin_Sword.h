#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CGoblin_Sword final : public CPartObject
{
public:
	typedef struct tagSword : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}GOBLIN_SWORD_DESC;
private:
	CGoblin_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_Sword(const CGoblin_Sword& Prototype);
	virtual ~CGoblin_Sword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:

private:
	const _float4x4* m_pSocketMatrices = {  };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_float4 m_vStartPos = {};
	PSX::PxSweepBufferN<12> m_SweepBuffer = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Sword_Hit();

public:
	static CGoblin_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
