#include "sys_event.h"

#include "../../qcommon/qcommon.h"

#include "../../client/keys.h"

// KB+Mouse
enum class KeyboardMouseInputAction : uint32_t
{
    // Keyboard
    ESCAPE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    BREAK,
    INSERT,
    DEL,
    NUM_LOCK,
    KP_MULTIPLY,

    ACUTE,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    NUM_0,
    MINUS,
    EQUAL,
    BACK_SPACE,
    KP_ADD,
    KP_SUBTRACT,

    TAB,
    Q,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    BRACKET_LEFT,
    BRACKET_RIGHT,
    BACK_SLASH,
    KP_7_HOME,
    KP_8_UP,
    KP_9_PAGE_UP,

    CAPS_LOCK,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    SEMICOLON,
    APOSTROPHE,
    ENTER,
    KP_4_LEFT,
    KP_5_BEGIN,
    KP_6_RIGHT,

    SHIFT_L,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,
    COMMA,
    PERIOD,
    FWRD_SLASH,
    SHIFT_R,
    KP_1_END,
    KP_2_DOWN,
    KP_3_PAGE_DOWN,

    CTRL_L,
    ALT_L,
    SPACE,
    ALT_R,
    CTRL_R,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    KP_0_INSERT,

    // Mouse
    LEFT_CLICK,
    MID_CLICK,
    RIGHT_CLICK,
    SCROLL_UP,
    SCROLL_DOWN,
    COUNT
};

ActionMappingDesc gKeyboardMouseActionMappings[] = {
    // Keyboard
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::ESCAPE),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_ESCAPE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F1),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F1},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F2),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F2},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F3),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F3},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F4),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F4},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F5),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F5},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F6),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F6},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F7),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F7},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F8),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F8},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F9),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F9},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F10),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F10},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F11),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F11},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F12),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F12},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::BREAK),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BREAK},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::INSERT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_INSERT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::DEL),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_DELETE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_LOCK),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_NUM_LOCK},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_MULTIPLY),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_MULTIPLY},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::ACUTE),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_EXTRA5},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_1),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_1},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_2),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_2},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_3),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_3},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_4),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_4},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_5),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_5},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_6),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_6},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_7),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_7},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_8),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_8},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_9),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_9},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::NUM_0),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_0},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::MINUS),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_MINUS},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::EQUAL),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_EQUAL},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::BACK_SPACE),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BACK_SPACE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_ADD),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_ADD},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_SUBTRACT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_SUBTRACT},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::TAB),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_TAB},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::Q),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_Q},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::W),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_W},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::E),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_E},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::R),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::T),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_T},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::Y),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_Y},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::U),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_U},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::I),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_I},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::O),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_O},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::P),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_P},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::BRACKET_LEFT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BRACKET_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::BRACKET_RIGHT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BRACKET_RIGHT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::BACK_SLASH),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BACKSLASH},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_7_HOME),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_HOME},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_8_UP),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_UP},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_9_PAGE_UP),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_PAGE_UP},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::CAPS_LOCK),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_CAPS_LOCK},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::A),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_A},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::S),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_S},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::D),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_D},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::F),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::G),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_G},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::H),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_H},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::J),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_J},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::K),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_K},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::L),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_L},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::SEMICOLON),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SEMICOLON},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::APOSTROPHE),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_APOSTROPHE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::ENTER),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_RETURN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_4_LEFT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_5_BEGIN),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_BEGIN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_6_RIGHT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_RIGHT},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::SHIFT_L),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SHIFT_L},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::Z),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_Z},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::X),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_X},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::C),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_C},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::V),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_V},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::B),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_B},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::N),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_N},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::M),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_M},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::COMMA),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_COMMA},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::PERIOD),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_PERIOD},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::FWRD_SLASH),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SLASH},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::SHIFT_R),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SHIFT_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_1_END),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_END},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_2_DOWN),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_DOWN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_3_PAGE_DOWN),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_PAGE_DOWN},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::CTRL_L),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_CTRL_L},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::ALT_L),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_ALT_L},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::SPACE),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SPACE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::ALT_R),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_ALT_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::CTRL_R),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_CTRL_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::LEFT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::RIGHT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_RIGHT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::UP),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_UP},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::DOWN),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_DOWN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::KP_0_INSERT),
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_INSERT},
    },

    // Mouse
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::LEFT_CLICK),
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::MID_CLICK),
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_MIDDLE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::RIGHT_CLICK),
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_RIGHT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::SCROLL_UP),
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_WHEEL_UP},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = static_cast<uint32_t>(KeyboardMouseInputAction::SCROLL_DOWN),
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_WHEEL_DOWN},
    },
};

bool SYS_input_handler(InputActionContext *ctx)
{
    LOGF(LogLevel::eDEBUG, "Action: %d, Value %i.", ctx->mActionId, ctx->mBool);

    return true;
}

void SYS_register_input()
{
    addActionMappings(gKeyboardMouseActionMappings, TF_ARRAY_COUNT(gKeyboardMouseActionMappings),
                      INPUT_ACTION_MAPPING_TARGET_ALL);

    for (uint32_t i = 0u; i < static_cast<uint32_t>(KeyboardMouseInputAction::COUNT); ++i)
    {
        InputActionDesc actionDesc = {
            .mActionId = i,
            .pFunction = SYS_input_handler,
        };
        addInputAction(&actionDesc);
    }
}