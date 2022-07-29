
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

const float K1 = 1.0f;
const float K0 = 0.0f;

namespace js3d {
	FileSystem g_fileSystem;
}


int main(int argc, char** argv)
{
	g_fileSystem.set_working_dir("d:/src/js3d/assets");

	rectangle[0].setPosition(vec4(-K1, K1, K0, K1));
	//rectangle[0].setQTangent(vec4(K1, K0, K0, K1));
	rectangle[0].setTextCoord(vec2(0, 1) / 64.0f);
	rectangle[0].setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));

	rectangle[1].setPosition(vec4(K1, K1, K0, K1));
	//rectangle[1].setQTangent(vec4(K0, K1, K0, K1));
	rectangle[1].setTextCoord(vec2(1, 1) / 64.0f);
	rectangle[1].setColor(vec4(0.0f, 1.0f, 0.0f, 1.0f));

	rectangle[2].setPosition(vec4(K1, -K1, K0, K1));
	//rectangle[2].setQTangent(vec4(K0, K0, K1, K1));
	rectangle[2].setTextCoord(vec2(1, 0) / 64.0f);
	rectangle[2].setColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));

	rectangle[3].setPosition(vec4(-K1, -K1, K0, K1));
	//rectangle[3].setQTangent(vec4(K1, K1, K1, K1));
	rectangle[3].setTextCoord(vec2(0, 0) / 64.0f);
	rectangle[3].setColor(vec4(1.0f, 1.0f, 0.0f, 1.0f));

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
			if (e->key.keysym.sym == SDLK_ESCAPE) {
				g_displayManager.post_quit_message();
			}
		}
		else if (e->type == SDL_WINDOWEVENT)
		{
			switch (e->window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				info("Window %d resized to %dx%d",
					e->window.windowID, e->window.data1,
					e->window.data2);

				g_displayManager.set_viewport(0, 0, e->window.data1, e->window.data2);
				break;
			}
		}
		});

	g_displayManager.set_main_callback([&]() {
		g_displayManager.draw_surface(surf);
	});

	g_sm.init();
	g_sm.use_program(ShaderManager::SHADER_PASSTHROUGH);

	Texture tex1,tex2,tex3;

	int w, h, n;
	unsigned char* image;

	g_fileSystem.load_image_base("textures/base-white-tile_albedo.png", w, h, n, &image);
	tex1.create_2d_default(w, h, n, image);
	surf.albedo = &tex1;
	g_fileSystem.free_image(image);

	g_fileSystem.load_image_base("textures/base-white-tile_metallic-base-white-tile_roughness.png", w, h, n, &image);
	tex2.create_2d_default(w, h, n, image);
	surf.metallic_roughness = &tex2;
	g_fileSystem.free_image(image);

	g_fileSystem.load_image_base("textures/base-white-tile_normal-ogl.png", w, h, n, &image);
	tex3.create_2d_default(w, h, n, image);
	surf.normal = &tex3;
	g_fileSystem.free_image(image);

	surf.shader = &g_sm.get_program(ShaderManager::SHADER_PASSTHROUGH);
	
	g_displayManager.run();


	return 0;
}