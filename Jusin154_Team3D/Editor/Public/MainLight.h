#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Editor)

class CMainLight final : public CGameObject
{
public:
	enum class DAY_PHASE { DAWN, DAY, DUSK, NIGHT, END };

private:
	CMainLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMainLight(const CMainLight& Prototype);
	virtual ~CMainLight() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CLight*				m_pLightCom = { nullptr };
	class CGameTime*	m_pGameTime = { nullptr };
	_float4				m_vCurDiffuse = {};
	_float4				m_vAmbient = {};
	_float4				m_vSpecular = {};

	DAY_PHASE			m_eCurPhase = { DAY_PHASE::END};
	DAY_PHASE			m_eNextPhase = { DAY_PHASE::END };

	_float4				m_Diffuse_Colors[ENUM_CLASS(DAY_PHASE::END)] = {};
	_float4				m_Ambient_Colors[ENUM_CLASS(DAY_PHASE::END)] = {};
	_float4				m_Specular_Colors[ENUM_CLASS(DAY_PHASE::END)] = {};


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMainLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
