#pragma once

#include <string>

#include "game_engine.h"

class Game_Resources {
public:

	class Models {
	public:

	};

	class Shaders {
	public:
		class Compute {
		public:

		};

		class Graphics {
		public:
			class Engine {

			};
			inline static const std::string STANDARD_VERT = "graphics.standard.standard.vert";
			inline static const std::string STANDARD_FRAG = "graphics.standard.standard.frag";

			inline static const std::string SKYBOX_VERT = "graphics.skybox.skybox.vert";
			inline static const std::string SKYBOX_FRAG = "graphics.skybox.skybox.frag";
		};
	};

	class Textures {
	public:
		inline static const std::string CONTAINER_DIFFUSE = "container.container2_diffuse.png";
		inline static const std::string CONTAINER_SPECULAR = "container.container2_specular.png";

		inline static const std::string SKYBOX_BACK = "skybox.back.jpg";
		inline static const std::string SKYBOX_BOTTOM = "skybox.bottom.jpg";
		inline static const std::string SKYBOX_FRONT = "skybox.front.jpg";
		inline static const std::string SKYBOX_LEFT = "skybox.left.jpg";
		inline static const std::string SKYBOX_RIGHT = "skybox.right.jpg";
		inline static const std::string SKYBOX_TOP = "skybox.top.jpg";
	};

private:

};