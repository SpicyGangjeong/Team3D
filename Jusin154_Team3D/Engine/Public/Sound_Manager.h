#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CSound_Manager final :
    public CBase
{
public:
    static  System* s_pSystem;
    static  _string s_ArrChannelName[ENUM_CLASS(SD_CHANNEL_GROUP::END)];
    static  list<Channel*> s_listChannel[ENUM_CLASS(SD_CHANNEL_GROUP::END)];
    static  Sound* s_ArrSounds[ENUM_CLASS(SOUND::SD_KIND::END)];
    static map<_wstring, SOUND::SD_KIND> s_SoundMap;

private:
    CSound_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CSound_Manager() = default;

public:
    HRESULT Update();
    void Sound_Play(SOUND::SD_KIND eSoundKind, SD_CHANNEL_GROUP eSoundChannel, _bool bRepeat, _float fVolume);
    void Sound_Play_3DPos(SOUND::SD_KIND eSoundKind, SD_CHANNEL_GROUP eSoundChannel, _float3& refSpeaker, _float fMin = 3.f, _float fMax = 20.f, _bool bRepeat = false);

    void Sound_Pause_Channel(SD_CHANNEL_GROUP eSoundChannel, bool bPause); // 채널 일시정지

    void Sound_Set3DListenerPos(class CTransform* pTransform);

    void Sound_StopChannel(SD_CHANNEL_GROUP eSoundChannel); // 특정채널 정지
    void Sound_Stop(SOUND::SD_KIND eSoundKind, SD_CHANNEL_GROUP eSoundChannel); // 특정 사운드 정지
    void Sound_StopAll(); // 전체 정지
    void Sound_SetVolume(SD_CHANNEL_GROUP eSoundChannel, float fVolume);

    HRESULT Load_Sound(SOUND::SD_KIND eKind, const _tchar* wstrSoundFilePath, FMOD_MODE eSoundMode);
    SOUND::SD_KIND Find_Sound(const _wstring wstrFilePath);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };
    bool    m_bNoSound = { false };

private:
    HRESULT Initialize();
    void Ready_SoundMap();

public:
    static CSound_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END
