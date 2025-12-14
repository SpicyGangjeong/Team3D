
#pragma once

#include "Editor_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CQuest_Panel final : public CPanelObject
{
private:
	CQuest_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Panel(const CQuest_Panel& rhs);
	virtual ~CQuest_Panel() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Ready_Element(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pQuest_Border = { nullptr };
	CGameObject* m_pQuest_Header = { nullptr };
	CGameObject* m_pQuest_HeaderLine = { nullptr };
	CGameObject* m_pQuest_List = { nullptr };
	CGameObject* m_pQuest_Info = { nullptr };
	CGameObject* m_pQuest_Info_Header= { nullptr };
	CGameObject* m_pQuest_Info_Line = { nullptr };
	CGameObject* m_pQuest_Entry_Line = { nullptr };

public:
	static CQuest_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
