#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CTroll_Rock final : public CPartObject
{
public:
	typedef struct tagTrollRock : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}TROLLROCK_DESC;
private:
	CTroll_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTroll_Rock(const CTroll_Rock& Prototype);
	virtual ~CTroll_Rock() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void RockHit();
	void Set_Attach(_bool Attach);
public:

private:
	const _float4x4* m_pSocketMatrices = {  };
	_float4 m_vStartPos = {};
	PSX::PxSweepBufferN<12> m_SweepBuffer = {};
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_bool m_bAttach = { true };

	class CEffectPool* m_pEffectPool = { nullptr };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTroll_Rock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
