#pragma once

#include "Editor_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CSpellLearn_Panel final : public CPanelObject
{

private:
	CSpellLearn_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_Panel(const CSpellLearn_Panel& rhs);
	virtual ~CSpellLearn_Panel() = default;

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

public:
	virtual const SPELLLEARNINFO Get_Learninfo(_int Index) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	vector<SPELLLEARNINFO>   m_Info = {};

	CGameObject* m_pSpellLearn_Name = { nullptr };
	CGameObject* m_pSpellLearn = { nullptr };
	CGameObject* m_pSpellLearn_MovePointer = { nullptr };
	CGameObject* m_pSPellLeam_ChaserPointer = { nullptr };
	CGameObject* m_pSpellLearn_Data = { nullptr };

	_int Index{};

public:
	static CSpellLearn_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
