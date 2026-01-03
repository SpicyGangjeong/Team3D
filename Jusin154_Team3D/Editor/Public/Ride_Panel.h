#pragma once

#include "Editor_Define.h"
#include "PanelObject.h"

NS_BEGIN(Editor)

class CRide_Panel final : public CPanelObject
{
private:
	CRide_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRide_Panel(const CRide_Panel& rhs);
	virtual ~CRide_Panel() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Ready_Element(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pRide_Info_Key = { nullptr };
	CGameObject* m_pRide_InfoBG = { nullptr };
	CGameObject* m_pRide_Info = { nullptr };
	CGameObject* m_pRide_Bbooster_Slot = { nullptr };
	CGameObject* m_pRide_BboosterBar = { nullptr };
	CGameObject* m_pRide_HpBar = { nullptr };
	CGameObject* m_pRide_HpSlot = { nullptr };
public:
	static CRide_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
