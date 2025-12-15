#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CCurrent_Slot_Number final : public CElementObject
{
private:
	CCurrent_Slot_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCurrent_Slot_Number(const CCurrent_Slot_Number& rhs);
	virtual ~CCurrent_Slot_Number() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

	virtual void SizeUpX(_float fSizeX) override;
	virtual void SizeUpY(_float fSizeY) override;
	virtual void SizeUpdate(_float fSizeX, _float fSizeY) override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	void	UV();
	_float4 Meter_Index(_uint Number);
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBufferCom = { nullptr };
	CVIBuffer_UI_Instance::UI_ATLAS_DESC	pUVDesc[4];

	_float2									m_fIamge_Size{};
	_float									m_fOffSetX{};
	_float									m_fOffSetY{};
	_uint									m_iCols{};

public:
	static CCurrent_Slot_Number* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
