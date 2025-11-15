#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpell_Slot final : public CElementObject
{
private:
	CSpell_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_Slot(const CSpell_Slot& rhs);
	virtual ~CSpell_Slot() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;


private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBuffer_UI_InstanceCom = { nullptr };

public:
	static CSpell_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
