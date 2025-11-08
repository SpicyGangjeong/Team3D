#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CDummy_Goblin_Head final : public CGameObject
{
private:
	CDummy_Goblin_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_Goblin_Head(const CDummy_Goblin_Head& Prototype);
	virtual ~CDummy_Goblin_Head() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_bool		m_bRimLight = { true };
	_float		m_fRimLightPower = { 3.2f };
	_float		m_fRimLightStrength = { 3.04f };
	_float3		m_vRimLightColor = { 69.f / 255.f, 5.f / 255.f, 10.f / 255.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDummy_Goblin_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END