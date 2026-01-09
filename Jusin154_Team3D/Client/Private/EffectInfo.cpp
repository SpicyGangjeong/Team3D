#include "pch.h"
#include "EffectInfo.h"
#include "Effect_Container.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Layer.h"
#include "Player.h"

CEffectInfo::CEffectInfo()
{
}

void CEffectInfo::Update(_float fTimeDelta)
{
	Refresh_LockOnEffects();
	Refresh_ActiveEffects();
}
void CEffectInfo::Change_Level()
{
	SAFE_RELEASE(m_pLockOnEffect);
	for (CEffect_Container* pEffect : m_ActiveEffects) {
		SAFE_RELEASE(pEffect);
	} m_ActiveEffects.clear();

}

HRESULT CEffectInfo::Regist_ActiveEffect(CEffect_Container* _pEffect)
{
	for (CEffect_Container* pEffect : m_ActiveEffects) {
		if (pEffect == _pEffect) {
			return E_ACCESSDENIED;
		}
	} m_ActiveEffects.push_back(_pEffect);
	SAFE_ADDREF(_pEffect);
	return S_OK;
}

HRESULT CEffectInfo::Deregist_ActiveEffect(CEffect_Container* _pEffect)
{
	for (list<CEffect_Container*>::iterator iter = m_ActiveEffects.begin(); iter != m_ActiveEffects.end();) {
		if (*iter == _pEffect) {
			SAFE_RELEASE(*iter);
			iter = m_ActiveEffects.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	return E_FAIL;
}

Client::CEffect_Container* CEffectInfo::Get_LockOnEffect()
{
	return m_pLockOnEffect;
}

HRESULT CEffectInfo::Refresh_LockOnEffects()
{
	SAFE_RELEASE(m_pLockOnEffect);
	_vector vCameraLook = m_pGameInstance->Get_CameraLook();
	_vector vCameraPos = m_pGameInstance->Get_CamXMPosition();
	_float fMaxDot = { 0.f };

	_vector vEffectPos;
	_vector vToEffectDir;
	_bool Aim = false;
	_float fFovy = 0.f;
	if (m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER) != nullptr)
	{
		Aim = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Get_Aim();
	}
	if (Aim)
	{
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(50.f), XMConvertToRadians(5.f));
	}
	else {
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(30.f), XMConvertToRadians(20.f));
	}

	{ 
		for (list<CEffect_Container*>::iterator iter = m_ActiveEffects.begin(); iter != m_ActiveEffects.end(); ++iter)
		{
			CEffect_Container* pEffect = (*iter);
			if (!pEffect->Get_Visible())
				continue;
			vEffectPos = pEffect->Get_WorldPostion();
			vToEffectDir = vEffectPos - vCameraPos;
			if (40.f <= XMVectorGetX(XMVector4Length(vToEffectDir))) {
				continue;
			}
			_float fDotResult = CMyTools::DirectionCompare(vCameraLook, vToEffectDir);
			if (fDotResult <= cosf(fFovy)) {
				continue;
			}

			if (nullptr == m_pLockOnEffect) {
				m_pLockOnEffect = pEffect;
				fMaxDot = fDotResult;
				continue;
			}

			if (fMaxDot < fDotResult) {
				m_pLockOnEffect = pEffect;
				fMaxDot = fDotResult;
			}


		}

	}
	if (nullptr != m_pLockOnEffect) {
		SAFE_ADDREF(m_pLockOnEffect);
	}
	return S_OK;
}

HRESULT CEffectInfo::Refresh_ActiveEffects()
{
	list<CEffect_Container*>::iterator iter = m_ActiveEffects.begin();
	for (; iter != m_ActiveEffects.end();) {
		if (true == (*iter)->isDead()) {
			SAFE_RELEASE(*iter);
			iter = m_ActiveEffects.erase(iter);
		}
		else {
			++iter;
		}
	}
	return S_OK;
}

HRESULT CEffectInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pInfoInstance = CInfoInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContex;

	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pInfoInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);

	return S_OK;
}

CEffectInfo* CEffectInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CEffectInfo* pInstance = new CEffectInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEffectInfo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLockOnEffect);
	for (CEffect_Container* pEffect : m_ActiveEffects) {
		SAFE_RELEASE(pEffect);
	} m_ActiveEffects.clear();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
