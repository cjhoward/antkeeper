/*
 * Copyright (C) 2020  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sdl-scancode-table.hpp"
#include "scancode.hpp"

const scancode sdl_scancode_table[287] =
{
	scancode::unknown, // SDL_SCANCODE_UNKNOWN = 0,
	scancode::unknown, // Unassigned = 1
	scancode::unknown, // Unassigned = 2
	scancode::unknown, // Unassigned = 3
	scancode::a, // SDL_SCANCODE_A = 4,
	scancode::b, // SDL_SCANCODE_B = 5,
	scancode::c, // SDL_SCANCODE_C = 6,
	scancode::d, // SDL_SCANCODE_D = 7,
	scancode::e, // SDL_SCANCODE_E = 8,
	scancode::f, // SDL_SCANCODE_F = 9,
	scancode::g, // SDL_SCANCODE_G = 10,
	scancode::h, // SDL_SCANCODE_H = 11,
	scancode::i, // SDL_SCANCODE_I = 12,
	scancode::j, // SDL_SCANCODE_J = 13,
	scancode::k, // SDL_SCANCODE_K = 14,
	scancode::l, // SDL_SCANCODE_L = 15,
	scancode::m, // SDL_SCANCODE_M = 16,
	scancode::n, // SDL_SCANCODE_N = 17,
	scancode::o, // SDL_SCANCODE_O = 18,
	scancode::p, // SDL_SCANCODE_P = 19,
	scancode::q, // SDL_SCANCODE_Q = 20,
	scancode::r, // SDL_SCANCODE_R = 21,
	scancode::s, // SDL_SCANCODE_S = 22,
	scancode::t, // SDL_SCANCODE_T = 23,
	scancode::u, // SDL_SCANCODE_U = 24,
	scancode::v, // SDL_SCANCODE_V = 25,
	scancode::w, // SDL_SCANCODE_W = 26,
	scancode::x, // SDL_SCANCODE_X = 27,
	scancode::y, // SDL_SCANCODE_Y = 28,
	scancode::z, // SDL_SCANCODE_Z = 29,
	scancode::one, // SDL_SCANCODE_1 = 30,
	scancode::two, // SDL_SCANCODE_2 = 31,
	scancode::three, // SDL_SCANCODE_3 = 32,
	scancode::four, // SDL_SCANCODE_4 = 33,
	scancode::five, // SDL_SCANCODE_5 = 34,
	scancode::six, // SDL_SCANCODE_6 = 35,
	scancode::seven, // SDL_SCANCODE_7 = 36,
	scancode::eight, // SDL_SCANCODE_8 = 37,
	scancode::nine, // SDL_SCANCODE_9 = 38,
	scancode::zero, // SDL_SCANCODE_0 = 39,
	scancode::enter, // SDL_SCANCODE_RETURN = 40,
	scancode::escape, // SDL_SCANCODE_ESCAPE = 41,
	scancode::backspace, // SDL_SCANCODE_BACKSPACE = 42,
	scancode::tab, // SDL_SCANCODE_TAB = 43,
	scancode::space, // SDL_SCANCODE_SPACE = 44,
	scancode::minus, // SDL_SCANCODE_MINUS = 45,
	scancode::equal, // SDL_SCANCODE_EQUALS = 46,
	scancode::left_brace, // SDL_SCANCODE_LEFTBRACKET = 47,
	scancode::right_brace, // SDL_SCANCODE_RIGHTBRACKET = 48,
	scancode::backslash, // SDL_SCANCODE_BACKSLASH = 49,
	scancode::non_us_hash, // SDL_SCANCODE_NONUSHASH = 50,
	scancode::semicolon, // SDL_SCANCODE_SEMICOLON = 51,
	scancode::apostrophe, // SDL_SCANCODE_APOSTROPHE = 52,
	scancode::grave, // SDL_SCANCODE_GRAVE = 53,
	scancode::comma, // SDL_SCANCODE_COMMA = 54,
	scancode::dot, // SDL_SCANCODE_PERIOD = 55,
	scancode::slash, // SDL_SCANCODE_SLASH = 56,
	scancode::caps_lock, // SDL_SCANCODE_CAPSLOCK = 57,
	scancode::f1, // SDL_SCANCODE_F1 = 58,
	scancode::f2, // SDL_SCANCODE_F2 = 59,
	scancode::f3, // SDL_SCANCODE_F3 = 60,
	scancode::f4, // SDL_SCANCODE_F4 = 61,
	scancode::f5, // SDL_SCANCODE_F5 = 62,
	scancode::f6, // SDL_SCANCODE_F6 = 63,
	scancode::f7, // SDL_SCANCODE_F7 = 64,
	scancode::f8, // SDL_SCANCODE_F8 = 65,
	scancode::f9, // SDL_SCANCODE_F9 = 66,
	scancode::f10, // SDL_SCANCODE_F10 = 67,
	scancode::f11, // SDL_SCANCODE_F11 = 68,
	scancode::f12, // SDL_SCANCODE_F12 = 69,
	scancode::print_screen, // SDL_SCANCODE_PRINTSCREEN = 70,
	scancode::scroll_lock, // SDL_SCANCODE_SCROLLLOCK = 71,
	scancode::pause, // SDL_SCANCODE_PAUSE = 72,
	scancode::insert, // SDL_SCANCODE_INSERT = 73,
	scancode::home, // SDL_SCANCODE_HOME = 74,
	scancode::page_up, // SDL_SCANCODE_PAGEUP = 75,
	scancode::del, // SDL_SCANCODE_DELETE = 76,
	scancode::end, // SDL_SCANCODE_END = 77,
	scancode::page_down, // SDL_SCANCODE_PAGEDOWN = 78,
	scancode::right, // SDL_SCANCODE_RIGHT = 79,
	scancode::left, // SDL_SCANCODE_LEFT = 80,
	scancode::down, // SDL_SCANCODE_DOWN = 81,
	scancode::up, // SDL_SCANCODE_UP = 82,
	scancode::num_lock, // SDL_SCANCODE_NUMLOCKCLEAR = 83,
	scancode::kp_slash, // SDL_SCANCODE_KP_DIVIDE = 84,
	scancode::kp_asterisk, // SDL_SCANCODE_KP_MULTIPLY = 85,
	scancode::kp_minus, // SDL_SCANCODE_KP_MINUS = 86,
	scancode::kp_plus, // SDL_SCANCODE_KP_PLUS = 87,
	scancode::kp_enter, // SDL_SCANCODE_KP_ENTER = 88,
	scancode::kp_1, // SDL_SCANCODE_KP_1 = 89,
	scancode::kp_2, // SDL_SCANCODE_KP_2 = 90,
	scancode::kp_3, // SDL_SCANCODE_KP_3 = 91,
	scancode::kp_4, // SDL_SCANCODE_KP_4 = 92,
	scancode::kp_5, // SDL_SCANCODE_KP_5 = 93,
	scancode::kp_6, // SDL_SCANCODE_KP_6 = 94,
	scancode::kp_7, // SDL_SCANCODE_KP_7 = 95,
	scancode::kp_8, // SDL_SCANCODE_KP_8 = 96,
	scancode::kp_9, // SDL_SCANCODE_KP_9 = 97,
	scancode::kp_0, // SDL_SCANCODE_KP_0 = 98,
	scancode::kp_dot, // SDL_SCANCODE_KP_PERIOD = 99,
	scancode::non_us_backslash, // SDL_SCANCODE_NONUSBACKSLASH = 100,
	scancode::application, // SDL_SCANCODE_APPLICATION = 101,
	scancode::power, // SDL_SCANCODE_POWER = 102,
	scancode::kp_equal, // SDL_SCANCODE_KP_EQUALS = 103,
	scancode::f13, // SDL_SCANCODE_F13 = 104,
	scancode::f14, // SDL_SCANCODE_F14 = 105,
	scancode::f15, // SDL_SCANCODE_F15 = 106,
	scancode::f16, // SDL_SCANCODE_F16 = 107,
	scancode::f17, // SDL_SCANCODE_F17 = 108,
	scancode::f18, // SDL_SCANCODE_F18 = 109,
	scancode::f19, // SDL_SCANCODE_F19 = 110,
	scancode::f20, // SDL_SCANCODE_F20 = 111,
	scancode::f21, // SDL_SCANCODE_F21 = 112,
	scancode::f22, // SDL_SCANCODE_F22 = 113,
	scancode::f23, // SDL_SCANCODE_F23 = 114,
	scancode::f24, // SDL_SCANCODE_F24 = 115,
	scancode::execute, // SDL_SCANCODE_EXECUTE = 116,
	scancode::help, // SDL_SCANCODE_HELP = 117,
	scancode::menu, // SDL_SCANCODE_MENU = 118,
	scancode::select, // SDL_SCANCODE_SELECT = 119,
	scancode::stop, // SDL_SCANCODE_STOP = 120,
	scancode::again, // SDL_SCANCODE_AGAIN = 121,
	scancode::undo, // SDL_SCANCODE_UNDO = 122,
	scancode::cut, // SDL_SCANCODE_CUT = 123,
	scancode::copy, // SDL_SCANCODE_COPY = 124,
	scancode::paste, // SDL_SCANCODE_PASTE = 125,
	scancode::find, // SDL_SCANCODE_FIND = 126,
	scancode::mute, // SDL_SCANCODE_MUTE = 127,
	scancode::volume_up, // SDL_SCANCODE_VOLUMEUP = 128,
	scancode::volume_down, // SDL_SCANCODE_VOLUMEDOWN = 129,
	scancode::locking_caps_lock, // Unassigned = 130
	scancode::locking_num_lock, // Unassigned = 131
	scancode::locking_scroll_lock, // Unassigned = 132
	scancode::kp_comma, // SDL_SCANCODE_KP_COMMA = 133,
	scancode::kp_equal_as400, // SDL_SCANCODE_KP_EQUALSAS400 = 134,
	scancode::international_1, // SDL_SCANCODE_INTERNATIONAL1 = 135,
	scancode::international_2, // SDL_SCANCODE_INTERNATIONAL2 = 136,
	scancode::international_3, // SDL_SCANCODE_INTERNATIONAL3 = 137,
	scancode::international_4, // SDL_SCANCODE_INTERNATIONAL4 = 138,
	scancode::international_5, // SDL_SCANCODE_INTERNATIONAL5 = 139,
	scancode::international_6, // SDL_SCANCODE_INTERNATIONAL6 = 140,
	scancode::international_7, // SDL_SCANCODE_INTERNATIONAL7 = 141,
	scancode::international_8, // SDL_SCANCODE_INTERNATIONAL8 = 142,
	scancode::international_9, // SDL_SCANCODE_INTERNATIONAL9 = 143,
	scancode::lang_1, // SDL_SCANCODE_LANG1 = 144,
	scancode::lang_2, // SDL_SCANCODE_LANG2 = 145,
	scancode::lang_3, // SDL_SCANCODE_LANG3 = 146,
	scancode::lang_4, // SDL_SCANCODE_LANG4 = 147,
	scancode::lang_5, // SDL_SCANCODE_LANG5 = 148,
	scancode::lang_6, // SDL_SCANCODE_LANG6 = 149,
	scancode::lang_7, // SDL_SCANCODE_LANG7 = 150,
	scancode::lang_8, // SDL_SCANCODE_LANG8 = 151,
	scancode::lang_9, // SDL_SCANCODE_LANG9 = 152,
	scancode::alt_erase, // SDL_SCANCODE_ALTERASE = 153,
	scancode::sys_req, // SDL_SCANCODE_SYSREQ = 154,
	scancode::cancel, // SDL_SCANCODE_CANCEL = 155,
	scancode::clear, // SDL_SCANCODE_CLEAR = 156,
	scancode::prior, // SDL_SCANCODE_PRIOR = 157,
	scancode::return_2, // SDL_SCANCODE_RETURN2 = 158,
	scancode::separator, // SDL_SCANCODE_SEPARATOR = 159,
	scancode::_out, // SDL_SCANCODE_OUT = 160,
	scancode::oper, // SDL_SCANCODE_OPER = 161,
	scancode::clear_again, // SDL_SCANCODE_CLEARAGAIN = 162,
	scancode::cr_sel, // SDL_SCANCODE_CRSEL = 163,
	scancode::ex_sel, // SDL_SCANCODE_EXSEL = 164,
	scancode::unknown, // Unassigned = 165
	scancode::unknown, // Unassigned = 166
	scancode::unknown, // Unassigned = 167
	scancode::unknown, // Unassigned = 168
	scancode::unknown, // Unassigned = 169
	scancode::unknown, // Unassigned = 170
	scancode::unknown, // Unassigned = 171
	scancode::unknown, // Unassigned = 172
	scancode::unknown, // Unassigned = 173
	scancode::unknown, // Unassigned = 174
	scancode::unknown, // Unassigned = 175
	scancode::kp_00, // SDL_SCANCODE_KP_00 = 176,
	scancode::kp_000, // SDL_SCANCODE_KP_000 = 177,
	scancode::thousands_separator, // SDL_SCANCODE_THOUSANDSSEPARATOR = 178,
	scancode::decimal_separator, // SDL_SCANCODE_DECIMALSEPARATOR = 179,
	scancode::currency_unit, // SDL_SCANCODE_CURRENCYUNIT = 180,
	scancode::currency_sub_unit, // SDL_SCANCODE_CURRENCYSUBUNIT = 181,
	scancode::kp_left_paren, // SDL_SCANCODE_KP_LEFTPAREN = 182,
	scancode::kp_right_paren, // SDL_SCANCODE_KP_RIGHTPAREN = 183,
	scancode::kp_left_brace, // SDL_SCANCODE_KP_LEFTBRACE = 184,
	scancode::kp_right_brace, // SDL_SCANCODE_KP_RIGHTBRACE = 185,
	scancode::kp_tab, // SDL_SCANCODE_KP_TAB = 186,
	scancode::kp_backspace, // SDL_SCANCODE_KP_BACKSPACE = 187,
	scancode::kp_a, // SDL_SCANCODE_KP_A = 188,
	scancode::kp_b, // SDL_SCANCODE_KP_B = 189,
	scancode::kp_c, // SDL_SCANCODE_KP_C = 190,
	scancode::kp_d, // SDL_SCANCODE_KP_D = 191,
	scancode::kp_e, // SDL_SCANCODE_KP_E = 192,
	scancode::kp_f, // SDL_SCANCODE_KP_F = 193,
	scancode::kp_xor, // SDL_SCANCODE_KP_XOR = 194,
	scancode::kp_power, // SDL_SCANCODE_KP_POWER = 195,
	scancode::kp_percent, // SDL_SCANCODE_KP_PERCENT = 196,
	scancode::kp_less, // SDL_SCANCODE_KP_LESS = 197,
	scancode::kp_greater, // SDL_SCANCODE_KP_GREATER = 198,
	scancode::kp_ampersand, // SDL_SCANCODE_KP_AMPERSAND = 199,
	scancode::kp_double_ampersand, // SDL_SCANCODE_KP_DBLAMPERSAND = 200,
	scancode::kp_vertical_bar, // SDL_SCANCODE_KP_VERTICALBAR = 201,
	scancode::kp_double_vertical_bar, // SDL_SCANCODE_KP_DBLVERTICALBAR = 202,
	scancode::kp_colon, // SDL_SCANCODE_KP_COLON = 203,
	scancode::kp_hash, // SDL_SCANCODE_KP_HASH = 204,
	scancode::kp_space, // SDL_SCANCODE_KP_SPACE = 205,
	scancode::kp_at, // SDL_SCANCODE_KP_AT = 206,
	scancode::kp_exclam, // SDL_SCANCODE_KP_EXCLAM = 207,
	scancode::kp_mem_store, // SDL_SCANCODE_KP_MEMSTORE = 208,
	scancode::kp_mem_recall, // SDL_SCANCODE_KP_MEMRECALL = 209,
	scancode::kp_mem_clear, // SDL_SCANCODE_KP_MEMCLEAR = 210,
	scancode::kp_mem_add, // SDL_SCANCODE_KP_MEMADD = 211,
	scancode::kp_mem_subtract, // SDL_SCANCODE_KP_MEMSUBTRACT = 212,
	scancode::kp_mem_multiply, // SDL_SCANCODE_KP_MEMMULTIPLY = 213,
	scancode::kp_mem_divide, // SDL_SCANCODE_KP_MEMDIVIDE = 214,
	scancode::kp_plus_minus, // SDL_SCANCODE_KP_PLUSMINUS = 215,
	scancode::kp_clear, // SDL_SCANCODE_KP_CLEAR = 216,
	scancode::kp_clear_entry, // SDL_SCANCODE_KP_CLEARENTRY = 217,
	scancode::kp_binary, // SDL_SCANCODE_KP_BINARY = 218,
	scancode::kp_octal, // SDL_SCANCODE_KP_OCTAL = 219,
	scancode::kp_decimal, // SDL_SCANCODE_KP_DECIMAL = 220,
	scancode::kp_hexadecimal, // SDL_SCANCODE_KP_HEXADECIMAL = 221,
	scancode::unknown, // Unassigned = 222
	scancode::unknown, // Unassigned = 223
	scancode::left_ctrl, // SDL_SCANCODE_LCTRL = 224,
	scancode::left_shift, // SDL_SCANCODE_LSHIFT = 225,
	scancode::left_alt, // SDL_SCANCODE_LALT = 226,
	scancode::left_gui, // SDL_SCANCODE_LGUI = 227,
	scancode::right_ctrl, // SDL_SCANCODE_RCTRL = 228,
	scancode::right_shift, // SDL_SCANCODE_RSHIFT = 229,
	scancode::right_alt, // SDL_SCANCODE_RALT = 230,
	scancode::right_gui, // SDL_SCANCODE_RGUI = 231,
	scancode::unknown, // Unassigned = 232
	scancode::unknown, // Unassigned = 233
	scancode::unknown, // Unassigned = 234
	scancode::unknown, // Unassigned = 235
	scancode::unknown, // Unassigned = 236
	scancode::unknown, // Unassigned = 237
	scancode::unknown, // Unassigned = 238
	scancode::unknown, // Unassigned = 239
	scancode::unknown, // Unassigned = 240
	scancode::unknown, // Unassigned = 241
	scancode::unknown, // Unassigned = 242
	scancode::unknown, // Unassigned = 243
	scancode::unknown, // Unassigned = 244
	scancode::unknown, // Unassigned = 245
	scancode::unknown, // Unassigned = 246
	scancode::unknown, // Unassigned = 247
	scancode::unknown, // Unassigned = 248
	scancode::unknown, // Unassigned = 249
	scancode::unknown, // Unassigned = 250
	scancode::unknown, // Unassigned = 251
	scancode::unknown, // Unassigned = 252
	scancode::unknown, // Unassigned = 253
	scancode::unknown, // Unassigned = 254
	scancode::unknown, // Unassigned = 255
	scancode::unknown, // Unassigned = 256
	scancode::mode, // SDL_SCANCODE_MODE = 257,
	scancode::audio_next, // SDL_SCANCODE_AUDIONEXT = 258,
	scancode::audio_prev, // SDL_SCANCODE_AUDIOPREV = 259,
	scancode::audio_stop, // SDL_SCANCODE_AUDIOSTOP = 260,
	scancode::audio_play, // SDL_SCANCODE_AUDIOPLAY = 261,
	scancode::audio_mute, // SDL_SCANCODE_AUDIOMUTE = 262,
	scancode::media_select, // SDL_SCANCODE_MEDIASELECT = 263,
	scancode::www, // SDL_SCANCODE_WWW = 264,
	scancode::mail, // SDL_SCANCODE_MAIL = 265,
	scancode::calculator, // SDL_SCANCODE_CALCULATOR = 266,
	scancode::computer, // SDL_SCANCODE_COMPUTER = 267,
	scancode::ac_search, // SDL_SCANCODE_AC_SEARCH = 268,
	scancode::ac_home, // SDL_SCANCODE_AC_HOME = 269,
	scancode::ac_back, // SDL_SCANCODE_AC_BACK = 270,
	scancode::ac_forward, // SDL_SCANCODE_AC_FORWARD = 271,
	scancode::ac_stop, // SDL_SCANCODE_AC_STOP = 272,
	scancode::ac_refresh, // SDL_SCANCODE_AC_REFRESH = 273,
	scancode::ac_bookmarks, // SDL_SCANCODE_AC_BOOKMARKS = 274,
	scancode::brightness_down, // SDL_SCANCODE_BRIGHTNESSDOWN = 275,
	scancode::brightness_up, // SDL_SCANCODE_BRIGHTNESSUP = 276,
	scancode::display_switch, // SDL_SCANCODE_DISPLAYSWITCH = 277,
	scancode::kbd_illum_toggle, // SDL_SCANCODE_KBDILLUMTOGGLE = 278,
	scancode::kbd_illum_down, // SDL_SCANCODE_KBDILLUMDOWN = 279,
	scancode::kbd_illum_up, // SDL_SCANCODE_KBDILLUMUP = 280,
	scancode::eject, // SDL_SCANCODE_EJECT = 281,
	scancode::sleep, // SDL_SCANCODE_SLEEP = 282,
	scancode::app_1, // SDL_SCANCODE_APP1 = 283,
	scancode::app_2, // SDL_SCANCODE_APP2 = 284,
	scancode::audio_rewind, // SDL_SCANCODE_AUDIOREWIND = 285,
	scancode::audio_fast_forward, // SDL_SCANCODE_AUDIOFASTFORWARD = 286,
};
