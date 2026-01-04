#include "UI_Engine.h"

#include "Graphics.h"
#include "Logger.h"
#include "Resources.h"

#if !defined(NO_UI)
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
#endif

#include <GLFW/glfw3.h>

#include <span>
#include <vector>

#include "tracy/Tracy.hpp"


bool UI_Engine::Init()
{

#if !defined(NO_UI)
	// Load the OpenGL functions.
	Rml::String renderer_message;
	if (!RmlGL3::Initialize(&renderer_message)) {
		Logger::LogError(LOG_POS("Init"), "Failed to initialize RmlGL3");
		return false;
	}

	//m_system_interface = std::make_shared<SystemInterface_GLFW>();
	//m_render_interface = std::make_shared<RenderInterface_GL3>();

	m_backend = std::make_unique<BackendData>();

	GLFWwindow* window = Graphics::Instance().Get_GLFW_Window();

	m_backend->system_interface.SetWindow(window);
	//m_system_interface->LogMessage(Rml::Log::LT_INFO, renderer_message);
	Logger::LogInfo(LOG_POS("Init"), "%s", renderer_message.c_str());
	m_backend->system_interface.LogMessage(Rml::Log::LT_INFO, renderer_message);

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
	m_backend->render_interface.SetViewport(width, height);

	// Receive num lock and caps lock modifiers for proper handling of numpad inputs in text fields.
	glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

	// Install the custom interfaces constructed by the backend before initializing RmlUi.
	Rml::SetSystemInterface(&m_backend->system_interface);
	Rml::SetRenderInterface(&m_backend->render_interface);

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
		Shutdown();
		return false;
	}

	//Rml::Debugger::Initialise(m_context);

	m_initialized = true;
	Logger::LogInfo(LOG_POS("Init"), "Rml UI initialized successfully.");
#endif
	return true;
}

void UI_Engine::Load_Font(std::string resource_name)
{
#if !defined(NO_UI)
	assert(m_context);
	assert(m_initialized);

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
#else
	Logger::LogError(LOG_POS("Load_Font"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif

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
#if !defined(NO_UI)
	assert(m_context);
	assert(m_initialized);
	return m_documents.contains(name);
#else
	Logger::LogError(LOG_POS("Document_Exists"), "UI operations not supported: NO_UI was defined.");
	assert(false);
	return false;
#endif
}

ElementDocument UI_Engine::Load_Document_Resource(std::string name, std::string resource_name)
{
#if !defined(NO_UI)
	assert(m_context);
	assert(m_initialized);

	if (m_documents.contains(name))
		return m_documents[name];

	assert(Resources::Has_Data_File(resource_name));

	Resources::Asset asset = Resources::Get_Data_Asset(resource_name);
	std::string rel_path = asset.relative_path;

	Rml::ElementDocument* document = m_context->LoadDocument(rel_path);
	assert(document);

	m_documents[name] = document;//std::shared_ptr<Rml::ElementDocument>(document);
	Logger::LogInfo(LOG_POS("Load_Document_File"), "Document file %s loaded.", name.c_str());
	return m_documents[name];
#else
	Logger::LogError(LOG_POS("Load_Document_Resource"), "UI operations not supported: NO_UI was defined.");
	assert(false);
	return m_documents["null"];
#endif
	

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

ElementDocument UI_Engine::Load_Document_File(std::string name, std::string file_path)
{
#if !defined(NO_UI)
	assert(m_context);
	assert(m_initialized);
	if (m_documents.contains(name))
		return m_documents[name];
	Rml::ElementDocument* document = m_context->LoadDocument(file_path);
	assert(document);

	m_documents[name] = document;//std::shared_ptr<Rml::ElementDocument>(document);
	Logger::LogInfo(LOG_POS("Load_Document_File"), "Document file %s loaded", name.c_str());
	return m_documents[name];
#else
	Logger::LogError(LOG_POS("Load_Document_File"), "UI operations not supported: NO_UI was defined.");
	assert(false);
	return m_documents["null"];
#endif
}

ElementDocument UI_Engine::Get_Document(std::string name)
{
#if !defined(NO_UI)
	assert(m_context);
	assert(m_initialized);
	if (!Document_Exists(name)) {
		return nullptr;
	}
	return m_documents[name];
#else
	Logger::LogError(LOG_POS("Get_Document"), "UI operations not supported: NO_UI was defined.");
	assert(false);
	return m_documents["null"];
#endif
}

void UI_Engine::Unload_Document(std::string name)
{
#if !defined(NO_UI)
	assert(m_context);
	assert(m_initialized);
	if (!m_documents.contains(name))
		return;

	Logger::LogDebug(LOG_POS("Unload_Document"), "Unloading UI document: %s", name.c_str());
	m_context->UnloadDocument(m_documents[name]);
	m_documents.erase(name);

#else
	Logger::LogError(LOG_POS("Unload_Document"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif
}

void UI_Engine::Accept_Input(bool val)
{
	m_accept_input = val;
}

void UI_Engine::Display(std::string doc_name)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return;
	assert(m_context);
	if (!Document_Exists(doc_name)) {
		Logger::LogWarning(LOG_POS("Display"), "Unable to display document '%s'.", doc_name.c_str());
		return;
	}

	m_documents[doc_name]->Show();
	Logger::LogDebug(LOG_POS("Display"), "Display document: %s", doc_name.c_str());
#else
	Logger::LogError(LOG_POS("Display"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif
}

void UI_Engine::Update()
{
	ZoneScopedN("Client Rml UI");

#if !defined(NO_UI)

	if (!m_initialized)
		return;
	assert(m_context);

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
	m_backend->render_interface.BeginFrame();
	m_context->Render();
	m_backend->render_interface.EndFrame();

	// Optional, used to mark frames during performance profiling.
	RMLUI_FrameMark;

#endif
	//Logger::LogDebug(LOG_POS("Update"), "update");
}

void UI_Engine::Shutdown()
{
#if !defined(NO_UI)

	m_documents.clear();

	Rml::Shutdown();
	m_backend.reset();
	RmlGL3::Shutdown();
#endif
}

bool UI_Engine::KeyCallback(GLFWwindow*, int glfw_key, int, int glfw_action, int glfw_mods)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return true;
	assert(m_context);

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
#else
	Logger::LogError(LOG_POS("KeyCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
	return false;
#endif
}

void UI_Engine::CharCallback(GLFWwindow*, unsigned int codepoint)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return;
	assert(m_context);
	RmlGLFW::ProcessCharCallback(m_context, codepoint);
#else
	Logger::LogError(LOG_POS("CharCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif
}

void UI_Engine::CursorEnterCallback(GLFWwindow*, int entered)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return;
	assert(m_context);
	RmlGLFW::ProcessCursorEnterCallback(m_context, entered);
#else
	Logger::LogError(LOG_POS("CursorEnterCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif
}

void UI_Engine::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return;
	assert(m_context);
	RmlGLFW::ProcessCursorPosCallback(m_context, window, xpos, ypos, m_glfw_active_modifiers);
#else
	Logger::LogError(LOG_POS("CursorPosCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif
}

bool UI_Engine::MouseButtonCallback(GLFWwindow*, int button, int action, int mods)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return true;
	assert(m_context);

	if (!m_accept_input) {
		return true;
	}

	m_glfw_active_modifiers = mods;
	bool propogated = RmlGLFW::ProcessMouseButtonCallback(m_context, button, action, mods);
	return propogated;
#else
	Logger::LogError(LOG_POS("MouseButtonCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
	return false;
#endif
}

bool UI_Engine::ScrollCallback(GLFWwindow*, double, double yoffset)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return true;
	assert(m_context);

	if (!m_accept_input) {
		return true;
	}

	bool propogated = RmlGLFW::ProcessScrollCallback(m_context, yoffset, m_glfw_active_modifiers);
	return propogated;
#else
	Logger::LogError(LOG_POS("ScrollCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
	return false;
#endif
}

void UI_Engine::FramebufferSizeCallback(GLFWwindow*, int width, int height)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return;
	assert(m_context);
	m_backend->render_interface.SetViewport(width, height);
	RmlGLFW::ProcessFramebufferSizeCallback(m_context, width, height);
#else
	Logger::LogError(LOG_POS("FramebufferSizeCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif
}

void UI_Engine::WindowContentScaleCallback(GLFWwindow*, float xscale, float)
{
#if !defined(NO_UI)
	if (!m_initialized)
		return;
	assert(m_context);
	RmlGLFW::ProcessContentScaleCallback(m_context, xscale);
#else
	Logger::LogError(LOG_POS("WindowContentScaleCallback"), "UI operations not supported: NO_UI was defined.");
	assert(false);
#endif
}


