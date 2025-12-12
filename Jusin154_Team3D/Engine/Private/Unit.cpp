#include "pch.h"
#include "Unit.h"

#include "GameInstance.h"

CUnit::CUnit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CUnit::CUnit(const CUnit& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CUnit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUnit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Animation.resize(STATEANIM::END);

	return S_OK;
}

void CUnit::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUnit::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUnit::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

#ifdef _DEBUG
void CUnit::Load_KeyFrame()
{
	FILE* fp = nullptr;
	_string Path = {};

	for (_uint i = 0; i < m_pGameInstance->BinaryModelFilePathCount(); i++)
	{
		const _char* szCategory = m_pGameInstance->Load_BinaryModelFilePath(i);

		_char szDir[MAX_PATH] = {};
		_char szName[MAX_PATH] = {};
		_splitpath_s(szCategory, nullptr, 0, szDir, MAX_PATH, szName, MAX_PATH, nullptr, 0);

		if (strstr(CMyTools::ToString(m_strModelPrototypeTag).c_str(), szName))
		{
			Path = _string(szDir) + "KeyFrame.bin";

			fopen_s(&fp, Path.c_str(), "rb");
			if (!fp) {
				return;
			}

			_uint KeyFrameSize = 0;
			fread(&KeyFrameSize, sizeof(_uint), 1, fp);

			for (size_t i = 0; i < KeyFrameSize; i++)
			{
				float fKey = 0.f;
				fread(&fKey, sizeof(float), 1, fp);

				_uint EventLen = 0;
				fread(&EventLen, sizeof(_uint), 1, fp);

				_string str;
				str.resize(EventLen);

				fread(str.data(), sizeof(_char), EventLen, fp);
				m_KeyFrames.emplace(str, fKey);
			}

			fclose(fp);
		}
	}
}
#endif
_float CUnit::Get_KeyFrame(_string FrameName)
{
	auto iter = m_KeyFrames.find(FrameName);

	if (iter == m_KeyFrames.end())
		return 0;

	return iter->second;
}

_bool CUnit::IsCurrentKeyFrame(_string FrameName)
{
	if (m_pModelCom->Get_CurrentTrackPosition() >= Get_KeyFrame(FrameName))
	{
		return true;
	}
	return false;
}

HRESULT CUnit::Ready_Components(void *pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	/* Com_FSM */
	if (FAILED(Add_Component<CFSM>(g_iStaticLevel, &m_pFSM))){
		return E_FAIL;
	}
	 
	return S_OK;
}

void CUnit::Play_Event()
{
	for (auto iter = m_PendingEvents.begin(); iter != m_PendingEvents.end(); )
	{
		_float ratio = m_pModelCom->Get_CurrentTrackProgressRatio();
		_uint curAnim = m_pModelCom->Get_AnimIndex();

		if (curAnim == iter->AnimIndex)
		{
			if (!iter->bExecuted && ratio >= iter->fRatio)
			{
				iter->Callback();
				iter->bExecuted = true;

				if (!iter->bKeep)
				{
					iter = m_PendingEvents.erase(iter);
					continue;
				}
			}
		}
		else
		{
			iter->bExecuted = false;
		}

		++iter;
	}
}

void CUnit::Add_Event(_uint AnimIndex, function<void()> Callback, _float fRatio,_bool bKeep)
{
	PendingEvent Desc;
	Desc.AnimIndex = AnimIndex;
	Desc.fRatio = fRatio;
	Desc.Callback = Callback;
	Desc.bKeep = bKeep;
	m_PendingEvents.push_back(Desc);
}

void CUnit::Check_HitAngle(_vector ProjectileDir)
{
	_vector vProjectileDir = ProjectileDir;

	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	vLook = XMVector3Normalize(vLook);
	vProjectileDir = XMVector3Normalize(vProjectileDir);

	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vProjectileDir));
	fDot = clamp(fDot, -1.0f, 1.0f);

	_float fAngleRad = acosf(fDot);
	m_fHitDegree = XMConvertToDegrees(fAngleRad);
	_vector vCross = XMVector3Cross(vLook, vProjectileDir);
	m_fHitCross = XMVectorGetY(vCross);
}

void CUnit::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pFSM);
}
#ifdef _DEBUG

void CUnit::Describe_Entity()
{
}

#endif // _DEBUG
