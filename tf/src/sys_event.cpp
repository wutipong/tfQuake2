#include "sys_event.h"

extern "C"{
#include "../../qcommon/qcommon.h"
#include "../../client/keys.h"

uint32_t sys_msg_time;
}

#include <ITime.h>

ActionMappingDesc gKeyboardMouseActionMappings[] = {
    // Keyboard
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_ESCAPE,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_ESCAPE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F1,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F1},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F2,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F2},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F3,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F3},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F4,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F4},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F5,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F5},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F6,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F6},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F7,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F7},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F8,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F8},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F9,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F9},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F10,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F10},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F11,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F11},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_F12,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F12},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_PAUSE,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BREAK},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_INS,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_INSERT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_HOME,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_INSERT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_PGUP,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_INSERT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_DEL,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_DELETE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_PGDN,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_INSERT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_END,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_INSERT},
    },
    /*
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_NUM_LOCK,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_NUM_LOCK},
    },*/
    /*
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_MULTIPLY,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_MULTIPLY},
    },
    */

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '`',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_GRAVE},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '1',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_1},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '2',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_2},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '3',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_3},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '4',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_4},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '5',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_5},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '6',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_6},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '7',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_7},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '8',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_8},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '9',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_9},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '0',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_0},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '-',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_MINUS},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '=',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_EQUAL},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_BACKSPACE,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BACK_SPACE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_PLUS,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_ADD},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_MINUS,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_SUBTRACT},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_TAB,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_TAB},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'q',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_Q},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'w',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_W},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'e',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_E},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'r',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 't',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_T},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'y',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_Y},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'u',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_U},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'i',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_I},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'o',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_O},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'p',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_P},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '[',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BRACKET_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = ']',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BRACKET_RIGHT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '\\',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_BACKSLASH},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_HOME,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_HOME},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_UPARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_UP},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_PGUP,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_PAGE_UP},
    },
    /*
        {
            .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
            .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
            .mActionId = K_CAPS_LOCK,
            .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_CAPS_LOCK},
        },
    */
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'a',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_A},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 's',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_S},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'd',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_D},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'f',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_F},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'g',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_G},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'h',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_H},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'j',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_J},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'k',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_K},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'l',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_L},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = ';',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SEMICOLON},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '\'',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_APOSTROPHE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_ENTER,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_RETURN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_LEFTARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_5,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_BEGIN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_RIGHTARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_RIGHT},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_SHIFT,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SHIFT_L, KeyboardButton::KEYBOARD_BUTTON_SHIFT_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'z',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_Z},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'x',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_X},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'c',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_C},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'v',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_V},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'b',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_B},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'n',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_N},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = 'm',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_M},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = ',',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_COMMA},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '.',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_PERIOD},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = '/',
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SLASH},
    },
    /*
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_SHIFT,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SHIFT_R},
    },
    */
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_END,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_END},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_DOWNARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_DOWN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_PGDN,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_PAGE_DOWN},
    },

    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_CTRL,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_CTRL_L, KeyboardButton::KEYBOARD_BUTTON_CTRL_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_ALT,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_ALT_L, KeyboardButton::KEYBOARD_BUTTON_ALT_R},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_SPACE,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_SPACE},
    },
    /*
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_ALT,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_ALT_R},
    },
    */
    /*
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_CTRL,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_CTRL_R},
    },
    */
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_LEFTARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_RIGHTARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_RIGHT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_UPARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_UP},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_DOWNARROW,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_DOWN},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_INS,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_INSERT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_KEYBOARD,
        .mActionId = K_KP_DEL,
        .mDeviceButtons = {KeyboardButton::KEYBOARD_BUTTON_KP_DELETE},
    },

    // Mouse
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = K_MOUSE1,
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_LEFT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = K_MOUSE3,
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_MIDDLE},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = K_MOUSE2,
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_RIGHT},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = K_MWHEELUP,
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_WHEEL_UP},
    },
    {
        .mActionMappingType = INPUT_ACTION_MAPPING_NORMAL,
        .mActionMappingDeviceTarget = INPUT_ACTION_MAPPING_TARGET_MOUSE,
        .mActionId = K_MWHEELDOWN,
        .mDeviceButtons = {MouseButton::MOUSE_BUTTON_WHEEL_DOWN},
    },
};

bool SYS_input_handler(InputActionContext *ctx)
{
    LOGF(LogLevel::eDEBUG, "Action: %d, Value %i.", ctx->mActionId, ctx->mBool);

    sys_msg_time = getTimeSinceStart();
    switch (ctx->mDeviceType)
    {
    case InputDeviceType::INPUT_DEVICE_KEYBOARD:
        Key_Event( ctx->mActionId, ctx->mBool, sys_msg_time);
        break;

    case InputDeviceType::INPUT_DEVICE_MOUSE:
        Key_Event(ctx->mActionId, ctx->mBool, sys_msg_time);
        break;
    default:
        return false;
    }
    return true;
}

void SYS_register_input()
{
    addActionMappings(gKeyboardMouseActionMappings, TF_ARRAY_COUNT(gKeyboardMouseActionMappings),
                      INPUT_ACTION_MAPPING_TARGET_ALL);

    for (auto &m : gKeyboardMouseActionMappings)
    {
        InputActionDesc actionDesc = {
            .mActionId = m.mActionId,
            .pFunction = SYS_input_handler,
        };
        addInputAction(&actionDesc);
    }
}