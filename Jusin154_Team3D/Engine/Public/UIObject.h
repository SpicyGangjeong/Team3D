#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	enum class SPELLTYPE {
CONTROL, POWER, DAMAGE,
UTILITY ,TRANSFORM, CURSE, ESSENTIAL
};
typedef struct tagUIObjectDesc
{
	_float fX, fY = {};
	_float fSizeX, fSizeY = {};
}UIOBJECT_DESC;

typedef struct tagSpellInfo
{
	_int			iSpell_ID{};
	_wstring		pSpell_Name;
	_wstring		pImage_Name;
	_int			iSpell_Type{};
	_int			iSkill_Type{};
	_float			fSpell_CoolTime{};
	_float			fDuration{};
	_int			iAnimNum{};
	_wstring		pSpellInfo;
	_bool			bSpell_Lock = false;
	_bool			bEquip_Spell = false;
}SPELLINFO;
typedef struct SlotHover
{
	_int iSlotID{};
	_int iHover_Index{};
}HOVER_INFO;
protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& rhs);
	virtual ~CUIObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual _vector Get_WorldPostion() override;

public:
	virtual void MoveX(_float fX);
	virtual void MoveY(_float fY);
	virtual void Move(_float fX, _float fY);
	virtual void SizeUpdate(_float fSizeX, _float fSizeY);
	virtual void SizeUpX(_float fSizeX);
	virtual void SizeUpY(_float fSizeY);
	virtual void SizeUpdate_float(_float3 fSizeXY);
	virtual void Set_Time(_float fTime);
	virtual void Set_TimeMult(_float Mult);
	virtual _float Get_EndTime();
	virtual _float Get_DelayTime();
	virtual _float Get_TimeMult();

	virtual void Visible(_bool bVisible);
	virtual _bool Get_Visible();
	virtual _bool Get_Active();
	virtual _bool Chack_Visible();

	virtual void Set_FadeIn();
	virtual void Set_FadeOut();
	virtual _bool Get_FadeIn();
	virtual _bool Get_FadeOut();
	virtual void Set_OwnerFadeIn();
	virtual void Set_OwnerFadeOut();
	virtual _bool Get_OwnerFadeIn();
	virtual _bool Get_OwnerFadeOut();
	virtual void Set_Alpha(_float fAlpha);
	virtual void Set_AlphaTime(_float fAlpha);
	virtual _float Get_Alpha();
	virtual _float Get_OwnerAlpha();
	virtual _float Get_AlphaTime();

	virtual void Set_U(_float U);
	virtual void Set_V(_float V);
	virtual _float2 Get_UV();


	virtual void Nine_Slice_Left(_float X);
	virtual void Nine_Slice_Right(_float Y);
	virtual void Nine_Slice_Top(_float Z);
	virtual void Nine_Slice_Bottom(_float W);
	virtual _float Get_Nine_Slice_Left();
	virtual _float Get_Nine_Slice_Right();
	virtual _float Get_Nine_Slice_Top();
	virtual _float Get_Nine_Slice_Bottom();

	virtual _float2 Get_Origin_Position();			// Start Position
	virtual _vector Get_Current_Position();			// Current Position
	virtual _float3 Get_Origin_Size();				// Start Size
	virtual _float3 Get_Current_Size();				// Current Size

	virtual void Lerp_PosX(_float PosX);
	virtual void Lerp_PosY(_float PosY);
	virtual void Start_Lerp(_float fTimeDelta);
	virtual void Reset_Pos(_float fTimeDelta);
	virtual void Start_Size_Lerp(_float fTimeDelta);
	virtual void Reset_Size_Lerp(_float fTimeDelta);
	virtual _vector Get_Lerp_Pos();
	virtual void LerpOn();
	virtual void LerpOff();
	virtual _bool Get_LerpOn();
	virtual _bool Get_LerpOff();

	virtual _float MovDir(_float Start, _float End, _float Speed);
	virtual void Set_Speed(_float Speed);
	virtual _float Get_Speed();

	virtual void Set_Angle(_float fAngle);
	virtual _float Get_Angle();

	virtual void Set_SkillType(_int eType);
	virtual _int Get_SkillType();
	virtual void Set_SpellType(_int eType);
	virtual _int Get_SpellType();

	virtual void Set_CoolTime(_float CoolTime);
	virtual _float Get_CoolTime();

	virtual void Add_Function(wstring Name, function<void(void*)> Evnet);
	virtual void Function_Callback(wstring Name, void* pArg = nullptr);

	virtual _bool Get_Hover();

	virtual const SPELLINFO Get_Info(_int Index);
protected:
	_vector					m_fOrigin_Position_vector{};
	_float2					m_fOrigin_Position{};
	_vector					m_fCurrent_Position{};
	_vector					m_vLerp_Position{};
	_float					m_fLerpX{};
	_float					m_fLerpY{};
	_float3					m_fOrigin_Size{};

protected:
	_float4x4				m_ViewMatrix{};							// 직교를 하기 위해서 필요
	_float4x4				m_ProjMatrix{};
	_float					m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float					m_fWinSizeX{}, m_fWinSizeY{};

	_bool					m_bActive = { false };					// UI들이 켜질지 꺼질지
	_bool					m_bHover = { false };					// UI들 위에 마우스가 올라가 있는지 확인
	_bool					m_bFadeIn = { false };
	_bool					m_bFadeOut = { false };
	_bool					m_bOwnerFadeIn = { false };
	_bool					m_bOwnerFadeOut = { false };
	_bool					m_bLerpOn = { false };
	_bool					m_bLerpOff = { false };
	_bool					m_bEventOn = { false };

	_float					m_fAlpha{};								// UI의 알파값을 조절해서 서서히 나오거나 서서히 사라지게 하기 위함
	_float					m_fOwnerAlpha{};						// 부모의 알파값인데 부모가 어두워지면 자식들도 어두워 져야해서 설정을 해준다.
	_float					m_fCanvasAlpha{};						// 캔버스의 알파를 던지기 위해
	_float					m_fAlphaTime{};							// UI의 알파값을 조절 할 때 알파는 0~1이라서 n을 나누거나 곱해서 시간을 늘리거나 빠르게 하기 위함
	_float					m_fTime{};								// 캔버스는 아니더라도 패널전체랑 Element들은 필요함
	_float					m_fCoolTime{};


	_float					m_fEndTime{};							// UI의 움직임이 끝나는 시간
	_float					m_fDelayTime{};							// UI의 딜레이 시간
	_float					m_fTimeMult{1.f};						// 애니메이션 속도를 조절하기 위해서 필요
	_float3					m_vScale{};								// UI의 스케일을 조절하기 위해서 필요함

	_float2					m_vUVScale{};
	_float					m_fAngle{};

	RECT					m_pRect{};

	_int					m_iSpellType{};
	_int					m_iSkillType{};
	_float					m_fSortZ{};
	_float					m_fMoveSpeed{};

	_float4					m_vNine_Slice{};

	_float					m_fTopY{};

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Ready_Components(void* pArg) override;
public:
	virtual void Free() override;
};


NS_END
