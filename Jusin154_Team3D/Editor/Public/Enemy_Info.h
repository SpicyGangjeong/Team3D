#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CEnemy_Info final : public CElementObject
{
private:
	CEnemy_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Info(const CEnemy_Info& rhs);
	virtual ~CEnemy_Info() = default;

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

public:
	void Set_Info(_int Level, _wstring Name);
	void Set_Font_Move(_bool Boss);
private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float2 m_fFontPos{};
	_float  m_fFont2OffSet{};
	_wstring m_pEnemy_Name;
	_wstring m_pLevel;
	_bool	m_bAnimation = {false};
	_bool   m_bBoss = { false };
public:
	static CEnemy_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
