
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


#define _tinygltf tinygltf::

int main(int argc, char** argv)
{

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

	info("scenes:    %d", model.scenes.size());
	info("nodes:     %d", model.nodes.size());
	info("ligths:    %d", model.lights.size());
	info("cameras:   %d", model.cameras.size());
	info("meshes:    %d", model.meshes.size());
	info("materials: %d", model.materials.size());
	info("textures:  %d", model.textures.size());
	info("binary data size: %d", model.buffers[0].data.size());
	info("==================================================");


	g_fileSystem.set_working_dir("d:/src/js3d/assets");

	Mesh rect;

	rect.add_position(vec3(-K1, K1, K0));
	rect.add_position(vec3(K1, K1, K0));
	rect.add_position(vec3(K1, -K1, K0));
	rect.add_position(vec3(-K1, -K1, K0));

	rect.add_uv(vec2(0, 1) * 2.0f);
	rect.add_uv(vec2(1, 1) * 2.0f);
	rect.add_uv(vec2(1, 0) * 2.0f);
	rect.add_uv(vec2(0, 0) * 2.0f);

	rect.add_color(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	rect.add_color(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	rect.add_color(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	rect.add_color(vec4(1.0f, 1.0f, 0.0f, 1.0f));

	rect.add_normal(vec3(0.0f, 0.0f, 1.0f));
	rect.add_normal(vec3(0.0f, 0.0f, 1.0f));
	rect.add_normal(vec3(0.0f, 0.0f, 1.0f));
	rect.add_normal(vec3(0.0f, 0.0f, 1.0f));

	rect.add_tangent(vec4(0.0f, 0.0f, 1.0f, 0.0));
	rect.add_tangent(vec4(0.0f, 0.0f, 1.0f, 0.0));
	rect.add_tangent(vec4(0.0f, 0.0f, 1.0f, 0.0));
	rect.add_tangent(vec4(0.0f, 0.0f, 1.0f, 0.0));

	rect.add_index(0);
	rect.add_index(3);
	rect.add_index(2);
	
	rect.add_index(0);
	rect.add_index(2);
	rect.add_index(1);
	rect.set_bounds(vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f));

	info("js3d v2.0");

	drawSurface_t surf{};
	surf.meshId = 0;


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


	Texture tex1, tex2, tex3;
	Material mat;
	surf.material = &mat;


	int d, n, mr;

	int w, h, nr;
	unsigned char* image;

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

	mat.set_shader(ShaderManager::SHADER_DEFAULT_PBR);
	mat.set_textures(d, nr, mr);

	createMeshCommand_t* cmd = (createMeshCommand_t *) g_displayManager.create_command(RC_CREATE_MESH, sizeof(createMeshCommand_t));
	cmd->r_mesh = (RenderMesh*)g_displayManager.alloc_frame_mem(sizeof(RenderMesh));

	RenderMesh rectMesh(0, rect, &g_displayManager);
	*cmd->r_mesh = rectMesh;

	g_displayManager.run_one_frame();

	while (g_displayManager.is_running())
	{
		drawSurfaceCommand_t* cmd = (drawSurfaceCommand_t*)g_displayManager.create_command(js3d::RC_DRAW_SURF, sizeof(drawSurfaceCommand_t));
		drawSurface_t* dsurf = (drawSurface_t*)g_displayManager.alloc_frame_mem(sizeof(drawSurface_t));
		Material* mat = (Material*)g_displayManager.alloc_frame_mem(sizeof(Material));
		*mat = *surf.material;

		dsurf->material = mat;
		dsurf->elementType = surf.elementType;
		dsurf->stateFlags = surf.stateFlags;

		cmd->drawSurf = dsurf;

		g_displayManager.run_one_frame();
	}

	return 0;
}