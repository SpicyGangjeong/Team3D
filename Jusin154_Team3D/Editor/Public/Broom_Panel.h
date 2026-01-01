#pragma once

#include "Editor_Define.h"
#include "PanelObject.h"

NS_BEGIN(Editor)

class CBroom_Panel final : public CPanelObject
{
private:
	CBroom_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom_Panel(const CBroom_Panel& rhs);
	virtual ~CBroom_Panel() = default;

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

	CGameObject* m_pBroom_Flag = { nullptr };
	CGameObject* m_pBroom_Circle = { nullptr };
	CGameObject* m_pBroom_Scoreboard = { nullptr };
	CGameObject* m_pBroom_Finish = { nullptr };
	CGameObject* m_pBroom_Record = { nullptr };
	CGameObject* m_pBroom_Exit = { nullptr };
	CGameObject* m_pBroom_Trophy = { nullptr };

public:
	static CBroom_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
