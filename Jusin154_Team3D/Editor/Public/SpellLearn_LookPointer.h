#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpellLearn_LookPointer final : public CElementObject
{
private:
	CSpellLearn_LookPointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_LookPointer(const CSpellLearn_LookPointer& rhs);
	virtual ~CSpellLearn_LookPointer() = default;

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
	void Set_SpellLearn(_int Index);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


	_bool	m_bMoveStart = { false };

	_int	m_iLineIndex{};
	_int	m_iCurrentLine{};
	vector<_float4> m_MoveLine;
	_float m_localX{};
	_float m_localY{};
public:
	static CSpellLearn_LookPointer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
