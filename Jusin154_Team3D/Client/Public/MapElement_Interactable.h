#pragma once

#include "Client_Define.h"
#include "MapElement.h"

NS_BEGIN(Client)

class CMapElement_Interactable final : public CMapElement
{
public:
	typedef struct tagElement_Interactable_Desc : MAPELEMENT_DESC
	{
		_uint iInteractableID = {};
		_uint iSubKind = {};
		_float3 vBoxLocalPosition;
		_float3 vBoxSize;
	}ELEMENT_INTERACTABLE_DESC;

private:
	CMapElement_Interactable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Interactable(const CMapElement_Interactable& rhs);
	virtual ~CMapElement_Interactable() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void		GrapToPlayer(_fvector vPos, _float fRatio);
	_vector				Get_LockOnPos();
	virtual void		Set_DrawOutLine();
	virtual HRESULT		Render_OutLine() override;
	void				Set_KinematicFlag(_bool bFlag);

private:
	ELEMENT_INTERACTABLE_ID			m_eInteractableID = { ELEMENT_INTERACTABLE_ID::END };
	class CInfoInstance*			m_pInfoInstance = { nullptr };

	class CRigidBody_Dynamic*		m_pRigidBody = { nullptr };
	PSX::PxRigidDynamic*			m_pActor = { nullptr };
	_bool							m_bDrawOutLine = { false };
	_float4							m_vOutLineColor = CMyTools::ColorRGBA_HEXtoFLOAT4(0xfefefe00);
	_float							m_fOutLineThickness = { 5.f };
	_float							m_fOutLineScale = { 1.f };
	_float							m_fOutLinePower = { 1.f };


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CMapElement_Interactable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
