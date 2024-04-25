#pragma once

// GC (Game Constants) Namespace: Contains constants used throughout the game for easy management and accessibility.
namespace GC {
	// UI Constants
	const DirectX::SimpleMath::Vector4 UI_SELECT_COLOR = { 0.5f, 0.5f, 0.5f, 1.0f };    // Color for selected UI element.
	const DirectX::SimpleMath::Vector4 UI_INTERACTING_COLOR = { 0.1f, 0.1f, 0.1f, 1.0f };    // Color for interacting wtih UI element.
	const DirectX::SimpleMath::Vector4 UI_REGULAR_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };   // Color for regular UI element.
	const float UI_ARROW_OFFSET = 20.0f;

	const std::string UI_TUTORIAL_CONTROLLER_GUIDE = "MOVEMENT\nYOU CAN MOVE LEFT OR RIGHT USING\nTHE LEFT JOYSTICK OF YOUR CONTROLLER\n\nSHOOTING\nTO SHOOT YOU CAN EITHER PRESS THE A BUTTON\nOR YOU CAN PULL THE RIGHT TRIGGER\n\nMENUS\nJUST MOVE YOUR LEFT JOYSTICK, OR DPAD, UP OR\n DOWN TO MOVE THROUGH THE BUTTONS AND\nPRESS A TO ACTIVATE THEM\n\nSCORE MENU\nMOVE YOUR RIGHT JOYSTICK UP OR DOWN TO\nSCROLL THROUGH THE SCORES";
	const std::string UI_TUTORIAL_KEYBOARD_GUIDE = "MOVEMENT\nYOU CAN MOVE LEFT OR RIGHT USING\nTHE A,D,LEFT OR RIGHT ARROW KEYS\n\nSHOOTING\nTO SHOOT YOU CAN EITHER PRESS SPACE\nOR YOU CAN CLICK YOUR LEFT MOUSE BUTTON\n\nMENUS\nJUST MOVE YOUR MOUSE OR USE THE ARROW\nKEYS UP OR DOWN TO MOVE THROUGH THE\nBUTTONS AND PRESS ENTER TO ACTIVATE THEM\n\nSCORE MENU\nPRESS YOUR W,S,UP OR DOWN ARROW KEYS\nTO SCROLL THROUGH THE SCORES";

	// IntroMode Constants
	const DirectX::SimpleMath::Vector3 DEF_CAM_POS = DirectX::SimpleMath::Vector3(3, 5, -15);   // Default camera position in IntroMode.
	const DirectX::SimpleMath::Vector3 CAMERA_END_POS = DirectX::SimpleMath::Vector3(0, 3, -1); // Ending camera position in IntroMode.

	// PlayMode Constants
	const float GAME_OVER_TRANSITION_TIME = 5.0f;  // Time before transitioning to the game over state.
	const int BGND_LAYERS = 2;                     // Number of background layers in PlayMode.
	const RECTF MISSILE_SPIN_FRAMES[]{
		// Frames for animating missile spin.
		{ 1,  0, 36, 52},
		{ 37, 0, 66, 52 },
		{ 67, 0, 93, 52 },
		{ 94, 0, 123, 52 },
	};

	// ScoreMenu Constants
	const float SCROLL_LIST_INC = 500.0f;
	const float SCROLL_LIST_MAX = 1.1f;
	const float SCROLL_LIST_MIN = -20.0f;

	// Player Constants
	const float SHIP_SPEED = 350.0f;     // Speed of the player's ship.
	const float MISSILE_SPEED = 450.0f;  // Speed of the missiles fired by the player.
	const float SCROLL_SPEED = 50.0f;	 // Speed of the background scroll in PlayMode.
	const float FIRE_DELAY = 0.3f;		 // Delay between fires.

	// Shelter Constants
	const static int NUM_SHELTERS = 4;			   // Number of shelters in the game.
	const float SHELTER_OFFSET_Y = 110.0f;         // Vertical offset of the shelters.
	const static int SHELTER_TEXTURE_STATES = 10;  // Number of texture states for the shelter damage.

	// Enemy Constants
	const static int NUM_ENEMIES = 55;  // Int that holds the maximum amount of enemies

	const float ENEMY_SPEED_INC = 3.5f;    // Speed increase of enemies over time.
	const float ENEMY_UFO_SPEED = 200.0f;  // Speed of the UFO enemy.
	const float LASER_SPEED = 200.0f;      // Speed of the enemy lasers.

	const static int ENEMY_TIME_BTWN_SHOTS = 1;  // Amount of time between the chance to shoot
	const static int ENEMY_SHOOT_CHANCE = 3;     // (0-100) Example: 34 equals a 34% chance for the enemy to shoot

	const static int ENEMY_UFO_CHANCE = 5;  // Chance of a ufo spawning (from 0-100) everytime the enemies move down

	// Constant variables used for enemy behavior and positioning
	const static int ENEMY_DOWNSTEP = 15;     // Vertical step for enemies after reaching a boundary
	const static int ENEMY_LIMIT_OFFSET = 25;  // Offset from screen edges for enemy movement

	// Enemy positioning and scoring values
	const static int ENEMIES_PER_ROW = 11;   // Number of enemies per row
	//const static int NUM_ROWS = 5;           // Total number of rows of enemies
	//const static int ROWX_SPACING = 10;      // Horizontal spacing between rows
	//const static int ROWY_SPACING = 50;      // Vertical spacing between rows
	//const static int ENEMY_INITIAL_Y = 120;  // Initial Y position for the top row
	//const static int UFO_INITIAL_Y = 50;     // Initial Y position for the ufo

	// Score Constants
	const std::string SCORE_FILE_PATH = "data/scores.dat";             // Path to the score file.
	const std::string ENCRYPT_KEY = "=ami%#Ip,Mo@l+sMI]/t$j$`KDwzbQ";  // Key for encrypting and decrypting scores.
	const static int MAX_SCORES_SAVE = 100;                            // Maximum number of scores to save.

	const static int OCTOPUS_POINTS = 10;          // Points for defeating an octopus enemy.
	const static int CRAB_POINTS = 20;             // Points for defeating a crab enemy.
	const static int SQUID_POINTS = 40;            // Points for defeating a squid enemy.
	const int UFO_POINTS[4] = { 50,100,150,300 };  // Randomly selected points for defeating a UFO enemy.
	const static int WAVE_FINISH_POINTS = 1000;    // Bonus points for completing a wave.

#if defined(DEBUG) || (_DEBUG)
	// Debugging Constants
	const float DEBUG_CAM_INC = 1.0f;  // Camera increment for debug movements.
#endif
};