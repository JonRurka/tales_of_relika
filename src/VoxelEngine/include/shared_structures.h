#pragma once

#include <cstdio>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <utility>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <array>
#include <unordered_map>
#include <optional>
#include <set>
#include <algorithm>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/vec4.hpp>

namespace VoxelEngine {

	class ChunkProcessor_private;

	struct ChunkProcessorSettings {
		int TotalBatchGroups;
		int BatchesPerGroup;
		int chunkMeterSizeX;
		int chunkMeterSizeY;
		int chunkMeterSizeZ;
		int voxelsPerMeter;
		char ProgramDirectory[256];
		bool InvertTrianges;

		ChunkProcessorSettings() {
			TotalBatchGroups = 2;
			BatchesPerGroup = 2;
			chunkMeterSizeX = 32;
			chunkMeterSizeY = 32;
			chunkMeterSizeZ = 32;
			voxelsPerMeter = 1;
			ProgramDirectory[0] = '\0';
			InvertTrianges = false;
		}

		ChunkProcessorSettings(int size) {
			TotalBatchGroups = 2;
			BatchesPerGroup = 2;
			chunkMeterSizeX = size;
			chunkMeterSizeY = size;
			chunkMeterSizeZ = size;
			voxelsPerMeter = 1;
			ProgramDirectory[0] = '\0';
			InvertTrianges = false;
		}

		ChunkProcessorSettings(int size, int voxels_per_meter) {
			TotalBatchGroups = 2;
			BatchesPerGroup = 2;
			chunkMeterSizeX = size;
			chunkMeterSizeY = size;
			chunkMeterSizeZ = size;
			voxelsPerMeter = voxels_per_meter;
			ProgramDirectory[0] = '\0';
			InvertTrianges = false;
		}
	};

	struct OutVertex3D {
		float X;
		float Y;
		float Z;
		float W;
	};

	class ChunkRequest {
		friend class ChunkProcessor_private;
	public:
		ChunkRequest() {};
		ChunkRequest(int x, int y, int z) {
			X = x;
			Y = y;
			Z = z;
			m_complete = false;
		}

		int X{ 0 };
		int Y{ 0 };
		int Z{ 0 };

		bool IsComplete() {
			return m_complete;
		}

		int GetNumVertices() {
			return m_numVertex;
		}

		int GetNumTrianges() {
			return m_numTriangles;
		}

		void* Vertex_Ptr() {
			return m_vertices;
		}
		
		void* Normal_Ptr() {
			return m_normals;
		}

		int* Triange_Ptr() {
			return m_triangles;
		}

		void Dispose() {
			delete m_vertices;
			delete m_normals;
			delete m_triangles;

			m_vertices = nullptr;
			m_normals = nullptr;
			m_triangles = nullptr;
		}

	private:

		bool m_complete{ false };
		OutVertex3D* m_vertices = nullptr;
		OutVertex3D* m_normals = nullptr;
		int* m_triangles = nullptr;
		int m_numVertex{ 0 };
		int m_numTriangles{ 0 };
	};

	struct Block {

	};

	struct MeshData {
		glm::vec4* Vertex;
		glm::vec4* Normal;
		int* Triangles;

		int NumVertex;
		int NumTriangles;
	};

	struct Vector3Int {
		int X;
		int Y;
		int Z;

		Vector3Int(int x, int y, int z) {
			X = x;
			Y = y;
			Z = z;
		}
	};

	struct Settings {
	public:

		void setFloat(std::string name, float val) {
			m_float_settings[name] = val;
		}

		void setInt(std::string name, int val) {
			m_int_settings[name] = val;
		}

		void setString(std::string name, std::string val) {
			m_string_settings[name] = val;
		}


		float getFloat(std::string name) {
			return m_float_settings[name];
		}

		int getInt(std::string name) {
			return m_int_settings[name];
		}

		std::string getString(std::string name) {
			return m_string_settings[name];
		}

	private:
		std::map<std::string, float> m_float_settings;
		std::map<std::string, int> m_int_settings;
		std::map<std::string, std::string> m_string_settings;
	};

	struct ChunkSettings {
	public:

		Settings* GetSettings() {
			return &m_settings;
		}

		ChunkSettings() {
			m_settings.setInt("WorkGroups", 16);
			m_settings.setInt("TotalBatchGroups", 1);
			m_settings.setInt("BatchesPerGroup", 1);

			m_settings.setInt("chunkMeterSizeX", 32);
			m_settings.setInt("chunkMeterSizeY", 32);
			m_settings.setInt("chunkMeterSizeZ", 32);
			m_settings.setInt("voxelsPerMeter", 1);
			m_settings.setInt("InvertTrianges", 0);
		}

	private:
		Settings m_settings;
	};

	struct ChunkGenerationOptions {
		std::vector<glm::ivec3> locations;
		/*int X;
		int Y;
		int Z;*/


		Settings GetSettings() {
			return m_settings;
		}

		ChunkGenerationOptions() {
			//X = 0;
			//Y = 0;
			//Z = 0;

		}

	private:
		Settings m_settings;
	};

	struct ChunkRenderOptions {
		std::vector<glm::ivec3> locations;
		/*int X;
		int Y;
		int Z;*/


		Settings GetSettings() {
			return m_settings;
		}

	private:
		Settings m_settings;
	};

}
