#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CMissionBanner_Border final : public CElementObject
{
private:
	CMissionBanner_Border(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissionBanner_Border(const CMissionBanner_Border& rhs);
	virtual ~CMissionBanner_Border() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

	virtual void SizeUpX(_float fSizeX) override;
	virtual void Lerp_PosY(_float fSizeY) override;
	void Size_LerpOn(_float LerpX, _float fTimeDelta);
	void Size_LerpOff(_float fTimeDelta);

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pMission_Icon = { nullptr };

	_float 	m_fLerpTime{};

public:
	static CMissionBanner_Border* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
