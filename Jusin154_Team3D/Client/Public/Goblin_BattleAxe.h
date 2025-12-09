#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CGoblin_BattleAxe final : public CPartObject
{
public:
	typedef struct tagGoblinBattleAxe : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}GOBLIN_BATTLEAXE_DESC;
private:
	CGoblin_BattleAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_BattleAxe(const CGoblin_BattleAxe& Prototype);
	virtual ~CGoblin_BattleAxe() = default;

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

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGoblin_BattleAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
