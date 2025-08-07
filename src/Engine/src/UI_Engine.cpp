#include "UI_Engine.h"

#include "Graphics.h"
#include "Logger.h"
#include "Resources.h"

#include "FontEngineInterfaceBitmap.h"
#include "ShellFileInterface.h"
#include <RmlUi/Core.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/StreamMemory.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Debugger.h>
#include "RmlUi_Platform_GLFW.h"
#include "RmlUi_Renderer_GL3.h"
#include <GLFW/glfw3.h>

#include <span>
#include <vector>

bool UI_Engine::Init()
{
	// Load the OpenGL functions.
	Rml::String renderer_message;
	if (!RmlGL3::Initialize(&renderer_message)) {
		Logger::LogError(LOG_POS("Init"), "Failed to initialize RmlGL3");
		return false;
	}

	m_system_interface = new SystemInterface_GLFW();
	m_render_interface = new RenderInterface_GL3();

	GLFWwindow* window = Graphics::Instance().Get_GLFW_Window();

	m_system_interface->SetWindow(window);
	//m_system_interface->LogMessage(Rml::Log::LT_INFO, renderer_message);
	Logger::LogInfo(LOG_POS("Init"), "%s", renderer_message.c_str());
	m_system_interface->LogMessage(Rml::Log::LT_INFO, renderer_message);

	glfwSetCharCallback(window, [](GLFWwindow* p_window, unsigned int codepoint) {
		Instance().CharCallback(p_window, codepoint);
	});

	// TODO: Make event in window class and Graphics class for this instead.
	glfwSetCursorEnterCallback(window, [](GLFWwindow* p_window, int entered) { 
		Instance().CursorEnterCallback(p_window, entered);
	});

	// TODO: Make event in window class and Graphics class for this instead.
	//glfwSetScrollCallback(window, [](GLFWwindow* p_window, double xoffset, double yoffset) {
	//	m_instance->ScrollCallback(p_window, xoffset, yoffset);
	//});

	glfwSetWindowContentScaleCallback(window, [](GLFWwindow* p_window, float xscale, float yscale) { 
		Instance().WindowContentScaleCallback(p_window, xscale, yscale);
	});

	int width = Graphics::Width();
	int height = Graphics::Height();

	// The window size may have been scaled by DPI settings, get the actual pixel size.
	glfwGetFramebufferSize(window, &width, &height);
	m_render_interface->SetViewport(width, height);

	// Receive num lock and caps lock modifiers for proper handling of numpad inputs in text fields.
	glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

	// Install the custom interfaces constructed by the backend before initializing RmlUi.
	Rml::SetSystemInterface(m_system_interface);
	Rml::SetRenderInterface(m_render_interface);

	// The shell overrides the default file interface so that absolute paths in RML/RCSS-documents are relative to the 'Samples' directory.
	//file_interface = Rml::MakeUnique<ShellFileInterface>(root);
	m_ui_data_root = Resources::Get_Data_Directory() + "UI\\";
	Rml::SetFileInterface(new ShellFileInterface(m_ui_data_root));

	// Construct and load the font interface.
	//auto font_interface = Rml::MakeUnique<FontEngineInterfaceBitmap>();
	Rml::SetFontEngineInterface(new FontEngineInterfaceBitmap());

	struct FontFace {
		const char* filename;
		bool fallback_face;
	};
	FontFace font_faces[] = {
		{"LatoLatin-Regular.ttf", false},
		{"LatoLatin-Italic.ttf", false},
		{"LatoLatin-Bold.ttf", false},
		{"LatoLatin-BoldItalic.ttf", false},
		{"NotoEmoji-Regular.ttf", true},
	};
	//for (const FontFace& face : font_faces)
	//	Rml::LoadFontFace(face.filename, face.fallback_face);

	// RmlUi initialisation.
	Rml::Initialise();

	// Create the main RmlUi context.
	m_context = Rml::CreateContext("main", Rml::Vector2i(width, height));
	if (!m_context)
	{
		Rml::Shutdown();
		Shutdown();
		return false;
	}

	//Rml::Debugger::Initialise(m_context);

	m_initialized = true;
	Logger::LogInfo(LOG_POS("Init"), "Rml UI initialized successfully.");

	return true;
}

void UI_Engine::Load_Font(std::string resource_name)
{
	if (!m_context)
		return;
	if (!m_initialized)
		return;

	if (!Resources::Has_Data_File(resource_name)) {
		Logger::LogError(LOG_POS("Load_Font"), "Font asset '%s' not found!", resource_name.c_str());
		return;
	}

	Resources::Asset asset = Resources::Get_Data_Asset(resource_name, false);
	Logger::LogDebug(LOG_POS("Load_Font"), "Font relative: %s", asset.relative_path.c_str());

	if (!Rml::LoadFontFace(asset.relative_path))
	{
		Logger::LogError(LOG_POS("Load_Font"), "Failed to load font asset %s at %s", 
			resource_name.c_str(), asset.relative_path.c_str());
	}

	/*
	std::vector<char> font_bin_vec = Resources::Get_Data_File_Bin(resource_name);

	if (font_bin_vec.size() == 0) {
		Logger::LogError(LOG_POS("Load_Font"), "Font asset '%s' was empty!", resource_name.c_str());
		return;
	}

	Rml::Span<const byte> font_bin_span((Rml::byte*)font_bin_vec.data(), font_bin_vec.size());

	// Load bitmap font
	std::string family = "Comfortaa_Regular_22";
	Rml::Style::FontStyle style = Rml::Style::FontStyle::Normal;
	//if (!Rml::LoadFontFace(font_bin_span, family, style))
	if (!Rml::LoadFontFace("Engine/Fonts/Comfortaa_Regular_22.fnt"))
	{
		Logger::LogError(LOG_POS("Load_Font"), "Failed to load font asset: %s", resource_name.c_str());
	}
	//!Rml::LoadFontFace("basic/bitmap_font/data/Comfortaa_Regular_22.fnt")
	//Comfortaa_Regular_22.fnt
	*/
}

bool UI_Engine::Document_Exists(std::string name)
{
	if (!m_context)
		return false;
	if (!m_initialized)
		return false;
	return m_documents.contains(name);
}

Rml::ElementDocument* UI_Engine::Load_Document_Resource(std::string name, std::string resource_name)
{
	if (!m_context)
		return nullptr;
	if (!m_initialized)
		return nullptr;
	if (m_documents.contains(name))
		return m_documents[name];

	if (!Resources::Has_Data_File(resource_name)) {
		Logger::LogError(LOG_POS("Load_Document_Resource"), "Document asset '%s' not found!", resource_name.c_str());
		return nullptr;
	}

	Resources::Asset asset = Resources::Get_Data_Asset(resource_name);
	std::string rel_path = asset.relative_path;

	Rml::ElementDocument* document = m_context->LoadDocument(rel_path);
	if (!document) {
		Logger::LogError(LOG_POS("Load_Document_File"), "Failed to load document file: %s", name.c_str());
		return nullptr;
	}

	m_documents[name] = document;
	Logger::LogInfo(LOG_POS("Load_Document_File"), "Document file %s loaded.", name.c_str());
	return document;
	

	/*
	std::string file_content = Resources::Get_Data_File_String(resource_name);

	auto stream = Rml::MakeUnique<Rml::StreamMemory>();
	stream->Write(file_content);
	
	Rml::ElementDocument* document = m_context->LoadDocument(stream.get());
	if (!document) {
		Logger::LogError(LOG_POS("Load_Document_Resource"), "Failed to load document asset: %s", resource_name.c_str());
		return nullptr;
	}
	
	m_documents[name] = document;
	Logger::LogInfo(LOG_POS("Load_Document_Resource"), "Document asset %s loaded: %s", name.c_str(), file_content.c_str());
	return document;*/
}

Rml::ElementDocument* UI_Engine::Load_Document_File(std::string name, std::string file_path)
{
	if (!m_context)
		return nullptr;
	if (!m_initialized)
		return nullptr;
	if (m_documents.contains(name))
		return m_documents[name];
	Rml::ElementDocument* document = m_context->LoadDocument(file_path);
	if (!document) {
		Logger::LogError(LOG_POS("Load_Document_File"), "Failed to load document file: %s", name.c_str());
		return nullptr;
	}

	m_documents[name] = document;
	Logger::LogInfo(LOG_POS("Load_Document_File"), "Document file %s loaded", name.c_str());
	return document;
}

Rml::ElementDocument* UI_Engine::Get_Document(std::string name)
{
	if (!Document_Exists(name)) {
		return nullptr;
	}
	return m_documents[name];
}

void UI_Engine::Display(std::string doc_name)
{
	if (!m_context)
		return;
	if (!m_initialized)
		return;
	if (!Document_Exists(doc_name)) {
		Logger::LogWarning(LOG_POS("Display"), "Unable to display document '%s'.", doc_name.c_str());
		return;
	}

	if (auto el = m_documents[doc_name]->GetElementById("title"))
		el->SetInnerRML("Bitmap font");

	m_documents[doc_name]->Show();
	Logger::LogDebug(LOG_POS("Display"), "Display document: %s", doc_name.c_str());
}

void UI_Engine::Update()
{
	if (!m_initialized) {
		return;
	}
	if (!m_context)
		return;

	// The initial window size may have been affected by system DPI settings, apply the actual pixel size and dp-ratio to the context.
	if (m_context_dimensions_dirty)
	{
		m_context_dimensions_dirty = false;

		GLFWwindow* window = Graphics::Instance().Get_GLFW_Window();

		Rml::Vector2i window_size;
		float dp_ratio = 1.f;
		glfwGetFramebufferSize(window, &window_size.x, &window_size.y);
		glfwGetWindowContentScale(window, &dp_ratio, nullptr);

		m_context->SetDimensions(window_size);
		m_context->SetDensityIndependentPixelRatio(dp_ratio);
	}

	m_context->Update();

	//m_render_interface->Clear();
	m_render_interface->BeginFrame();
	m_context->Render();
	m_render_interface->EndFrame();

	// Optional, used to mark frames during performance profiling.
	RMLUI_FrameMark;

	//Logger::LogDebug(LOG_POS("Update"), "update");
}

void UI_Engine::Shutdown()
{
	RmlGL3::Shutdown();
	delete m_system_interface;
	delete m_render_interface;
}

bool UI_Engine::KeyCallback(GLFWwindow*, int glfw_key, int, int glfw_action, int glfw_mods)
{
	if (!m_context)
		return true;
	if (!m_initialized)
		return true;

	if (!m_accept_input) {
		return true;
	}

	m_glfw_active_modifiers = glfw_mods;

	GLFWwindow* window = Graphics::Instance().Get_GLFW_Window();

	bool res = false;

	switch (glfw_action)
	{
	case GLFW_PRESS:
	case GLFW_REPEAT:
	{
		const Rml::Input::KeyIdentifier key = RmlGLFW::ConvertKey(glfw_key);
		const int key_modifier = RmlGLFW::ConvertKeyModifiers(glfw_mods);
		float dp_ratio = 1.f;
		glfwGetWindowContentScale(window, &dp_ratio, nullptr);

		//if (key_down_callback && !key_down_callback(context, key, key_modifier, dp_ratio, true))
		//	break;

		bool propogated = RmlGLFW::ProcessKeyCallback(m_context, glfw_key, glfw_action, glfw_mods);
		res = propogated;
	}
	break;
	case GLFW_RELEASE: 
		RmlGLFW::ProcessKeyCallback(m_context, glfw_key, glfw_action, glfw_mods); 
		break;
	}

	return res;
}

void UI_Engine::CharCallback(GLFWwindow*, unsigned int codepoint)
{
	if (!m_context)
		return;
	if (!m_initialized)
		return;
	RmlGLFW::ProcessCharCallback(m_context, codepoint);
}

void UI_Engine::CursorEnterCallback(GLFWwindow*, int entered)
{
	if (!m_context)
		return;
	if (!m_initialized)
		return;
	RmlGLFW::ProcessCursorEnterCallback(m_context, entered);
}

void UI_Engine::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (!m_context)
		return;
	if (!m_initialized)
		return;
	RmlGLFW::ProcessCursorPosCallback(m_context, window, xpos, ypos, m_glfw_active_modifiers);
}

bool UI_Engine::MouseButtonCallback(GLFWwindow*, int button, int action, int mods)
{
	if (!m_context)
		return true;
	if (!m_initialized)
		return true;

	if (!m_accept_input) {
		return true;
	}

	m_glfw_active_modifiers = mods;
	bool propogated = RmlGLFW::ProcessMouseButtonCallback(m_context, button, action, mods);
	return propogated;
}

bool UI_Engine::ScrollCallback(GLFWwindow*, double, double yoffset)
{
	if (!m_context)
		return true;
	if (!m_initialized)
		return true;

	if (!m_accept_input) {
		return true;
	}

	bool propogated = RmlGLFW::ProcessScrollCallback(m_context, yoffset, m_glfw_active_modifiers);
	return propogated;
}

void UI_Engine::FramebufferSizeCallback(GLFWwindow*, int width, int height)
{
	if (!m_context)
		return;
	if (!m_initialized)
		return;
	m_render_interface->SetViewport(width, height);
	RmlGLFW::ProcessFramebufferSizeCallback(m_context, width, height);
}

void UI_Engine::WindowContentScaleCallback(GLFWwindow*, float xscale, float)
{
	if (!m_context)
		return;
	if (!m_initialized)
		return;
	RmlGLFW::ProcessContentScaleCallback(m_context, xscale);
}


