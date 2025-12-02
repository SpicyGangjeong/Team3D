#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CMissionBanner_Key final : public CElementObject
{
private:
	CMissionBanner_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissionBanner_Key(const CMissionBanner_Key& rhs);
	virtual ~CMissionBanner_Key() = default;

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
	void QuestType(QUESTYPE eType);

private:
	void Compute_UV();
	void Compute_Alphabat(_tchar Alphabet);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CTexture* m_pDiffuse_TextureCom3 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float	m_fPI{};
	_bool	m_bisHoldOn = { false };
	_bool	m_bKeyHold = { false };

	_float4 m_vKeyHold{};
	_float4 m_vKey{};
	_float4 m_vUV{};
	_float4 m_vActive_Icon{};
	QUESTYPE m_eType = QUESTYPE::END;

public:
	static CMissionBanner_Key* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
