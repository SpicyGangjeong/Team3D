#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CEnemy_Detection final : public CElementObject
{
private:
	CEnemy_Detection(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Detection(const CEnemy_Detection& rhs);
	virtual ~CEnemy_Detection() = default;

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
	void SizeUpdate(_float fSizeX);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CTexture* m_pDiffuse_TextureCom3 = { nullptr };
	CTexture* m_pDiffuse_TextureCom4 = { nullptr };
	CTexture* m_pDiffuse_TextureCom5 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float4 m_vImageposi{};

	_bool m_bStep1 = { false };
	_bool m_bStep2 = { false };
	_bool m_bStep3 = { false };
public:
	static CEnemy_Detection* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
