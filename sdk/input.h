#pragma once

constexpr int MULTIPLAYER_BACKUP = 150;

enum button_flags_t : int {
	IN_ATTACK    = ( 1 << 0 ),
	IN_JUMP      = ( 1 << 1 ),
	IN_DUCK      = ( 1 << 2 ),
	IN_FORWARD   = ( 1 << 3 ),
	IN_BACK      = ( 1 << 4 ),
	IN_USE       = ( 1 << 5 ),
	IN_CANCEL    = ( 1 << 6 ),
	IN_LEFT      = ( 1 << 7 ),
	IN_RIGHT     = ( 1 << 8 ),
	IN_MOVELEFT  = ( 1 << 9 ),
	IN_MOVERIGHT = ( 1 << 10 ),
	IN_ATTACK2   = ( 1 << 11 ),
	IN_RUN       = ( 1 << 12 ),
	IN_RELOAD    = ( 1 << 13 ),
	IN_ALT1      = ( 1 << 14 ),
	IN_ALT2      = ( 1 << 15 ),
	IN_SCORE     = ( 1 << 16 ),
	IN_SPEED     = ( 1 << 17 ),
	IN_WALK      = ( 1 << 18 ),
	IN_ZOOM      = ( 1 << 19 ),
	IN_WEAPON1   = ( 1 << 20 ),
	IN_WEAPON2   = ( 1 << 21 ),
	IN_BULLRUSH  = ( 1 << 22 ),
	IN_GRENADE1  = ( 1 << 23 ),
	IN_GRENADE2  = ( 1 << 24 ),
	IN_ATTACK3   = ( 1 << 25 ),
};


class CCSUsrMsg_ServerRankRevealAll {
public:
	__forceinline CCSUsrMsg_ServerRankRevealAll( ) : seconds_till_shutdown{} {}
public:
	PAD( 0x8 );
	int seconds_till_shutdown;
};

class CUserCmd {
public:
	CUserCmd()
	{
		memset(this, 0, sizeof(*this));
	};
	virtual ~CUserCmd() {};

	CRC32_t get_checksum(void) const
	{
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &m_command_number, sizeof(m_command_number));
		CRC32_ProcessBuffer(&crc, &m_tick, sizeof(m_tick));
		CRC32_ProcessBuffer(&crc, &m_view_angles, sizeof(m_view_angles));
		CRC32_ProcessBuffer(&crc, &m_aimdirection, sizeof(m_aimdirection));
		CRC32_ProcessBuffer(&crc, &m_forward_move, sizeof(m_forward_move));
		CRC32_ProcessBuffer(&crc, &m_side_move, sizeof(m_side_move));
		CRC32_ProcessBuffer(&crc, &m_up_move, sizeof(m_up_move));
		CRC32_ProcessBuffer(&crc, &m_buttons, sizeof(m_buttons));
		CRC32_ProcessBuffer(&crc, &m_impulse, sizeof(m_impulse));
		CRC32_ProcessBuffer(&crc, &m_weapon_select, sizeof(m_weapon_select));
		CRC32_ProcessBuffer(&crc, &m_weapon_subtype, sizeof(m_weapon_subtype));
		CRC32_ProcessBuffer(&crc, &m_random_seed, sizeof(m_random_seed));
		CRC32_ProcessBuffer(&crc, &m_mousedx, sizeof(m_mousedx));
		CRC32_ProcessBuffer(&crc, &m_mousedy, sizeof(m_mousedy));
		CRC32_ProcessBuffer(&crc, &m_predicted, sizeof(m_predicted));
		CRC32_ProcessBuffer(&crc, &m_head_angles, sizeof(m_head_angles));
		CRC32_ProcessBuffer(&crc, &m_head_offset, sizeof(m_head_offset));

		CRC32_Final(&crc);
		return crc;
	}
	int     m_command_number;
	int     m_tick;
	ang_t   m_view_angles;
	vec3_t  m_aimdirection;
	float   m_forward_move;
	float   m_side_move;
	float   m_up_move;
	int     m_buttons;
	char m_impulse;
	int     m_weapon_select;
	int     m_weapon_subtype;
	int     m_random_seed;
	short   m_mousedx;
	short   m_mousedy;
	bool    m_predicted;
	vec3_t  m_head_angles;
	vec3_t  m_head_offset;
};

class CVerifiedUserCmd {
public:
	CUserCmd   m_cmd;
	ulong_t    m_crc;
};

class IInputSystem {
public:
	enum indices : size_t {
		ENABLEINPUT          = 11,
		ISBUTTONDOWN         = 15,
		GETBUTTONPRESSEDTICK = 16,
		GETCURSORPOSITION    = 56,
	};

public:
	__forceinline void EnableInput( bool enable ) {
		return util::get_method< void( __thiscall* )( decltype( this ), bool ) >( this, ENABLEINPUT )( this, enable );
	}

	__forceinline void GetCursorPosition( int* x, int* y ) {
		return util::get_method< void( __thiscall* )( decltype( this ), int*, int* ) >( this, GETCURSORPOSITION )( this, x, y );
	}
};

class CInput {
public:
	enum indices : size_t {
		CREATEMOVE                = 3,
		WRITEUSERCMDDELTATOBUFFER = 5,
		ENCODEUSERCMDTOBUFFER     = 6,
		GETUSERCMD                = 8,
		CAMISTHIRDPERSON          = 32,
		CAMTOTHIRDPERSON          = 35,
		CAMTOFIRSTPERSON          = 36,
	};

public:
	PAD(0xC);
	bool              m_trackir;					// 0x04
	bool              m_mouse_init;					// 0x05
	bool              m_mouse_active;				// 0x06
	PAD( 154 );									// 0x38
	bool              m_camera_in_third_person;		// 0x9D
	bool              m_camera_moving_with_mouse;	// 0x9A 
	vec3_t            m_camera_offset;				// 0xA0
	//bool              m_camera_distance_move;		// 0xAC
	//int               m_camera_old_x;				// 0xB0
	//int               m_camera_old_y;				// 0xB4
	//int               m_camera_x;					// 0xB8
	//int               m_camera_y;					// 0xBC
	//bool              m_camera_is_orthographic;		// 0xC0
	//ang_t             m_previous_view_angles;		// 0xC4
	//ang_t             m_previous_view_angles_tilt;	// 0xD0
	//float             m_last_forward_move;			// 0xDC
	//int               m_clear_input_state;			// 0xE0
	PAD(0x38);
	CUserCmd*         m_commands;					// 0xEC
	CVerifiedUserCmd* m_verified;					// 0xF0

public:
	__forceinline int CAM_IsThirdPerson( int slot = -1 ) {
		return util::get_method< int( __thiscall* )( decltype( this ), int ) >( this, CAMISTHIRDPERSON )( this, slot );
	}

	__forceinline void CAM_ToThirdPerson( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, CAMTOTHIRDPERSON )( this );
	}

	__forceinline void CAM_ToFirstPerson( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, CAMTOFIRSTPERSON )( this );
	}

	__forceinline CUserCmd* GetUserCmd( int slot, int sequence_number ) {
		return util::get_method< CUserCmd*( __thiscall* )( decltype( this ), int, int ) >( this, GETUSERCMD )( this, slot, sequence_number );
	}

	__forceinline CUserCmd* GetUserCmd(int sequence_number) {
		return &m_commands[sequence_number % 150];
	}

	__forceinline CVerifiedUserCmd* GetVerifiedUserCmd(int sequence_number)
	{
		return &m_verified[sequence_number % 150];
	}

};