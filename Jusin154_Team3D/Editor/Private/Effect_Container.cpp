#include "pch.h"
#include "Effect_Container.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "TrailObject.h"


#include <sstream>

CEffect_Container::CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CEffect_Container::CEffect_Container(const CEffect_Container& rhs)
	: CContainerObject(rhs)
{

}

HRESULT CEffect_Container::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CEffect_Container::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Container::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CEffect_Container::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CEffect_Container::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CEffect_Container::OnCollision(CGameObject* pOther, void* pDesc)
{
}

HRESULT CEffect_Container::Load_Directory(const _char* pPath)
{

	for (const auto& file : filesystem::directory_iterator(pPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (!(strcmp(ext.c_str(), ".bin") ^ strcmp(ext.c_str(), ".trail")))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		/* Trail */

		if (!strcmp(ext.c_str(), ".trail"))
		{
			CTrailObject* pTrail = nullptr;

			CPartObject::PARTOBJECT_DESC PartsDesc{};

			PartsDesc.pParentTransform = m_pTransformCom;


			_wstring wstrFileName = file.path().stem().wstring(); //이름 넣기

			if (FAILED(Add_PartObject<CTrailObject>(CMyTools::ToString(wstrFileName), NEXT_LEVEL, &pTrail, &PartsDesc))) {
				assert(false);
				return E_FAIL;
			}


			/*이펙트 로드 로드*/
			size_t iPos = {};
			_string strFilePath = szFilePath;

			while ((iPos = strFilePath.find(".trail")) != std::string::npos) {
				strFilePath.erase(iPos, 6);
			};


			if (FAILED(pTrail->Load_Trail(strFilePath.c_str(), LEVEL::EFFECT)))
			{
				Safe_Release(pTrail);
				continue;
			}

			Safe_Release(pTrail);

			continue;
		}


		/////////////////////////////

		CEditEffect* pEditEffect = nullptr;

		CPartObject::PARTOBJECT_DESC PartsDesc{};

		PartsDesc.pParentTransform = m_pTransformCom;




		/*이펙트 껍데기 생성*/

		_wstring wstrFileName = file.path().stem().wstring(); //이름 넣기

		if (FAILED(Add_PartObject<CEditEffect>(CMyTools::ToString(wstrFileName), NEXT_LEVEL, &pEditEffect, &PartsDesc))) {
			assert(false);
			return E_FAIL;
		}
		

		/*이펙트 로드 로드*/
		size_t iPos = {};
		_string strFilePath = szFilePath;

		while ((iPos = strFilePath.find(".bin")) != std::string::npos) {
			strFilePath.erase(iPos, 4);
		};


		if (FAILED(pEditEffect->Load(strFilePath.c_str(), LEVEL::EFFECT)))
		{
			Safe_Release(pEditEffect);
			continue;
		}

		Safe_Release(pEditEffect);
	}

	return S_OK;
}

HRESULT CEffect_Container::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC TransformDesc = {};

	TransformDesc.fRadius = 20.f;
	TransformDesc.fRotationPerSec = 10.f;
	TransformDesc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Ready_Components(&TransformDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Container::Ready_Child()
{

	return S_OK;
}

void CEffect_Container::Free()
{
	__super::Free();

}

HRESULT CEffect_Container::Bind_ShaderResources()
{
	return S_OK;
}

void CEffect_Container::Update_Event(_float fTimeDelta)
{

	m_fPreAccTime = m_fAccTime;
	m_fAccTime += fTimeDelta;

	if (m_fAccTime >= m_fDelay || m_isDelayed == false) // 아직 딜레이되지 않았고 딜레이를 넘어섰다면
	{
		m_isDelayed = true;
	}
	else
	{
		return;
	}


	if (m_fAccTime >= m_fDuration) // 듀레이션을 넘어섰다면
	{
		if (m_isLoop)
		{
			m_fAccTime = 0.f;
			m_fPreAccTime = 0.f;
			m_isDelayed = false;
		}
		else
		{
			m_bVisible = false;
		}
	}


	for (auto& pPair : m_Events) // 이벤트들을 비교하면서 실행함
	{
		if (pPair.first >= m_fPreAccTime && pPair.first <= m_fAccTime)
		{
			pPair.second();
		}
	}
	

}
