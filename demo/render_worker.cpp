#include <cstring>
#include "js3d.h"
#include "render_worker.h"
#include "draw_surf.h"
#include "filesystem.h"

using namespace js3d;

void MyWorker::run()
{
	drawSurfaceCommand_t* cmd = (drawSurfaceCommand_t*)g_renderSystem.create_command(js3d::RC_DRAW_SURF, sizeof(drawSurfaceCommand_t));
	drawSurface_t* dsurf = (drawSurface_t*)g_renderSystem.alloc_frame_mem(sizeof(drawSurface_t));	
	Material* mat = (Material*)g_renderSystem.alloc_frame_mem(sizeof(*mat));
	triSurf_t* tris = (triSurf_t*)g_renderSystem.alloc_frame_mem(sizeof(*tris));

	tris->numVerts = rmesh.numVertices();
	tris->numIndices = rmesh.numIndices();

	tris->vertHandle = g_vertexCache.alloc_vertex(rmesh.vertexArray(), rmesh.numVertices());
	tris->idxHandle = g_vertexCache.alloc_index(rmesh.elementArray(), rmesh.numIndices());

	*mat = *surf.material;

	dsurf->material = mat;
	dsurf->elementType = surf.elementType;
	dsurf->stateFlags = surf.stateFlags;
	dsurf->meshId = -1;
	dsurf->vertexData = tris;

	cmd->drawSurf = dsurf;

}

void MyWorker::init()
{

	rect.add_position(vec3(-K1,  K1, K0));
	rect.add_position(vec3( K1,  K1, K0));
	rect.add_position(vec3( K1, -K1, K0));
	rect.add_position(vec3(-K1, -K1, K0));

	rect.add_uv(vec2(0, 1) * 1.5f);
	rect.add_uv(vec2(1, 1) * 1.5f);
	rect.add_uv(vec2(1, 0) * 1.5f);
	rect.add_uv(vec2(0, 0) * 1.5f);

	rect.add_color(vec4(1.0f));
	rect.add_color(vec4(1.0f));
	rect.add_color(vec4(1.0f));
	rect.add_color(vec4(1.0f));

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

	rmesh.set_mesh(1, rect, false);

	mat.set_shader(ShaderManager::SHADER_DEFAULT_PBR);
	mat.set_textures(t1, t2, t3);

	createMeshCommand_t* cmd = (createMeshCommand_t*)g_renderSystem.create_command(RC_CREATE_MESH, sizeof(createMeshCommand_t));
	cmd->r_mesh = (RenderMesh*)g_renderSystem.alloc_frame_mem(sizeof(RenderMesh));

	RenderMesh rectMesh(0, rect, &g_renderSystem);
	*cmd->r_mesh = rectMesh;


}
