#pragma once

#include "game_engine.h"
#include "dynamic_compute.h"

using namespace DynamicCompute::Compute;

class Test_OpenCL_Scene : public Scene {
public:

protected:
	void Init() override;

	void Update(float dt) override;

private:

	inline static const std::string LOG_LOC{ "TEST_OPENCL_SCENE" };
};