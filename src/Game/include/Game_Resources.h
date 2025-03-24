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
		};
	};

	class Textures {
	public:
		inline static const std::string CONTAINER_DIFFUSE = "container.container2_diffuse.png";
		inline static const std::string CONTAINER_SPECULAR = "container.container2_specular.png";
	};

private:

};