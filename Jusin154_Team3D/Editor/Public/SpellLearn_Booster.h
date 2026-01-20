#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpellLearn_Booster final : public CElementObject
{
private:
	struct Booster
	{
		_bool bBoosterOn = { false };
		_int iBoosterIndex{};
	};
private:
	CSpellLearn_Booster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_Booster(const CSpellLearn_Booster& rhs);
	virtual ~CSpellLearn_Booster() = default;

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
	void BoosterOn(void* pArg);
	void BoosterOff(void* pArg);

	void Set_Index(_int iIndex);
	void Reset();

private:
	void Clear();

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CTexture* m_pDiffuse_TextureCom3 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_bool	m_bBoosterOn = { false };
	_bool	m_bBoosterOff = { false };
	_float4 m_fImagePosi{};
	_int	m_iIndex{};
public:
	static CSpellLearn_Booster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
