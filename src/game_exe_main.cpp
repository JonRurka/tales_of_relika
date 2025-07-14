#include <iostream>

#include "game_imp.h"

int main()
{
	Logger::Set_Direct(true);

	Game game;
	return game.Run();
}


/*
#include "FontEngineInterfaceBitmap.h"
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi_Backend.h>

#include "PlatformExtensions.h"
#include "ShellFileInterface.h"

#include "Engine/include/Logger.h"
#include "Engine/include/Resources.h"

namespace {
	static Rml::UniquePtr<ShellFileInterface> file_interface;

	bool ProcessKeyDownShortcuts(Rml::Context* context, Rml::Input::KeyIdentifier key, int key_modifier, float native_dp_ratio, bool priority)
	{
		if (!context)
			return true;

		// Result should return true to allow the event to propagate to the next handler.
		bool result = false;

		// This function is intended to be called twice by the backend, before and after submitting the key event to the context. This way we can
		// intercept shortcuts that should take priority over the context, and then handle any shortcuts of lower priority if the context did not
		// intercept it.
		if (priority)
		{
			// Priority shortcuts are handled before submitting the key to the context.

			// Toggle debugger and set dp-ratio using Ctrl +/-/0 keys.
			if (key == Rml::Input::KI_F8)
			{
				Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
			}
			else if (key == Rml::Input::KI_0 && key_modifier & Rml::Input::KM_CTRL)
			{
				context->SetDensityIndependentPixelRatio(native_dp_ratio);
			}
			else if (key == Rml::Input::KI_1 && key_modifier & Rml::Input::KM_CTRL)
			{
				context->SetDensityIndependentPixelRatio(1.f);
			}
			else if ((key == Rml::Input::KI_OEM_MINUS || key == Rml::Input::KI_SUBTRACT) && key_modifier & Rml::Input::KM_CTRL)
			{
				const float new_dp_ratio = Rml::Math::Max(context->GetDensityIndependentPixelRatio() / 1.2f, 0.5f);
				context->SetDensityIndependentPixelRatio(new_dp_ratio);
			}
			else if ((key == Rml::Input::KI_OEM_PLUS || key == Rml::Input::KI_ADD) && key_modifier & Rml::Input::KM_CTRL)
			{
				const float new_dp_ratio = Rml::Math::Min(context->GetDensityIndependentPixelRatio() * 1.2f, 2.5f);
				context->SetDensityIndependentPixelRatio(new_dp_ratio);
			}
			else
			{
				// Propagate the key down event to the context.
				result = true;
			}
		}
		else
		{
			// We arrive here when no priority keys are detected and the key was not consumed by the context. Check for shortcuts of lower priority.
			if (key == Rml::Input::KI_R && key_modifier & Rml::Input::KM_CTRL)
			{
				for (int i = 0; i < context->GetNumDocuments(); i++)
				{
					Rml::ElementDocument* document = context->GetDocument(i);
					const Rml::String& src = document->GetSourceURL();
					if (src.size() > 4 && src.substr(src.size() - 4) == ".rml")
					{
						document->ReloadStyleSheet();
					}
				}
			}
			else
			{
				result = true;
			}
		}

		return result;
	}
}

int main()
{
	Logger::Set_Direct(true);

	Resources* resources = new Resources();

	int window_width = 1024;
	int window_height = 768;

	Rml::String root = Resources::Get_Data_Directory() + "UI\\";
	if (root.empty())
		return false;

	// The shell overrides the default file interface so that absolute paths in RML/RCSS-documents are relative to the 'Samples' directory.
	file_interface = Rml::MakeUnique<ShellFileInterface>(root);
	Rml::SetFileInterface(file_interface.get());

	// Constructs the system and render interfaces, creates a window, and attaches the renderer.
	if (!Backend::Initialize("Tales of Relica bitmap sample", window_width, window_height, true))
	{
		file_interface.reset();
		return -1;
	}

	// Install the custom interfaces constructed by the backend before initializing RmlUi.
	Rml::SetSystemInterface(Backend::GetSystemInterface());
	Rml::SetRenderInterface(Backend::GetRenderInterface());

	// Construct and load the font interface.
	auto font_interface = Rml::MakeUnique<FontEngineInterfaceBitmap>();
	Rml::SetFontEngineInterface(font_interface.get());

	// RmlUi initialisation.
	Rml::Initialise();

	// Create the main RmlUi context.
	Rml::Context* context = Rml::CreateContext("main", Rml::Vector2i(window_width, window_height));
	if (!context)
	{
		Rml::Shutdown();
		Backend::Shutdown();
		file_interface.reset();
		return -1;
	}

	Rml::Debugger::Initialise(context);

	if (!Rml::LoadFontFace("Engine/Fonts/Comfortaa_Regular_22.fnt"))
	{
		Rml::Shutdown();
		Backend::Shutdown();
		file_interface.reset();
		return -1;
	}

	// Load and show the demo document.
	//if (Rml::ElementDocument* document = context->LoadDocument("basic/bitmap_font/data/bitmap_font.rml"))
	if (Rml::ElementDocument* document = context->LoadDocument("bitmap_font.rml"))
	{
		if (auto el = document->GetElementById("title"))
			el->SetInnerRML("Bitmap font");

		document->Show();
	}

	bool running = true;
	while (running)
	{
		running = Backend::ProcessEvents(context, &ProcessKeyDownShortcuts, true);

		context->Update();

		Backend::BeginFrame();
		context->Render();
		Backend::PresentFrame();
	}

	// Shutdown RmlUi.
	Rml::Shutdown();

	Backend::Shutdown();
	file_interface.reset();
}
*/
