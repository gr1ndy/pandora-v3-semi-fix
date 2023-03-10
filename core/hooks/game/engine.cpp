#include "../../../includes.h"

bool Hooks::IsConnected( ) {
	if( !this || !g_csgo.m_engine )
		return false;
	
	Stack stack;

	static const Address IsLoadoutAllowed{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 04 B0 01 5F" ) ) };

	if(g_cfg[XOR("misc_unlock_inventory")].get<bool>() && stack.ReturnAddress( ) == IsLoadoutAllowed )
		return false;

	return g_hooks.m_engine.GetOldMethod< IsConnected_t >( IVEngineClient::ISCONNECTED )( this );
}

void Hooks::ProcessMovement(Player* player, CMoveData* data)
{
	// fix prediction error in air (by stop calculating some vars in movement)
	data->m_bGameCodeMovedPlayer = false;
	g_hooks.m_game_movement.GetOldMethod< ProcessMovement_t >(1)(this, player, data);
}

void* Hooks::AllocKeyValuesMemory(int iSize)
{
	Stack stack;

	static const Address uAllocKeyValuesEngine{ pattern::find(g_csgo.m_engine_dll, XOR("85 C0 74 0F 51 6A 00 56 8B C8 E8 ? ? ? ? 8B F0")) };
	static const Address uAllocKeyValuesClient{ pattern::find(g_csgo.m_client_dll, XOR("85 C0 74 10 6A 00 6A 00 56 8B C8 E8 ? ? ? ? 8B F0")) };

	if (stack.ReturnAddress() == uAllocKeyValuesEngine || stack.ReturnAddress() == uAllocKeyValuesClient)
		return nullptr;

	return g_hooks.m_key_values_system.GetOldMethod<AllocKeyValuesMemory_t>(2)(this, iSize);
}

void Hooks::FireEvents()
{
	/*if (!g_cl.m_processing) {
		g_hooks.m_engine.GetOldMethod< IsConnected_t >(IVEngineClient::FIREEVENTS)(this);
		return;
	}

	if (g_cl.m_allow_fire_events) {
		g_hooks.m_engine.GetOldMethod< IsConnected_t >(IVEngineClient::FIREEVENTS)(this);

		g_cl.m_allow_fire_events = false;
	}*/
	
	g_hooks.m_engine.GetOldMethod< IsConnected_t >(IVEngineClient::FIREEVENTS)(this);
}

bool Hooks::IsHLTV( ) {
	if( !this || !g_csgo.m_engine )
		return false;
	
	Stack stack;

	static const Address SetupVelocity{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80" ) ) };

	// AccumulateLayers
	if( g_anims.m_bEnablePVS)
		return true;

	// fix for animstate velocity.
	if( stack.ReturnAddress( ) == SetupVelocity )
		return true; 

	return g_hooks.m_engine.GetOldMethod< IsHLTV_t >( IVEngineClient::ISHLTV )( this );
}

bool Hooks::IsPaused() {
	static DWORD* return_to_extrapolation = (DWORD*)(pattern::find(g_csgo.m_client_dll,
		XOR("FF D0 A1 ?? ?? ?? ?? B9 ?? ?? ?? ?? D9 1D ?? ?? ?? ?? FF 50 34 85 C0 74 22 8B 0D ?? ?? ?? ??")) + 0x29);

	if (_ReturnAddress() == (void*)return_to_extrapolation)
		return true;

	return g_hooks.m_engine.GetOldMethod< IsPaused_t >(IVEngineClient::ISPAUSED)(this);
}