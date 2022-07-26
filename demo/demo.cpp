
#include <cstdio>
#include <thread>
#include <vector>
#include <SDL.h>
#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include "js3d.h"
#include "material.h"
#include "texture_manager.h"
#include "render_common.h"
#include "thread.h"
#include "render_worker.h"
#include "gltf_loader.h"

using namespace js3d;
using namespace glm;

ShaderManager g_sm;


#define _tinygltf tinygltf::

int main(int argc, char** argv)
{
	g_fileSystem.set_working_dir("d:/src/js3d/assets");

	std::string err, warn;

	err = "none";
	warn = "none";

	GltfLoader loader;

	loader.load(g_fileSystem.resolve("Steampunk_Dirigible_with_Ship.glb"), err, warn);
	info("Number of logical CPU cores: %d", SDL_GetCPUCount());


	info("js3d v2.0");



	if (!g_renderSystem.create_surface(600, 600, 0, false))
	{
		error("Cannot initialize display");
		exit(-1);
	}

	g_vertexCache.init();
	g_shaderManager.init();

	g_renderSystem.set_input_handler([](const SDL_Event* e) {
		if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP)
		{
			info("%d key pressed/released", e->key.keysym.sym);
			if (e->key.keysym.sym == SDLK_ESCAPE) {
				g_renderSystem.post_quit_message();
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

				g_renderSystem.set_viewport(0, 0, e->window.data1, e->window.data2);
				break;
			}
		}
		});

	MyWorker worker("test-worker");

	int d, n, mr;

	int w, h, nr;
	unsigned char* image;
	Texture tex1, tex2, tex3;

	g_fileSystem.load_image_base("textures/base-white-tile_albedo.png", w, h, n, &image);
	tex1.create_2d_default(w, h, n, image);
	d = g_textureManager.add(tex1);
	g_fileSystem.free_image(image);

	g_fileSystem.load_image_base("textures/base-white-tile_metallic-base-white-tile_roughness.png", w, h, n, &image);
	tex2.create_2d_default(w, h, n, image);
	mr = g_textureManager.add(tex2);
	g_fileSystem.free_image(image);

	g_fileSystem.load_image_base("textures/base-white-tile_normal-ogl.png", w, h, n, &image);
	tex3.create_2d_default(w, h, n, image);
	nr = g_textureManager.add(tex3);
	g_fileSystem.free_image(image);

	worker.t1 = d;
	worker.t2 = mr;
	worker.t3 = nr;

	worker.init();

	while (g_renderSystem.is_running())
	{

		const js3d::emptyCommand_t* cmds = g_renderSystem.swap_command_buffers();
		

		g_vertexCache.begin_backend();
		worker.start_worker();
		g_renderSystem.draw_frame(cmds);


		worker.wait_for_done();

	}

	return 0;
}