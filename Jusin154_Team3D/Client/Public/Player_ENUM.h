#pragma once
#include "Client_Define.h"

NS_BEGIN(Client)

enum class PLAYER_MESH_ORDER {
	HEAD_EYE_OCC,
	HEAD_FACE,
	HEAD_TEETH,
	HEAD_EYELASH,
	HEAD_EYES,
	BODY_ARMS,
	HAIR_MAIN,
	HAIR_MAIN_CLOTH,
	LOWER,
	SHOES,
	UPPER,
	HAIR_SUB,
	HAIR_SUB_CLOTH,
	ROBE_SKIRT,
	ROBE_FUR,
	ROBE_CLOTH,
	END
};

enum class PLAYER_JOINT_ORDER {
		RIGHTUP
	,	TAILUP
	,	LEFTUP

	,	RIGHT
	,	TAIL
	,	LEFT

	,	RIGHTDOWN
	,	RIGHTDOWNRIGHT
	,	TAILRIGHTDOWN

	,	TAILLEFTDOWN
	,	LEFTDOWNLEFT
	,	LEFTDOWN
	,	END
};

static const _char* PLAYER_JOINT_BONE_NAMES[12] = {
			"RightUpLeg_Cloth"
		,	"TailUp_Cloth"
		,	"LeftUpLeg_Cloth"

		,	"RightLeg_Cloth"
		,	"Tail_Cloth"
		,	"LeftLeg_Cloth"

		,	"RightDownLeg_Cloth"
		,	"RightDownRightLeg_Cloth"
		,	"TailRightDown_Cloth"

		,	"TailLeftDown_Cloth"
		,	"LeftDownLeftLeg_Cloth"
		,	"LeftDownLeg_Cloth"
};

enum class PLAYER_JOINT_ROUTE_ORDER {
		UP_R_T
	,	UP_T_L

	,	MIDDLE_R_T
	,	MIDDLE_T_L

	,	DOWN_R_RR
	,	DOWN_RR_TR

	,	DOWN_TR_TL

	,	DOWN_TL_L
	,	DOWN_L_LL
	,	END
};

NS_END
