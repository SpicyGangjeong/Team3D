#pragma once

#include "Client_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Client)

class CRaceRing final : public CContainerObject
{
public:
	typedef struct RaceRingDesc
	{
		class CBroomRaceManager* pBroomRaceManager = { nullptr };
		_float3 vPosition{};
		_float3 vRotation{};
		_float3 vScale{};
	}RACERING_DESC;
private:
	CRaceRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaceRing(const CRaceRing& Prototype);
	virtual ~CRaceRing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Target(_bool bTarget){ m_bTarget = bTarget; }

private:
	_bool	World_Screen(_float3 fWorld_Pos, _float2& fScreenPos, _float& fDepth);
	_bool	Screen(_float2& fWorld_Pos, _float Winx, _float Winy);
	_float2 Get_EdgePosition(_float2& fWorld_Pos, _float Winx, _float Winy);
	void	World_to_ScreenUI();

private:
	class CBroomRaceManager*	m_pBroomRaceManager = { nullptr };
	class CEffectParts* m_pRingEffect = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };
	_bool						m_bTarget = { false };
	_int						m_iIndex = {};

	_bool						m_bShowOffScreenUI = { false };
	_float2						m_fScreenPos{};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Parts();

public:
	static CRaceRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
