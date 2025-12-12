#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Editor)

class CMapElement_Chest_Lid final : public CMapElement
{
public:
	enum class LID_STATE{IDLE, OPENING, OPENED, END};

private:
	CMapElement_Chest_Lid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Chest_Lid(const CMapElement_Chest_Lid& rhs);
	virtual ~CMapElement_Chest_Lid() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_uint iShaderPass);

private:
	LID_STATE			m_eState = { LID_STATE::END };

	_bool				m_isRotationAnim = {};
	_float				m_fOpenDuration = {};
	_float				m_fTimeAcc = {};
	_float				m_fRotationAngle = {};

	_float3				m_vTargetPosition = {};
	_float3				m_vTargetRotation = {};

	_float4x4			m_CombinedWorldMatrix = {};

#ifdef _DEBUG
	_bool				m_isOpen = {};
	_float3				m_vDefaultPos = {};
	const _float4x4*	m_pParentWorldMatrix = { nullptr };
#endif // _DEBUG


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

	_bool Opening(_float fTimeDelta);

#ifdef _DEBUG
	void Reset();
#endif // _DEBUG


public:
	static CMapElement_Chest_Lid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
	virtual HRESULT Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)override;
};

NS_END
