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

const _float4x4* CUnit::Get_SocketMatrixPtr(const _char* pSocketName)
{
    return m_pModelCom->Get_BoneMatrixPtr(pSocketName);
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

_wstring CUnit::Get_Name()
{
    return _wstring();
}

_wstring CUnit::Get_NpcName()
{
    return _wstring();
}

_int CUnit::Get_TextID()
{
    return _int();
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

        _float prevRatio = iter->PrevRatio;

        if (curAnim == iter->AnimIndex)
        {
            if (prevRatio <= iter->fRatio && ratio >= iter->fRatio)
            {
                iter->Callback();

                if (!iter->bKeep)
                {
                    iter = m_PendingEvents.erase(iter);
                    continue;
                }
            }

            iter->PrevRatio = ratio;
        }
        else
        {
            iter->PrevRatio = 0.f;
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

void CUnit::Reset_Event()
{
    m_PendingEvents.clear();
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

void CUnit::Load_AnimXML(const string& path)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path.c_str());

    tinyxml2::XMLElement* root = doc.FirstChildElement("Animations");
    for (tinyxml2::XMLElement* elem = root->FirstChildElement("Anim");
        elem; elem = elem->NextSiblingElement("Anim"))
    {
        const char* stateStr = elem->Attribute("state");
        _int index = elem->IntAttribute("index");
        _bool loop = elem->BoolAttribute("loop");

        STATEANIM::ESTATE state = StringToStateAnim(stateStr);
        m_Animation[state] = { index, loop };
    }
}

STATEANIM::ESTATE CUnit::StringToStateAnim(const string& s)
{
    USING(STATEANIM);

    static const unordered_map<string, ESTATE> table =
    {
        // IDLE
        { "IDLE", IDLE }, { "IDLE_G", IDLE_G }, { "IDLE_AIM", IDLE_AIM },
        { "IDLE_TURN_L", IDLE_TURN_L }, { "IDLE_TURN_R", IDLE_TURN_R },
        { "IDLE_TURN_BWD", IDLE_TURN_BWD },
        { "IDLE_COMBAT_TURN_BWD_L", IDLE_COMBAT_TURN_BWD_L },
        { "IDLE_COMBAT_TURN_BWD_R", IDLE_COMBAT_TURN_BWD_R },
        { "IDLE_COMBAT_TURN_90_L", IDLE_COMBAT_TURN_90_L },
        { "IDLE_COMBAT_TURN_90_R", IDLE_COMBAT_TURN_90_R },
        { "IDLE_BREAK1", IDLE_BREAK1 }, { "IDLE_BREAK2", IDLE_BREAK2 },
        { "IDLE_BREAK3", IDLE_BREAK3 }, { "IDLE_BREAK4", IDLE_BREAK4 },
        { "IDLE_BREAK5", IDLE_BREAK5 }, { "IDLE_BREAK6", IDLE_BREAK6 },
        { "IDLE_BREAK7", IDLE_BREAK7 },
         
        { "IDLE_TURN_180_L", IDLE_TURN_180_L }, { "IDLE_TURN_180_R", IDLE_TURN_180_R },
        { "IDLE_TURN_45_L", IDLE_TURN_45_L },  { "IDLE_TURN_45_R", IDLE_TURN_45_R },
        { "IDLE_TURN_90_L", IDLE_TURN_90_L },  { "IDLE_TURN_90_R", IDLE_TURN_90_R },


        // MOVE
        { "MOVE", MOVE },

        // WALK
        { "WALK", WALK },
        { "WALK_FWD", WALK_FWD }, { "WALK_RIGHT", WALK_RIGHT },
        { "WALK_LEFT", WALK_LEFT }, { "WALK_BWD", WALK_BWD },
        { "WALK_START", WALK_START },
        { "WALK_STOP", WALK_STOP }, { "WALK_STOP_L", WALK_STOP_L },
        { "WALK_STOP_R", WALK_STOP_R },

        // JOG
        { "JOG", JOG }, { "JOG_START", JOG_START },
        { "JOG_FWD", JOG_FWD }, { "JOG_RIGHT", JOG_RIGHT },
        { "JOG_LEFT", JOG_LEFT }, { "JOG_BWD", JOG_BWD },
        { "JOG_STOP", JOG_STOP },
        { "JOG_AIM_LEFT", JOG_AIM_LEFT },
        { "JOG_AIM_RIGHT", JOG_AIM_RIGHT },
        { "JOG_AIM_BWD", JOG_AIM_BWD },
        { "JOG_AIM_STOP_L", JOG_AIM_STOP_L },
        { "JOG_AIM_STOP_R", JOG_AIM_STOP_R },
        { "JOG_AIM_STOP_BWD", JOG_AIM_STOP_BWD },
        { "JOG_LEFT_TURN", JOG_LEFT_TURN },
        { "JOG_RIGHT_TURN", JOG_RIGHT_TURN },
        { "JOG_112_L", JOG_112_L }, { "JOG_112_R", JOG_112_R },
        { "JOG_90_L", JOG_90_L }, { "JOG_90_R", JOG_90_R },

        // SPRINT
        { "SPRINT", SPRINT },

        // SLIDE
        { "SLIDE_START_R", SLIDE_START_R },
        { "SLIDE_LOOP_R", SLIDE_LOOP_R },
        { "SLIDE_STOP_R", SLIDE_STOP_R },

        // JUMP / FALL
        { "JUMP", JUMP }, { "JUMP_JOG", JUMP_JOG },
        { "JUMP_SPRINT", JUMP_SPRINT },
        { "FALL_LOOP", FALL_LOOP },

        // LAND
        { "LAND", LAND }, { "LAND_TO_JOG", LAND_TO_JOG },
        { "LAND_TO_SPRINT", LAND_TO_SPRINT },
        { "LAND_HARD", LAND_HARD }, { "LAND_MID", LAND_MID },
        { "LAND_SOFT", LAND_SOFT },

        // DODGE
        { "DODGE", DODGE }, { "DODGE_BLINK", DODGE_BLINK },
        { "DODGE_LEFT", DODGE_LEFT }, { "DODGE_RIGHT", DODGE_RIGHT },
        { "DODGE_ATTACK", DODGE_ATTACK },

        // COMBAT / SKILL
        { "COMBAT", COMBAT },
        { "SKILL", SKILL }, { "SKILL2", SKILL2 },
        { "SHIELD_BLOCK1", SHIELD_BLOCK1 }, { "SHIELD_BLOCK2", SHIELD_BLOCK2 },
        { "SHIELD_BLOCK3", SHIELD_BLOCK3 }, { "SHIELD_BLOCK4", SHIELD_BLOCK4 },
        { "SHIELD_BLOCK5", SHIELD_BLOCK5 }, { "SHIELD_BLOCK6", SHIELD_BLOCK6 },
        { "SHIELD_BLOCK7", SHIELD_BLOCK7 },

        // SPELL
        { "SPELL", SPELL }, { "SPELL_180_R", SPELL_180_R },
        { "SPELL_90_R", SPELL_90_R }, { "SPELL_90_L", SPELL_90_L },
        { "SPELL_90_L2", SPELL_90_L2 },

        // LIGHT ATTACK
        { "LIGHT_ATTACK", LIGHT_ATTACK },
        { "LIGHT_ATTACK2", LIGHT_ATTACK2 },
        { "LIGHT_ATTACK3", LIGHT_ATTACK3 },
        { "LIGHT_ATTACK4", LIGHT_ATTACK4 },
        { "LIGHT_ATTACK_180_R", LIGHT_ATTACK_180_R },
        { "LIGHT_ATTACK_90_L", LIGHT_ATTACK_90_L },
        { "LIGHT_ATTACK_90_R1", LIGHT_ATTACK_90_R1 },
        { "LIGHT_ATTACK_90_R2", LIGHT_ATTACK_90_R2 },
        { "LIGHT_ATTACK_90_R3", LIGHT_ATTACK_90_R3 },

        { "ATTACK_END", ATTACK_END },

        // SHUFFLE
        { "SHUFFLE1", SHUFFLE1 }, { "SHUFFLE2", SHUFFLE2 },
        { "SHUFFLE3", SHUFFLE3 }, { "SHUFFLE4", SHUFFLE4 },

        // MAP / POTION
        { "MAPHELP", MAPHELP }, { "POTION", POTION },

        // SPELLS
        { "ACCIO", ACCIO }, { "DESCENDO", DESCENDO },
        { "DIFFINDO", DIFFINDO }, { "DEPULSO", DEPULSO },
        { "LUMOS", LUMOS }, { "LUMOS_STOP", LUMOS_STOP },
        { "DISILLUSION_ENTER", DISILLUSION_ENTER },
        { "DISILLUSION_EXIT", DISILLUSION_EXIT },
        { "ANCIENT_THROW", ANCIENT_THROW },
        { "AVADA_KEDAVRA", AVADA_KEDAVRA },
        { "REPARO_START", REPARO_START },
        { "REPARO_LOOP", REPARO_LOOP },
        { "REPARO_END", REPARO_END },

        { "SPELL_FAIL", SPELL_FAIL },
        { "SPELL_CHARGE", SPELL_CHARGE },

        { "ANCIENT_LIGHTNING", ANCIENT_LIGHTNING },

        // PARRY
        { "PARRY_180", PARRY_180 },
        { "PARRY2", PARRY2 }, { "PARRY3", PARRY3 }, { "PARRY4", PARRY4 }, { "PARRY5", PARRY5 }, { "PARRY6", PARRY6 },

        // BROOM (기존 세트)
        { "BROOM_IDLE", BROOM_IDLE },
        { "BROOM_FWD", BROOM_FWD },
        { "BROOM_LEFT", BROOM_LEFT },
        { "BROOM_RIGHT", BROOM_RIGHT },
        { "BROOM_DOWN", BROOM_DOWN },
        { "BROOM_UP", BROOM_UP },
        { "BROOM_REVELIO", BROOM_REVELIO },

        { "BROOM_HOVER_IDLE", BROOM_HOVER_IDLE },
        { "BROOM_HOVER_FWD", BROOM_HOVER_FWD },
        { "BROOM_HOVER_LEFT", BROOM_HOVER_LEFT },
        { "BROOM_HOVER_RIGHT", BROOM_HOVER_RIGHT },
        { "BROOM_HOVER_START", BROOM_HOVER_START },
        { "BROOM_HOVER_REVELIO", BROOM_HOVER_REVELIO },

        { "BROOM_MOUNT", BROOM_MOUNT },
        { "BROOM_MOUNT_END", BROOM_MOUNT_END },
        { "BROOM_DISMOUNT", BROOM_DISMOUNT },

        { "BROOM_TURBO_FWD", BROOM_TURBO_FWD },
        { "BROOM_TURBO_DOWN", BROOM_TURBO_DOWN },
        { "BROOM_TURBO_UP", BROOM_TURBO_UP },
        { "BROOM_TURBO_LEFT", BROOM_TURBO_LEFT },
        { "BROOM_TURBO_RIGHT", BROOM_TURBO_RIGHT },

        // RUSH
        { "RUSH_START", RUSH_START },
        { "RUSH_LOOP", RUSH_LOOP },
        { "RUSH_END", RUSH_END },

        // THROW ROCK
        { "THROW_ROCK_START", THROW_ROCK_START },
        { "THROW_ROCK", THROW_ROCK },

        // ATTACKS (MONSTER)
        { "BACKHAND_SWING_JOG", BACKHAND_SWING_JOG },
        { "SWING_FWD", SWING_FWD },
        { "SLAM", SLAM },
        { "SLASH", SLASH },
        { "DASH", DASH },

        // SUSTAIN
        { "SUSTAIN_START", SUSTAIN_START },
        { "SUSTAIN_LOOP", SUSTAIN_LOOP },
        { "SUSTAIN_END", SUSTAIN_END },

        // BLINK
        { "BLINK", BLINK },
        { "BLINK_START", BLINK_START },

        // HIT
        { "HIT", HIT },
        { "HIT_FWD", HIT_FWD }, { "HIT_BWD", HIT_BWD },
        { "HIT_BWD2", HIT_BWD2 }, { "HIT_BWD3", HIT_BWD3 },
        { "HIT_BWD4", HIT_BWD4 },
        { "HIT_L", HIT_L }, { "HIT_R", HIT_R },
        { "HIT_FACE", HIT_FACE }, { "HIT_FACE_END", HIT_FACE_END },
        { "HIT_LEVIOSO", HIT_LEVIOSO }, { "INCARCEROUS", INCARCEROUS },
        { "LEVIOSO_LAND",LEVIOSO_LAND },

        // DAZED

        { "DAZED_START", DAZED_START },{ "DAZED_LOOP", DAZED_LOOP },
        { "DAZED_END1", DAZED_END1 }, { "DAZED_END2", DAZED_END2 },

        // KNOCKDOWN
        { "KNOCKDOWN", KNOCKDOWN },
        { "KNOCKDOWN_FWD", KNOCKDOWN_FWD },
        { "KNOCKDOWN_FWD_SPLT", KNOCKDOWN_FWD_SPLT },
        { "KNOCKDOWN_FWD_SPLT_HOLD", KNOCKDOWN_FWD_SPLT_HOLD },
        { "KNOCKDOWN_BWD", KNOCKDOWN_BWD },
        { "KNOCKDOWN_BWD_SPLT", KNOCKDOWN_BWD_SPLT },
        { "KNOCKDOWN_BWD_SPLT_HOLD", KNOCKDOWN_BWD_SPLT_HOLD },

        // GETUP
        { "GETUP", GETUP },
        { "GETUP_BWD", GETUP_BWD }, { "GETUP_FWD", GETUP_FWD },
        { "GETUP_L", GETUP_L }, { "GETUP_L_FD", GETUP_L_FD },
        { "GETUP_L_FU", GETUP_L_FU },
        { "GETUP_R", GETUP_R }, { "GETUP_R_FD", GETUP_R_FD },
        { "GETUP_R_FU", GETUP_R_FU },
        { "GETUP_SLOUCH", GETUP_SLOUCH },

        // DEAD
        { "DEAD", DEAD },
        { "DEAD_FWD", DEAD_FWD }, { "DEAD_FWD2", DEAD_FWD2 },
        { "DEAD_BWD", DEAD_BWD }, { "DEAD_BWD2", DEAD_BWD2 },
        { "DEAD_L", DEAD_L }, { "DEAD_L2", DEAD_L2 },
        { "DEAD_R", DEAD_R }, { "DEAD_R2", DEAD_R2 },

        // KNOCKBACK
        { "KNOCKBACK", KNOCKBACK }, { "KNOCKBACK2", KNOCKBACK2 },
        { "KNOCKBACK_BWD", KNOCKBACK_BWD },
        { "KNOCKBACK_FWD", KNOCKBACK_FWD },

        // TUMBLE
        { "TUMBLE", TUMBLE }, { "TUMBLE2", TUMBLE2 },
        { "TUMBLE_FWD", TUMBLE_FWD },

        // STATUS
        { "PETRIFICUSED_START", PETRIFICUSED_START },
        { "STUN", STUN },

        // FLINCH
        { "FLINCH_BWD", FLINCH_BWD },
        { "FLINCH_FWD", FLINCH_FWD },
        { "FLINCH_LEFT", FLINCH_LEFT },
        { "FLINCH_RIGHT", FLINCH_RIGHT },

        // STUMBLE
        { "STUMBLE_BWD_L", STUMBLE_BWD_L },
        { "STUMBLE_BWD_R", STUMBLE_BWD_R },
        { "STUMBLE_FWD", STUMBLE_FWD },

        // HOVER
        { "HOVER_LOOP", HOVER_LOOP },
        { "HOVER_DASH_FWD", HOVER_DASH_FWD },
        { "HOVER_DASH_BWD", HOVER_DASH_BWD },
        { "HOVER_DASH_LEFT", HOVER_DASH_LEFT },
        { "HOVER_DASH_RIGHT", HOVER_DASH_RIGHT },

        // FIREBALL
        { "FIREBALL1_A", FIREBALL1_A }, { "FIREBALL2_A", FIREBALL2_A },
        { "FIREBALL1_G", FIREBALL1_G }, { "FIREBALL2_G", FIREBALL2_G },

        // FIREBREATH / SWEEP
        { "FIREBREATH_A", FIREBREATH_A },
        { "FIREBREATH_COOLDOWN_A", FIREBREATH_COOLDOWN_A },
        { "FIREBREATH_WINDUP_A", FIREBREATH_WINDUP_A },
        { "FIRESWEEP_A", FIRESWEEP_A },
        { "FIRESWEEP_COOLDOWN_A", FIRESWEEP_COOLDOWN_A },
        { "FIRESWEEP_WINDUP_A", FIRESWEEP_WINDUP_A },
        { "FIREBREATH_G", FIREBREATH_G },
        { "FIRESWEEP_G", FIRESWEEP_G },
        { "FIRESWEEP_G_L", FIRESWEEP_G_L }, { "FIRESWEEP_G_R", FIRESWEEP_G_R },

        // ETC
        { "PULSE", PULSE },
        { "GROUND_SWIPE_L", GROUND_SWIPE_L },
        { "GROUND_SWIPE_R", GROUND_SWIPE_R },

        // FLY
        { "TUCKED", TUCKED },
        { "FLY_TO_HOVER", FLY_TO_HOVER },
        { "FLY", FLY },

        { "FIREBURST_REEL", FIREBURST_REEL },

        { "OPEN_DOOR", OPEN_DOOR },
        { "OPEN_DOOR_L", OPEN_DOOR_L },
        { "OPEN_DOOR_R", OPEN_DOOR_R },

        { "FEAR_L", FEAR_L },
        { "FEAR_R", FEAR_R },

        // SPELL LEARNING
        { "SPELL_LEARNING_FAIL", SPELL_LEARNING_FAIL },
        { "SPELL_LEARNING_LOOP", SPELL_LEARNING_LOOP },
        { "SPELL_LEARNING_START", SPELL_LEARNING_START },
        { "SPELL_LEARNING_SUCCESS", SPELL_LEARNING_SUCCESS },
        { "SPELL_LEARNING_WANDWAVE", SPELL_LEARNING_WANDWAVE },

        // CUT SCENES TROLL
        { "CUTSCENE_TROLLINTRO", CUTSCENE_TROLLINTRO },
        // CUT SCENES OPENING
        { "CUTSCENE_OPENINGINTRO1", CUTSCENE_OPENINGINTRO1 },
        { "CUTSCENE_OPENINGINTRO2", CUTSCENE_OPENINGINTRO2 },
        { "CUTSCENE_OPENINGINTRO3", CUTSCENE_OPENINGINTRO3 },

        // BROOM (B 세트)
        { "BROOM_MOUNT_B", BROOM_MOUNT_B },
        { "BROOM_HOVER_IDLE_B", BROOM_HOVER_IDLE_B },
        { "BROOM_HOVER_STOP_B", BROOM_HOVER_STOP_B },
        { "BROOM_HOVER_DOWN_B", BROOM_HOVER_DOWN_B },
        { "BROOM_HOVER_UP_B", BROOM_HOVER_UP_B },
        { "BROOM_HOVER_LEFT_B", BROOM_HOVER_LEFT_B },
        { "BROOM_HOVER_RIGHT_B", BROOM_HOVER_RIGHT_B },
        { "BROOM_FLY_DOWN_B", BROOM_FLY_DOWN_B },
        { "BROOM_FLY_LEFT_B", BROOM_FLY_LEFT_B },
        { "BROOM_FLY_RIGHT_B", BROOM_FLY_RIGHT_B },
        { "BROOM_FLY_UP_B", BROOM_FLY_UP_B },
        { "BROOM_FLY_B", BROOM_FLY_B },
        { "BROOM_ADD", BROOM_ADD },
    };




    auto it = table.find(s);
    if (it == table.end())
    {
        assert(false && "Unknown animation string");
        return STATEANIM::END;
    }
    return it->second;
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
