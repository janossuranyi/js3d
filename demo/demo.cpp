
#include <cstdio>
#include <thread>
#include <vector>
#include <SDL.h>
#include <glm/glm.hpp>
#include "js3d.h"
using namespace js3d;
using namespace glm;

ShaderManager g_sm;

drawVert_t* rectangle = (drawVert_t*)Mem_Alloc16(4*sizeof(drawVert_t));
elementIndex_t* rectangle_idx = (elementIndex_t*)Mem_Alloc16(6 * sizeof(elementIndex_t));

const float K_ONE = 1.0f;
const float K_ZERO = 0.0f;


int main(int argc, char** argv)
{
	rectangle[0].setPosition(vec3(-K_ONE, K_ONE, K_ZERO));
	rectangle[0].setNormal(vec3(K_ONE, K_ZERO, K_ZERO));
	rectangle[0].setTangent(vec4(K_ONE, K_ZERO, K_ZERO, K_ONE));
	rectangle[0].setTextCoord(vec2(K_ZERO, K_ONE));

	rectangle[1].setPosition(vec3(K_ONE, K_ONE, K_ZERO));
	rectangle[1].setNormal(vec3(K_ZERO, K_ONE, K_ZERO));
	rectangle[1].setTangent(vec4(K_ZERO, K_ONE, K_ZERO, K_ONE));
	rectangle[1].setTextCoord(vec2(K_ONE, K_ONE));

	rectangle[2].setPosition(vec3(K_ONE, -K_ONE, K_ZERO));
	rectangle[2].setNormal(vec3(K_ZERO, K_ZERO, K_ONE));
	rectangle[2].setTangent(vec4(K_ZERO, K_ZERO, K_ONE, K_ONE));
	rectangle[2].setTextCoord(vec2(K_ONE, K_ZERO));

	rectangle[3].setPosition(vec3(-K_ONE, -K_ONE, K_ZERO));
	rectangle[3].setNormal(vec3(K_ONE, K_ONE, K_ONE));
	rectangle[3].setTangent(vec4(K_ONE, K_ONE, K_ONE, K_ONE));
	rectangle[3].setTextCoord(vec2(K_ZERO, K_ZERO));

	rectangle_idx[0] = 0;
	rectangle_idx[1] = 3;
	rectangle_idx[2] = 2;
	
	rectangle_idx[3] = 0;	
	rectangle_idx[4] = 2;
	rectangle_idx[5] = 1;
	
	info("js3d v2.0");

	triSurf_t tris{};
	tris.indices = rectangle_idx;
	tris.vertices = rectangle;
	tris.numIndices = 6;
	tris.numVerts = 4;

	drawSurface_t surf{};
	surf.elementType = eDrawElementType::TRIANGLES;
	surf.vertexData = &tris;


	if (!g_displayManager.create_surface(600, 600, 0, false))
	{
		error("Cannot initialize display");
		exit(-1);
	}

	g_vertexCache.init();

	g_displayManager.set_input_handler([](const SDL_Event* e) {
		if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP)
		{
			info("%d key pressed/released", e->key.keysym.sym);
		}
	});

	g_displayManager.set_main_callback([&]() {
		g_displayManager.draw_surface(surf);
	});

	g_sm.init();
	g_sm.use_program(ShaderManager::SHADER_PASSTHROUGH);
	g_displayManager.run();


	return 0;
}