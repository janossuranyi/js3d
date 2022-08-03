
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

using namespace js3d;
using namespace glm;

ShaderManager g_sm;


#define _tinygltf tinygltf::

int main(int argc, char** argv)
{
	g_fileSystem.set_working_dir("d:/src/js3d/assets");

	std::string err, warn;

	_tinygltf Model model;
	_tinygltf TinyGLTF loader;

	err = "none";
	warn = "none";

	loader.LoadASCIIFromFile(&model, &err, &warn, "d:/src/js3d/assets/scene.gltf", 0);

	info("Scene warning: %s, scene error: %s", warn.c_str(), err.c_str());

	info("==================================================");
	info("Scene info");
	info("==================================================");
	info("asset.generator: %s", model.asset.generator.c_str());
	info("asset.version:   %s", model.asset.version.c_str());
	info("extension.used:  %d", model.extensionsUsed.size());
	for (auto e : model.extensionsUsed)
	{
		info("extension: %s", e.c_str());
	}

	info("Number of logical CPU cores: %d", SDL_GetCPUCount());
	info("scenes:    %d", model.scenes.size());
	info("nodes:     %d", model.nodes.size());
	info("ligths:    %d", model.lights.size());
	info("cameras:   %d", model.cameras.size());
	info("meshes:    %d", model.meshes.size());
	info("materials: %d", model.materials.size());
	info("textures:  %d", model.textures.size());
	info("binary data size: %d", model.buffers[0].data.size());
	info("==================================================");



	info("js3d v2.0");



	if (!g_displayManager.create_surface(600, 600, 0, false))
	{
		error("Cannot initialize display");
		exit(-1);
	}

	g_vertexCache.init();
	g_shaderManager.init();

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

	while (g_displayManager.is_running())
	{

		const js3d::emptyCommand_t* cmds = g_displayManager.swap_command_buffers();
		
		worker.start_worker();

		g_displayManager.draw_frame(cmds);


		worker.wait_for_done();

	}

	return 0;
}