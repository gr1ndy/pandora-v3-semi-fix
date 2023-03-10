#include "../includes.h"

equlent g_resolver{ };;

void equlent::ResolveAngles( Player* player, LagComp::LagRecord_t* record ) {
	AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

	if( !record->m_bDidShot ) {
		if( g_cfg[ XOR( "cheat_mode" ) ].get<int>( ) == 1 ) {
			switch( data->m_missed_shots % 6 ) {
			case 0:
			case 1:
				record->m_angEyeAngles.x = player->m_angEyeAngles( ).x = 90.f;
				break;

			case 2:
			case 3:
				record->m_angEyeAngles.x = player->m_angEyeAngles( ).x = 0.f;
				break;
			case 4:
			case 5:
				record->m_angEyeAngles.x = player->m_angEyeAngles( ).x = -90.f;
				break;

			default:
				break;
			}
		}

		//if (record->m_mode == Modes::RESOLVE_STAND)
		ResolveStand( data, record );
	}
}

void equlent::ResolveStand( AimPlayer* data, LagComp::LagRecord_t* record ) {


	float max_rotation = record->m_pEntity->GetMaxBodyRotation( );
	float resolve_value = 50.f;

	if( !record->m_pState )
		return;

	const auto info = g_anims.GetAnimationInfo( record->m_pEntity );
	if( !info )
		return;

	float eye_yaw = record->m_pState->m_flEyeYaw;

	if( max_rotation < resolve_value )
		resolve_value = max_rotation;

	data->m_extending = record->m_pLayers[ 3 ].m_cycle == 0.f && record->m_pLayers[ 3 ].m_weight == 0.f;

	if( data->m_extending )
		resolve_value = max_rotation;

	if( record->m_bDidShot )
		info->m_flBrute = equlent::ResolveShot( data, record );

	else 
	{
		float lbyt = record->m_pEntity->m_flLowerBodyYawTarget( );
		data->m_delta = std::abs( math::NormalizedAngle( eye_yaw - lbyt ) );

		float resolve_yaw = ( ( data->m_delta < 0.f ) ? resolve_value : resolve_value );

		switch( data->m_missed_shots % 3 ) {
		case 0:
			info->m_flBrute = data->m_last_resolve = resolve_yaw;
			break;
		case 1:
			info->m_flBrute = -data->m_last_resolve;
			break;
		case 2:
			info->m_flBrute = 0;
			break;
		}
	}

	record->m_pState->m_flGoalFeetYaw = eye_yaw + info->m_flBrute;
}

float equlent::ResolveShot( AimPlayer* data, LagComp::LagRecord_t* record ) {
	float flPseudoFireYaw = math::NormalizedAngle( math::CalcAngle( record->m_pMatrix[ 8 ].GetOrigin( ), g_cl.m_local->m_BoneCache( ).m_pCachedBones[ 0 ].GetOrigin( ) ).y );

	if( data->m_extending ) {
		float flLeftFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y + 58.f ) ) );
		float flRightFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y - 58.f ) ) );

		//g_notify.add( tfm::format( XOR( "found shot record on %s: [ yaw: %i ]" ), game::GetPlayerName( record->m_player->index( ) ), int( flLeftFireYawDelta > flRightFireYawDelta ? -58.f : 58.f ) ) );

		return flLeftFireYawDelta > flRightFireYawDelta ? -58.f : 58.f;
	}
	else {
		float flLeftFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y + 29.f ) ) );
		float flRightFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y - 29.f ) ) );

		//g_notify.add( tfm::format( XOR( "found shot record on %s: [ yaw: %i ]" ), game::GetPlayerName( record->m_player->index( ) ), int( flLeftFireYawDelta > flRightFireYawDelta ? -29.f : 29.f ) ) );

		return flLeftFireYawDelta > flRightFireYawDelta ? -29.f : 29.f;
	}
}