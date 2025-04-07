/* Copyright (c) 2019-2021, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <string>

#include "Utilities.h"


namespace Input {

	//class Platform;

	enum class EventSource
	{
		Keyboard,
		Mouse,
		Touchscreen
	};

	class InputEvent
	{
	public:
		InputEvent(EventSource source) :
			source{ source }
		{}

		EventSource get_source() const { return source; }

	private:
		EventSource source;
	};

	enum class KeyCode
	{
		Unknown,
		Space,
		Apostrophe, /* ' */
		Comma,      /* , */
		Minus,      /* - */
		Period,     /* . */
		Slash,      /* / */
		_0,
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		_8,
		_9,
		Semicolon, /* ; */
		Equal,     /* = */
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		LeftBracket,  /* [ */
		Backslash,    /* \ */
		RightBracket, /* ] */
		GraveAccent,  /* ` */
		Escape,
		Enter,
		Tab,
		Backspace,
		Insert,
		DelKey,
		Right,
		Left,
		Down,
		Up,
		PageUp,
		PageDown,
		Home,
		End,
		Back,
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,
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
		KP_0,
		KP_1,
		KP_2,
		KP_3,
		KP_4,
		KP_5,
		KP_6,
		KP_7,
		KP_8,
		KP_9,
		KP_Decimal,
		KP_Divide,
		KP_Multiply,
		KP_Subtract,
		KP_Add,
		KP_Enter,
		KP_Equal,
		LeftShift,
		LeftControl,
		LeftAlt,
		RightShift,
		RightControl,
		RightAlt
	};

	enum class KeyAction
	{
		Down,
		Up,
		Repeat,
		Unknown
	};

	class KeyInputEvent : public InputEvent
	{
	public:
		KeyInputEvent(KeyCode code, KeyAction action) :
			InputEvent{ EventSource::Keyboard },
			code{ code },
			action{ action },
			press_time{ Utilities::Get_Time() }, used{ false }, num_frames{ 0 }
		{
		}

		KeyInputEvent() :
			InputEvent{ EventSource::Keyboard },
			code {KeyCode::Unknown},
			action {KeyAction::Unknown},
			press_time{0}, used{false}, num_frames{0}
		{
		}

		void tick() { num_frames++; }
		void reset() { num_frames = 0; }
		bool is_used() { return used; }

		KeyCode get_code() const { return code; }

		KeyAction get_action() const { return action; }

		int get_num_frames() { return num_frames; }

		std::string to_string() const {
			std::string code_str = std::to_string((int)code);
			std::string action_str = "";
			switch (get_action()) {
			case KeyAction::Down:
				action_str = "Down";
				break;
			case KeyAction::Up:
				action_str = "Up";
				break;
			case KeyAction::Repeat:
				action_str = "Repeat";
				break;
			default:
				action_str = "Unknown";
				break;
			}
			std::string res = "KeyInputEvent(KeyCode: " + code_str + ", Action: " + action_str + ")";
			return res;
		}

		double get_press_time() const { return Utilities::Get_Time() - press_time; }

		void update_action(KeyAction new_action) {action = new_action;}
	private:
		KeyCode code;
		KeyAction action;
		double press_time;
		int num_frames;
		bool used;
	};

	enum class MouseButton
	{
		Left,
		Right,
		Middle,
		Back,
		Forward,
		Unknown
	};

	enum class MouseAction
	{
		Down,
		Up,
		Move,
		Unknown
	};

	class MouseButtonInputEvent : public InputEvent
	{
	public:
		MouseButtonInputEvent(MouseButton button, MouseAction action, float pos_x, float pos_y) :
			InputEvent{ EventSource::Mouse },
			button{ button },
			action{ action },
			pos_x{ pos_x },
			pos_y{ pos_y }
		{}

		MouseButton get_button() const { return button; }

		MouseAction get_action() const { return action; }

		float get_pos_x() const { return pos_x; }

		float get_pos_y() const { return pos_y; }

	private:
		MouseButton button;

		MouseAction action;

		float pos_x;

		float pos_y;
	};

	enum class TouchAction
	{
		Down,
		Up,
		Move,
		Cancel,
		PointerDown,
		PointerUp,
		Unknown
	};

	class TouchInputEvent : public InputEvent
	{
	public:
		TouchInputEvent(int32_t pointer_id, size_t pointer_count, TouchAction action, float pos_x, float pos_y) :
			InputEvent{ EventSource::Touchscreen },
			action{ action },
			pointer_id{ pointer_id },
			touch_points{ touch_points },
			pos_x{ pos_x },
			pos_y{ pos_y }
		{
		}

		TouchAction get_action() { return action; }

		int32_t get_pointer_id() { return pointer_id; }

		size_t get_touch_points() { return touch_points; }

		float get_pos_x() { return pos_x; }

		float get_pos_y() { return pos_y; }

	private:
		TouchAction action;

		int32_t pointer_id{ 0 };

		size_t touch_points{ 0 };

		float pos_x{0};

		float pos_y{0};
	};

	typedef void (*OnWindowErrorActionPtr)(void*, std::string error);
	typedef void (*OnResizeActionPtr)(void*, uint32_t width, uint32_t height);
	typedef void (*OnFocusActionPtr)(void*, int focused);
	typedef void (*OnKeyInputActionPtr)(void*, KeyInputEvent key_event);
	typedef void (*OnMouseButtonInputActionPtr)(void*, MouseButtonInputEvent mouse_event);

}
