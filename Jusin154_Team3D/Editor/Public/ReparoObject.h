#pragma once

#include "Editor_Define.h"
#include "Monster.h"

NS_BEGIN(Editor)

class CReparoObject final : public CMonster
{
private:
	CReparoObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CReparoObject(const CReparoObject& Prototype);
	virtual ~CReparoObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float4			m_vOutLineColor = CMyTools::ColorRGBA_HEXtoFLOAT4(0xfefefe00);
	_float			m_fOutLineThickness = { 5.f };
	_float			m_fOutLineScale = { 2.f };
	_float			m_fOutLinePower = { 2.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Render_OutLine();

public:
	static CReparoObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
