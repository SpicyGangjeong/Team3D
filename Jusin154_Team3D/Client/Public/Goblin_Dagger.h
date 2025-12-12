#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CGoblin_Dagger final : public CPartObject
{
public:
	typedef struct tagDagger : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}GOBLINDAGGER_DESC;
private:
	CGoblin_Dagger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_Dagger(const CGoblin_Dagger& Prototype);
	virtual ~CGoblin_Dagger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Attach(_bool Attach) { m_bAttach = Attach; }
public:

private:
	const _float4x4* m_pSocketMatrices = {  };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_bool m_bAttach = { true };

	_float4 m_vStartPos = {};
	PSX::PxSweepBufferN<12> m_SweepBuffer = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Dagger_Hit();

public:
	static CGoblin_Dagger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
