#include "pch.h"
#include "Sound_Manager.h"
#include "Transform.h"
#include "GameInstance.h"

System* CSound_Manager::s_pSystem = { nullptr };
_string CSound_Manager::s_ArrChannelName[ENUM_CLASS(SD_CHANNEL_GROUP::END)] = { "BGM", "ENVIRONMENT", "EFFECT", "VOICE" };
list<Channel*> CSound_Manager::s_listChannel[ENUM_CLASS(SD_CHANNEL_GROUP::END)] = {};
Sound* CSound_Manager::s_ArrSounds[ENUM_CLASS(SOUND::SD_KIND::END)] = {};


CSound_Manager::CSound_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
    SAFE_ADDREF(m_pDevice);
    SAFE_ADDREF(m_pGameInstance);
    SAFE_ADDREF(m_pContext);
}

HRESULT CSound_Manager::Update()
{
    s_pSystem->update();
    for (int i = 0; i < ENUM_CLASS(SD_CHANNEL_GROUP::END); ++i)
    {
        auto& channelList = s_listChannel[i];
        for (auto iter = channelList.begin(); iter != channelList.end(); )
        {
            _bool isPlaying = false;
            if (*iter && FMOD_OK == (*iter)->isPlaying(&isPlaying) && !isPlaying)
            {
                iter = channelList.erase(iter); // 재생 끝났으면 삭제
            }
            else
            {
                ++iter;
            }
        }
    }
    Sound_Test();

    return S_OK;
}

void CSound_Manager::Sound_Play(SOUND::SD_KIND eSoundKind, SD_CHANNEL_GROUP eSoundChannel, _bool bRepeat, _float fVolume)
{
    _uint iIndexSound = ENUM_CLASS(eSoundKind);
    _uint iIndexGroup = ENUM_CLASS(eSoundChannel);
    Sound* pSound = s_ArrSounds[iIndexSound];

    auto& channelList = s_listChannel[iIndexGroup];

    // 이미 끝난 채널 정리
    for (auto iter = channelList.begin(); iter != channelList.end(); )
    {
        Channel* pChannel = *iter;
        bool bIsPlaying = false;
        if (!pChannel || pChannel->isPlaying(&bIsPlaying) != FMOD_OK || !bIsPlaying) {
            iter = channelList.erase(iter);
        }
        else {
            ++iter;
        }
    }

    Channel* pChannel = nullptr;
    FMOD_RESULT fRersult = s_pSystem->playSound(pSound, nullptr, true, &pChannel);
    if (fRersult != FMOD_OK || nullptr == pChannel) {
        return;
    }

    if (true == bRepeat)
    {
        pChannel->setMode(FMOD_LOOP_NORMAL);
        pChannel->setLoopCount(-1);
    }
    else
    {
        pChannel->setMode(FMOD_LOOP_OFF);
    }


    pChannel->setVolume(fVolume);

    pChannel->setPaused(false);
    channelList.push_back(pChannel);
}

void CSound_Manager::Sound_Play_3DPos(SOUND::SD_KIND eSoundKind, SD_CHANNEL_GROUP eSoundChannel, _float3& refSpeaker, _float fMin, _float fMax, _bool bRepeat)
{
    auto& channelList = s_listChannel[ENUM_CLASS(eSoundChannel)];

    // 중복 재생 방지 (같은 Sound가 재생 중인지 확인)

    if (!bRepeat)
    {
        for (auto& pChannel : channelList)
        {
            Sound* pCurrentSound = nullptr;
            if (pChannel && FMOD_OK == pChannel->getCurrentSound(&pCurrentSound))
            {
                if (pCurrentSound == s_ArrSounds[ENUM_CLASS(eSoundKind)])
                    return; // 이미 같은 사운드 재생 중이면 무시
            }
        }
    }

    Channel* pChannel = nullptr;
    s_pSystem->playSound(s_ArrSounds[ENUM_CLASS(eSoundKind)], nullptr, true, &pChannel);

    if (pChannel)
    {
        FMOD_VECTOR soundPos = { refSpeaker.x, refSpeaker.y, refSpeaker.z };
        FMOD_VECTOR soundVel = { 0.0f, 0.0f, 0.0f };
        pChannel->set3DAttributes(&soundPos, &soundVel);
        pChannel->set3DMinMaxDistance(fMin, fMax);
        pChannel->setPaused(false);

        s_listChannel[ENUM_CLASS(eSoundChannel)].push_back(pChannel);
    }
}

void CSound_Manager::Sound_Pause_Channel(SD_CHANNEL_GROUP eSoundChannel, bool bPause)
{
    auto& channelList = s_listChannel[ENUM_CLASS(eSoundChannel)];
    for (auto& pChannel : channelList)
    {
        if (pChannel) {
            pChannel->setPaused(bPause);
        }
    }
}

void CSound_Manager::Sound_Set3DListenerPos(CTransform* pTransform)
{
    _float4 vListener = {};
    _vector vTemp = pTransform->Get_State(STATE::POSITION);
    memcpy_s(&vListener, sizeof(_float4), &vTemp, sizeof(_vector));
    FMOD_VECTOR listenerPos = { vListener.x, vListener.y, vListener.z };
    FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
    vTemp = pTransform->Get_State(STATE::LOOK);
    memcpy_s(&vListener, sizeof(_float4), &vTemp, sizeof(_vector));
    FMOD_VECTOR forward = { vListener.x, vListener.y, vListener.z };
    vTemp = pTransform->Get_State(STATE::UP);
    memcpy_s(&vListener, sizeof(_float4), &vTemp, sizeof(_vector));
    FMOD_VECTOR up = { vListener.x, vListener.y, vListener.z };
    s_pSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);

}

void CSound_Manager::Sound_StopChannel(SD_CHANNEL_GROUP eSoundChannel)
{
    auto& channelList = s_listChannel[ENUM_CLASS(eSoundChannel)];
    for (auto& pChannel : channelList)
    {
        if (pChannel) {
            pChannel->stop();
        }
    } channelList.clear(); // 리스트 초기화
}

void CSound_Manager::Sound_Stop(SOUND::SD_KIND eSoundKind, SD_CHANNEL_GROUP eSoundChannel)
{
    auto& channelList = s_listChannel[ENUM_CLASS(eSoundChannel)];

    for (auto it = channelList.begin(); it != channelList.end(); )
    {
        Sound* pCurrentSound = nullptr;
        if (*it && FMOD_OK == (*it)->getCurrentSound(&pCurrentSound))
        {
            if (pCurrentSound == s_ArrSounds[ENUM_CLASS(eSoundKind)])
            {
                (*it)->stop();
                it = channelList.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void CSound_Manager::Sound_StopAll()
{
    for (int i = 0; i < ENUM_CLASS(SD_CHANNEL_GROUP::END); ++i) {
        auto& channelList = s_listChannel[i];
        for (auto& pChannel : channelList)
        {
            if (pChannel) {
                pChannel->stop();
            }
        }
        channelList.clear(); // 리스트 초기화
    }
}

void CSound_Manager::Sound_SetVolume(SD_CHANNEL_GROUP eSoundChannel, float fVolume)
{
    auto& channelList = s_listChannel[ENUM_CLASS(eSoundChannel)];
    for (auto& pChannel : channelList)
    {
        if (pChannel)
            pChannel->setVolume(fVolume); // 0.0 ~ 1.0
    }
}

void CSound_Manager::Sound_Test()
{
    //GUI::Begin("Music_Player");
    //static int item_selected_CHANNEL = 0;
    //static int item_selected_SD_KIND = 0;

    //GUI::BeginChild("##ITEMs", ImVec2(350.f, 175.f), true, ImGuiWindowFlags_HorizontalScrollbar);
    //for (int iIndex = 0; iIndex < SOUND::SD_KIND::END; ++iIndex) {
    //    if (GUI::Button(CMyTools::ToString(SOUND::SD_PATH::SD_KIND_STRING[iIndex]).c_str())) {
    //        item_selected_SD_KIND = iIndex;
    //    }
    //}
    //GUI::EndChild();
    //GUI::BeginChild("##Channels", ImVec2(350.f, 175.f), true, ImGuiWindowFlags_HorizontalScrollbar);
    //for (int iIndex = 0; iIndex < ENUM_CLASS(SD_CHANNEL_GROUP::END); ++iIndex) {
    //    if (GUI::Button(s_ArrChannelName[iIndex].c_str())) {
    //        item_selected_CHANNEL = iIndex;
    //    }
    //}
    //GUI::EndChild();

    //if (GUI::Button("Play_Sound", { 50, 50 })) {
    //    Sound_Play(static_cast<SOUND::SD_KIND>(item_selected_SD_KIND), static_cast<SD_CHANNEL_GROUP>(item_selected_CHANNEL), false, false, 1.f);
    //} GUI::SameLine();
    //if (GUI::Button("Sound_STOP", { 50, 50 })) {
    //    Sound_StopAll();
    //}
    //GUI::End();
}

HRESULT CSound_Manager::Load_Sound(SOUND::SD_KIND eKind, const _tchar* wstrSoundFilePath, FMOD_MODE eSoundMode)
{
    //const _tchar* pSoundFilePath = TEXT("../Bin/Resources/Sounds/");
    //for (int iIndex = 0; iIndex < ENUM_CLASS(SOUND::SD_KIND::END); ++iIndex) {
    //    if (FAILED(m_pGameInstance->Load_Sound((SOUND::SD_KIND)iIndex, (_wstring(pSoundFilePath) + SOUND::SD_PATH::SD_KIND_STRING[iIndex]).c_str()))) {
    //        return E_FAIL;
    //    }
    //}

    //// 사운드 생성, 파일 불러오기
    FMOD_RESULT fr = CSound_Manager::s_pSystem->createSound(
        CMyTools::ToString(wstrSoundFilePath).c_str(), // 대상 경로
        eSoundMode, // 재생 모드
        0,          // 인덱스?
        &CSound_Manager::s_ArrSounds[ENUM_CLASS(eKind)]);   // 대상 사운드
    if (fr == 0) {
        return S_OK;
    }
    return E_FAIL;
}

HRESULT CSound_Manager::Initialize()
{
    FMOD_RESULT r = System_Create(&s_pSystem);
    if (r != FMOD_OK) return E_FAIL;

    int numDrivers = 0;
    s_pSystem->getNumDrivers(&numDrivers);

    if (numDrivers == 0) {
        m_bNoSound = true;
        s_pSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
    }

    r = s_pSystem->init(512, FMOD_INIT_NORMAL, nullptr);
    if (r != FMOD_OK) {
        m_bNoSound = true;
        s_pSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        s_pSystem->init(512, FMOD_INIT_NORMAL, nullptr);
    }

    return S_OK;
}

CSound_Manager* CSound_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSound_Manager* pInstance = new CSound_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CSound_Manager");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

void CSound_Manager::Free()
{
    __super::Free();
    Sound_StopAll();
    for (int i = 0; i < ENUM_CLASS(SOUND::SD_KIND::END); ++i) {
        if (s_ArrSounds[i]) {
            s_ArrSounds[i]->release();
            s_ArrSounds[i] = nullptr;
        }
    }
    // System 해제
    if (s_pSystem) {
        if (!m_bNoSound) {
            s_pSystem->close();
            s_pSystem->release();
        }
        else {
            // 무음 모드면 그냥 close/release 없이 바로 포인터만 비움
        }
        s_pSystem = nullptr;
    }

    SAFE_RELEASE(m_pGameInstance);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pContext);
}
