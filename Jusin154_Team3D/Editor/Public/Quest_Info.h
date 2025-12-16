#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CQuest_Info final : public CElementObject
{
private:
	CQuest_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Info(const CQuest_Info& rhs);
	virtual ~CQuest_Info() = default;

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
	void Set_Hover(_int Index);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_wstring m_pQuest_Info;

	_float	m_fOriginPerviewSize{};
	_float	m_fPreviewOffSet{};
	_float4 m_vHeaderBack{};
	_int	m_iQuest_Index{};

	_int	m_iPerQuestIndex{};
public:
	static CQuest_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
