#pragma once

#include "Client_Define.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUI_Manager final : public CUIObject
{
private:
	CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Manager(const CUI_Manager& rhs);
	virtual ~CUI_Manager() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	//virtual HRESULT	Ready_Panel(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Clear_Penel();

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pGamePlay_Canves= { nullptr };
	CGameObject* m_pSpell_Panel = { nullptr };
	_bool			m_bCanvas_Change = { false };

public:
	static CUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END;
