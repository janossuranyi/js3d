#include <SDL.h>
#include <GL/glew.h>
#include <vector>
#include "js3d.h"
#include "render_system.h"
#include "render_common.h"
#include "GLState.h"

#define VERTEX_ATTRIB_POSITION_INDEX	(0)
#define VERTEX_ATTRIB_QTANGENT_INDEX	(1)
#define VERTEX_ATTRIB_TEXCOORD_INDEX	(2)
#define VERTEX_ATTRIB_COLOR_INDEX	    (3)

#define VERTEX_ATTRIB_POSITION_OFFSET	(0)
#define VERTEX_ATTRIB_QTANGENT_OFFSET	(8)
#define VERTEX_ATTRIB_TEXCOORD_OFFSET	(16)
#define VERTEX_ATTRIB_COLOR_OFFSET	    (20)
#define CACHE_LINE_SIZE						128


namespace js3d {
    
	void ZeroCacheLine(void* ptr, int offset);

    void GLAPIENTRY OGLDebugOutputCallback(GLenum alSource, GLenum alType, GLuint alID, GLenum alSeverity, GLsizei alLength, const GLchar* apMessage, const void* apUserParam)
    {
        info("Source: %d Type: %d Id: %d Severity: %d '%s'", alSource, alType, alID, alSeverity, apMessage);
    }

	RenderSystem::RenderSystem()
	{
		_initialized = false;
		_sdl_window = nullptr;
		_gl_context = 0;
		_win_w = _win_h = 0;
		_glVersion = 0;
		_running = false;
		_inputtHandler = [](const SDL_Event* e) {};
		_mainFunc = []() {};
		_activeIndexBuffer = 0;
		_activeVertexBuffer = 0;
		_activeVertexLayout = VERTEX_LAYOUT_LAST_ENUM;
		_activeProgram = 0xffff;
		_vao = 0xFFFF;
		_glStateBits = 0;

		_vertexLayouts[0] = {
			sizeof(drawVert_t),
			{
				{VERTEX_ATTRIB_POSITION_INDEX, 4, GL_HALF_FLOAT, GL_FALSE, VERTEX_ATTRIB_POSITION_OFFSET},
				{VERTEX_ATTRIB_QTANGENT_INDEX, 4, GL_SHORT, GL_TRUE, VERTEX_ATTRIB_QTANGENT_OFFSET},
				{VERTEX_ATTRIB_TEXCOORD_INDEX, 2, GL_HALF_FLOAT, GL_FALSE, VERTEX_ATTRIB_TEXCOORD_OFFSET},
				{VERTEX_ATTRIB_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_ATTRIB_COLOR_OFFSET}
			}
		};

		for (int i = 0; i < NUM_FRAME_DATA; ++i)
		{
			_frameData = &_smpFrameData[i];
			_frameData->frameMemory = static_cast<uint8_t*>( Mem_Alloc16(FRAME_DATA_SIZE) );
			_frameData->frameMemoryAllocated.store(0, std::memory_order_relaxed);
		}

		_frameCount = 0;
		_smpFrame = 0;
		_frameData = &_smpFrameData[0];

		swap_command_buffers();

	}

    RenderSystem::~RenderSystem()
	{
        shutdown();
	}

    bool RenderSystem::create_surface(const int w, const int h, const int multisample, const bool fullscreen)
    {
        // Initialise SDL
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) < 0)
        {
            fatal_error("SDL could not initialize video! SDL Error: %s\n", SDL_GetError());
        }

        /*
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

        if (multisample > 0)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample);
        }

        _sdl_window = SDL_CreateWindow("OpenGL surface", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (_sdl_window == NULL)
        {
            fatal_error("Window could not be created! SDL Error: %s", SDL_GetError());
        }

        _gl_context = SDL_GL_CreateContext(_sdl_window);
        if (_gl_context == NULL)
        {
            SDL_DestroyWindow(_sdl_window);
            fatal_error("OpenGL context could not be created! SDL Error: %s", SDL_GetError());
        }


        // Initialize GLEW
        glewExperimental = true; // Needed for core profile
        if (glewInit() != GLEW_OK) {
            SDL_DestroyWindow(_sdl_window);
            fatal_error("Failed to initialize GLEW");
        }


        SDL_GL_SetSwapInterval(1);

        std::string renderer = (char*)glGetString(GL_RENDERER);
        std::string version = (char*)glGetString(GL_VERSION);
        const float gl_version = float(atof(version.c_str()));
        _glVersion = int(gl_version * 100);

        SDL_version ver;

        SDL_GetVersion(&ver);

        info("GL Renderer: %s", renderer.c_str());
        info("GL Version: %s (%.2f)", version.c_str(), gl_version);
        info("SDL version: %d.%d patch %d", (int)ver.major, (int)ver.minor, (int)ver.patch);

        int _w, _h;

        SDL_GetWindowSize(_sdl_window, &_w, &_h);
        glViewport(0, 0, w, h);
        glScissor(0, 0, w, h);

        _win_w = _w;
        _win_h = _h;

        if (GLEW_ARB_debug_output)
        {
            glDebugMessageCallbackARB(&OGLDebugOutputCallback, NULL);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        }
        else
        {
            warning("OGL debug output not supported!");
        }

        // Bind the default Vertex array object
        if (glVersion() <= 330)
        {
            glCreateVertexArrays(1, &_vao);
            glBindVertexArray(_vao);
        }


        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        if (glVersion() >= 430)
        {
			//_activeVertexLayout = VERTEX_LAYOUT_DRAW_VERT;
			for (auto attr : _vertexLayouts[VERTEX_LAYOUT_DRAW_VERT].attribs)
			{
				glVertexAttribFormat(attr.index, attr.size, attr.type, attr.normalized, attr.relOffset);
				glVertexAttribBinding(attr.index, 0);
			}
        }

        glViewport(0, 0, w, h);
        glScissor(0, 0, w, h);
        //glEnable(GL_FRAMEBUFFER_SRGB);

        //glLineWidth(4.0f);

		set_state(0, true);

        _initialized = true;
		_running = true;

		return true;
	}

    void RenderSystem::shutdown()
    {
        if (_initialized) {
            if (_vao != 0xFFFF) glDeleteVertexArrays(1, &_vao);
            SDL_GL_DeleteContext(_gl_context);
            SDL_DestroyWindow(_sdl_window);
        }

		for (int i = 0; i < NUM_FRAME_DATA; ++i)
		{
			Mem_Free(_smpFrameData[i].frameMemory);
		}
    }

	void RenderSystem::set_state(uint64_t stateBits, bool forceGlState)
	{
		uint64_t diff = stateBits ^ _glStateBits;

		if (forceGlState)
		{
			// make sure everything is set all the time, so we
			// can see if our delta checking is screwing up
			diff = 0xFFFFFFFFFFFFFFFF;
		}
		else if (diff == 0)
		{
			return;
		}

		//
		// culling
		//
		if (diff & (GLS_CULL_BITS))//| GLS_MIRROR_VIEW ) )
		{
			switch (stateBits & GLS_CULL_BITS)
			{
			case GLS_CULL_TWOSIDED:
				glDisable(GL_CULL_FACE);
				break;

			case GLS_CULL_BACKSIDED:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;

			case GLS_CULL_FRONTSIDED:
			default:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			}
		}

		//
		// check depthFunc bits
		//
		if (diff & GLS_DEPTHFUNC_BITS)
		{
			switch (stateBits & GLS_DEPTHFUNC_BITS)
			{
			case GLS_DEPTHFUNC_EQUAL:
				glDepthFunc(GL_EQUAL);
				break;
			case GLS_DEPTHFUNC_ALWAYS:
				glDepthFunc(GL_ALWAYS);
				break;
			case GLS_DEPTHFUNC_LESS:
				glDepthFunc(GL_LEQUAL);
				break;
			case GLS_DEPTHFUNC_GREATER:
				glDepthFunc(GL_GEQUAL);
				break;
			}
		}

		//
		// check blend bits
		//
		if (diff & (GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS))
		{
			GLenum srcFactor = GL_ONE;
			GLenum dstFactor = GL_ZERO;

			switch (stateBits & GLS_SRCBLEND_BITS)
			{
			case GLS_SRCBLEND_ZERO:
				srcFactor = GL_ZERO;
				break;
			case GLS_SRCBLEND_ONE:
				srcFactor = GL_ONE;
				break;
			case GLS_SRCBLEND_DST_COLOR:
				srcFactor = GL_DST_COLOR;
				break;
			case GLS_SRCBLEND_ONE_MINUS_DST_COLOR:
				srcFactor = GL_ONE_MINUS_DST_COLOR;
				break;
			case GLS_SRCBLEND_SRC_ALPHA:
				srcFactor = GL_SRC_ALPHA;
				break;
			case GLS_SRCBLEND_ONE_MINUS_SRC_ALPHA:
				srcFactor = GL_ONE_MINUS_SRC_ALPHA;
				break;
			case GLS_SRCBLEND_DST_ALPHA:
				srcFactor = GL_DST_ALPHA;
				break;
			case GLS_SRCBLEND_ONE_MINUS_DST_ALPHA:
				srcFactor = GL_ONE_MINUS_DST_ALPHA;
				break;
			default:
				assert(!"GL_State: invalid src blend state bits\n");
				break;
			}

			switch (stateBits & GLS_DSTBLEND_BITS)
			{
			case GLS_DSTBLEND_ZERO:
				dstFactor = GL_ZERO;
				break;
			case GLS_DSTBLEND_ONE:
				dstFactor = GL_ONE;
				break;
			case GLS_DSTBLEND_SRC_COLOR:
				dstFactor = GL_SRC_COLOR;
				break;
			case GLS_DSTBLEND_ONE_MINUS_SRC_COLOR:
				dstFactor = GL_ONE_MINUS_SRC_COLOR;
				break;
			case GLS_DSTBLEND_SRC_ALPHA:
				dstFactor = GL_SRC_ALPHA;
				break;
			case GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA:
				dstFactor = GL_ONE_MINUS_SRC_ALPHA;
				break;
			case GLS_DSTBLEND_DST_ALPHA:
				dstFactor = GL_DST_ALPHA;
				break;
			case GLS_DSTBLEND_ONE_MINUS_DST_ALPHA:
				dstFactor = GL_ONE_MINUS_DST_ALPHA;
				break;
			default:
				assert(!"GL_State: invalid dst blend state bits\n");
				break;
			}

			// Only actually update GL's blend func if blending is enabled.
			if (srcFactor == GL_ONE && dstFactor == GL_ZERO)
			{
				glDisable(GL_BLEND);
			}
			else
			{
				glEnable(GL_BLEND);
				glBlendFunc(srcFactor, dstFactor);
			}
		}

		//
		// check depthmask
		//
		if (diff & GLS_DEPTHMASK)
		{
			if (stateBits & GLS_DEPTHMASK)
			{
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDepthMask(GL_TRUE);
			}
		}

		//
		// check colormask
		//
		if (diff & (GLS_REDMASK | GLS_GREENMASK | GLS_BLUEMASK | GLS_ALPHAMASK))
		{
			GLboolean r = (stateBits & GLS_REDMASK) ? GL_FALSE : GL_TRUE;
			GLboolean g = (stateBits & GLS_GREENMASK) ? GL_FALSE : GL_TRUE;
			GLboolean b = (stateBits & GLS_BLUEMASK) ? GL_FALSE : GL_TRUE;
			GLboolean a = (stateBits & GLS_ALPHAMASK) ? GL_FALSE : GL_TRUE;
			glColorMask(r, g, b, a);
		}

		//
		// fill/line mode
		//
		if (diff & GLS_POLYMODE_LINE)
		{
			if (stateBits & GLS_POLYMODE_LINE)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		//
		// polygon offset
		//
		if (diff & GLS_POLYGON_OFFSET)
		{
			if (stateBits & GLS_POLYGON_OFFSET)
			{
				glPolygonOffset(_polyOfsScale, _polyOfsBias);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glEnable(GL_POLYGON_OFFSET_LINE);
			}
			else
			{
				glDisable(GL_POLYGON_OFFSET_FILL);
				glDisable(GL_POLYGON_OFFSET_LINE);
			}
		}

		//
		// stencil
		//
		if (diff & (GLS_STENCIL_FUNC_BITS | GLS_STENCIL_OP_BITS))
		{
			if ((stateBits & (GLS_STENCIL_FUNC_BITS | GLS_STENCIL_OP_BITS)) != 0)
			{
				glEnable(GL_STENCIL_TEST);
			}
			else
			{
				glDisable(GL_STENCIL_TEST);
			}
		}
		if (diff & (GLS_STENCIL_FUNC_BITS | GLS_STENCIL_FUNC_REF_BITS | GLS_STENCIL_FUNC_MASK_BITS))
		{
			GLuint ref = GLuint((stateBits & GLS_STENCIL_FUNC_REF_BITS) >> GLS_STENCIL_FUNC_REF_SHIFT);
			GLuint mask = GLuint((stateBits & GLS_STENCIL_FUNC_MASK_BITS) >> GLS_STENCIL_FUNC_MASK_SHIFT);
			GLenum func = 0;

			switch (stateBits & GLS_STENCIL_FUNC_BITS)
			{
			case GLS_STENCIL_FUNC_NEVER:
				func = GL_NEVER;
				break;
			case GLS_STENCIL_FUNC_LESS:
				func = GL_LESS;
				break;
			case GLS_STENCIL_FUNC_EQUAL:
				func = GL_EQUAL;
				break;
			case GLS_STENCIL_FUNC_LEQUAL:
				func = GL_LEQUAL;
				break;
			case GLS_STENCIL_FUNC_GREATER:
				func = GL_GREATER;
				break;
			case GLS_STENCIL_FUNC_NOTEQUAL:
				func = GL_NOTEQUAL;
				break;
			case GLS_STENCIL_FUNC_GEQUAL:
				func = GL_GEQUAL;
				break;
			case GLS_STENCIL_FUNC_ALWAYS:
				func = GL_ALWAYS;
				break;
			}
			glStencilFunc(func, ref, mask);
		}
		if (diff & (GLS_STENCIL_OP_FAIL_BITS | GLS_STENCIL_OP_ZFAIL_BITS | GLS_STENCIL_OP_PASS_BITS))
		{
			GLenum sFail = 0;
			GLenum zFail = 0;
			GLenum pass = 0;

			switch (stateBits & GLS_STENCIL_OP_FAIL_BITS)
			{
			case GLS_STENCIL_OP_FAIL_KEEP:
				sFail = GL_KEEP;
				break;
			case GLS_STENCIL_OP_FAIL_ZERO:
				sFail = GL_ZERO;
				break;
			case GLS_STENCIL_OP_FAIL_REPLACE:
				sFail = GL_REPLACE;
				break;
			case GLS_STENCIL_OP_FAIL_INCR:
				sFail = GL_INCR;
				break;
			case GLS_STENCIL_OP_FAIL_DECR:
				sFail = GL_DECR;
				break;
			case GLS_STENCIL_OP_FAIL_INVERT:
				sFail = GL_INVERT;
				break;
			case GLS_STENCIL_OP_FAIL_INCR_WRAP:
				sFail = GL_INCR_WRAP;
				break;
			case GLS_STENCIL_OP_FAIL_DECR_WRAP:
				sFail = GL_DECR_WRAP;
				break;
			}
			switch (stateBits & GLS_STENCIL_OP_ZFAIL_BITS)
			{
			case GLS_STENCIL_OP_ZFAIL_KEEP:
				zFail = GL_KEEP;
				break;
			case GLS_STENCIL_OP_ZFAIL_ZERO:
				zFail = GL_ZERO;
				break;
			case GLS_STENCIL_OP_ZFAIL_REPLACE:
				zFail = GL_REPLACE;
				break;
			case GLS_STENCIL_OP_ZFAIL_INCR:
				zFail = GL_INCR;
				break;
			case GLS_STENCIL_OP_ZFAIL_DECR:
				zFail = GL_DECR;
				break;
			case GLS_STENCIL_OP_ZFAIL_INVERT:
				zFail = GL_INVERT;
				break;
			case GLS_STENCIL_OP_ZFAIL_INCR_WRAP:
				zFail = GL_INCR_WRAP;
				break;
			case GLS_STENCIL_OP_ZFAIL_DECR_WRAP:
				zFail = GL_DECR_WRAP;
				break;
			}
			switch (stateBits & GLS_STENCIL_OP_PASS_BITS)
			{
			case GLS_STENCIL_OP_PASS_KEEP:
				pass = GL_KEEP;
				break;
			case GLS_STENCIL_OP_PASS_ZERO:
				pass = GL_ZERO;
				break;
			case GLS_STENCIL_OP_PASS_REPLACE:
				pass = GL_REPLACE;
				break;
			case GLS_STENCIL_OP_PASS_INCR:
				pass = GL_INCR;
				break;
			case GLS_STENCIL_OP_PASS_DECR:
				pass = GL_DECR;
				break;
			case GLS_STENCIL_OP_PASS_INVERT:
				pass = GL_INVERT;
				break;
			case GLS_STENCIL_OP_PASS_INCR_WRAP:
				pass = GL_INCR_WRAP;
				break;
			case GLS_STENCIL_OP_PASS_DECR_WRAP:
				pass = GL_DECR_WRAP;
				break;
			}
			glStencilOp(sFail, zFail, pass);
		}

		_glStateBits = stateBits;
	}

    void RenderSystem::poll_input()
    {
        SDL_Event e;

        while (SDL_PollEvent(&e) != SDL_FALSE)
        {
            if (e.type == SDL_QUIT)
            {
                _running = false;
            }
            else
            {
                _inputtHandler(&e);
            }
        }
    }

    void RenderSystem::post_quit_message()
    {
        _running = false;
    }

	void RenderSystem::draw_frame(const emptyCommand_t* cmds)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		for (; cmds; cmds = (emptyCommand_t*)cmds->next)
		{
			switch (cmds->commandId)
			{
			case RC_NOP:
				continue;
			case RC_CREATE_MESH:
				create_mesh((createMeshCommand_t*)cmds);
				break;
			case RC_DRAW_SURF:
				drawSurfaceCommand_t* cmd = (drawSurfaceCommand_t*)cmds;
				draw_surface(*cmd->drawSurf);
				break;
			}
		}

		glFinish();

		SDL_GL_SwapWindow(_sdl_window);
		++_frameCount;

		poll_input();
	}

    void RenderSystem::set_viewport(int x, int y, int w, int h)
    {
        glViewport(x, y, w, h);
    }

    void RenderSystem::set_scissor(int x, int y, int w, int h)
    {
        glScissor(x, y, w, h);
    }

    void RenderSystem::draw_surface(const drawSurface_t& surf)
    {
        if (!_initialized) return;
        // set material
        // ...

		RenderMesh& mesh = _render_meshes[surf.meshId];

		uint32_t indexOffset, indexSize, vertexOffset, vertexSize;
		bool isStatic;
		g_vertexCache.decode_handle(mesh.indexHandle(), indexOffset, indexSize, isStatic);
		g_vertexCache.decode_handle(mesh.vertexHandle(), vertexOffset, vertexSize, isStatic);

		VertexBuffer const* vertCacheBuf;

		if (isStatic)
		{
			vertCacheBuf = &g_vertexCache._static_cache.vertexBuffer;
		}
		else
		{
			vertCacheBuf = &g_vertexCache._frame_data[g_vertexCache._listNum].vertexBuffer;
		}
			

        if (glVersion() < 430)
        {
            //glBindVertexArray(vao);
			if (_activeVertexLayout != surf.vertexLayout || _activeVertexBuffer != vertCacheBuf->bufferId()) {
                _activeVertexBuffer = vertCacheBuf->bufferId();
				_activeVertexLayout = surf.vertexLayout;
				vertCacheBuf->bind();

				const GLuint stride = _vertexLayouts[_activeVertexLayout].stride;
				for (auto& attr : _vertexLayouts[_activeVertexLayout].attribs)
				{
					glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, stride, (void*)attr.relOffset);
				}
            }
        }
        else if (_activeVertexLayout != surf.vertexLayout || _activeVertexBuffer != vertCacheBuf->bufferId())
        {
            glBindVertexBuffer(0, vertCacheBuf->bufferId(), 0, _vertexLayouts[surf.vertexLayout].stride);
			_activeVertexBuffer = vertCacheBuf->bufferId();
			_activeVertexLayout = surf.vertexLayout;
        }

        if (_activeIndexBuffer != g_vertexCache._static_cache.indexBuffer.bufferId())
        {
            _activeIndexBuffer = g_vertexCache._static_cache.indexBuffer.bufferId();
			g_vertexCache._static_cache.indexBuffer.bind();
        }


        GLuint elemType = GL_TRIANGLES;
        switch (surf.elementType)
        {
        case eDrawElementType::LINES:
            elemType = GL_LINES;
            break;
        case eDrawElementType::POINT:
            elemType = GL_POINTS;
            break;
        default:
            elemType = GL_TRIANGLES;
        }

		Material* surfMaterial = surf.material;
		if (_activeProgram != surfMaterial->shader())
		{
			_activeProgram = surfMaterial->shader();
			const ShaderProgram& prg = g_shaderManager.get_program(_activeProgram);

			prg.use();
			prg.set_sampler_unit(0, 0);
			prg.set_sampler_unit(1, 1);
			prg.set_sampler_unit(2, 2);
		}

		if (_tmus[0].texId != surfMaterial->get_diffuse_texture())
		{
			_tmus[0].texId = surfMaterial->get_diffuse_texture();
			g_textureManager.bind(surfMaterial->get_diffuse_texture(), 0);
		}
		if (_tmus[1].texId != surfMaterial->get_metallic_roughness_texture())
		{
			_tmus[1].texId = surfMaterial->get_metallic_roughness_texture();
			g_textureManager.bind(surfMaterial->get_metallic_roughness_texture(), 1);
		}
		if (_tmus[2].texId != surfMaterial->get_normal_texture())
		{
			_tmus[2].texId = surfMaterial->get_normal_texture();
			g_textureManager.bind(surfMaterial->get_normal_texture(), 2);
		}

		set_state(surf.stateFlags, false);

        glDrawElementsBaseVertex(
            elemType,
            mesh.numIndices(),
            GL_UNSIGNED_SHORT,
            (void*)indexOffset,
            GLint(vertexOffset / sizeof(drawVert_t)));
    }

	void RenderSystem::create_mesh(createMeshCommand_t* cmd)
	{
		RenderMesh newMesh, *rm = cmd->r_mesh;
		
		for (auto& m : _render_meshes) 
		{
			if (m.id() == rm->id()) return;
		}

		if (rm->elementArray() && rm->vertexArray())
		{
			newMesh.set_id(rm->id());
			newMesh.set_vertex_handle(g_vertexCache.alloc_static_vertex(rm->vertexArray(), rm->numVertices()));
			newMesh.set_index_handle(g_vertexCache.alloc_static_index(rm->elementArray(), rm->numIndices()));
			newMesh.set_index_num(rm->numIndices());
			newMesh.set_vertex_num(rm->numVertices());

			if (_render_meshes.size() <= newMesh.id())
			{
				_render_meshes.resize(_render_meshes.size() + newMesh.id() + 1);
			}

			_render_meshes[newMesh.id()] = std::move(newMesh);
		}
	}

	void* RenderSystem::create_command(renderCommand_t rc, unsigned int bytes)
	{

		emptyCommand_t* cmd = reinterpret_cast<emptyCommand_t*>(alloc_frame_mem(bytes));
		cmd->commandId = rc;
		cmd->next = nullptr;

		_frameData->cmdTail->next = &cmd->commandId;
		_frameData->cmdTail = cmd;

		return cmd;
	}

	void* RenderSystem::alloc_frame_mem(unsigned int bytes)
	{
		bytes = (bytes + FRAME_DATA_MEM_ALIGN - 1) & ~(FRAME_DATA_MEM_ALIGN - 1);
		const unsigned offset = _frameData->frameMemoryAllocated.fetch_add(bytes, std::memory_order_relaxed);

		uint8_t *ptr = _frameData->frameMemory + offset;

		for (int offset = 0; offset < bytes; offset += CACHE_LINE_SIZE)
		{
			ZeroCacheLine(ptr, offset);
		}

		return ptr;
	}

	const emptyCommand_t* RenderSystem::swap_command_buffers()
	{

		emptyCommand_t* commandBuffer = _frameData->cmdHead;

		++_smpFrame;

		_frameData = &_smpFrameData[_smpFrame % NUM_FRAME_DATA];
		_frameData->frameMemoryAllocated.store(0, std::memory_order_relaxed);

		_frameData->cmdHead = _frameData->cmdTail = (emptyCommand_t*)alloc_frame_mem(sizeof(emptyCommand_t));
		_frameData->cmdHead->next = nullptr;
		_frameData->cmdHead->commandId = RC_NOP;

		return commandBuffer;
	}

	static inline void ZeroCacheLine(void* ptr, int offset)
	{
		uint8_t* bytePtr = (uint8_t*)((((uintptr_t)(ptr)) + (offset)) & ~(CACHE_LINE_SIZE - 1));
		::memset(bytePtr, 0, CACHE_LINE_SIZE);
	}

    RenderSystem g_renderSystem;
}
