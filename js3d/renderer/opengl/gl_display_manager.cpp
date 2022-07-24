#include <SDL.h>
#include <GL/glew.h>
#include "js3d.h"
#include "display_manager.h"
#include "render_common.h"

#define VERTEX_ATTRIB_POSITION_INDEX	(0)
#define VERTEX_ATTRIB_NORMAL_INDEX		(1)
#define VERTEX_ATTRIB_TANGENT_INDEX		(2)
#define VERTEX_ATTRIB_TEXCOORD_INDEX	(3)

#define VERTEX_ATTRIB_POSITION_OFFSET	(0)
#define VERTEX_ATTRIB_NORMAL_OFFSET		(3*4)
#define VERTEX_ATTRIB_TANGENT_OFFSET	(6*4)
#define VERTEX_ATTRIB_TEXCOORD_OFFSET	(10*4)

#define VERTEX_ATTRIB_COMPACT_POSITION_OFFSET	(0)
#define VERTEX_ATTRIB_COMPACT_NORMAL_OFFSET		(12)
#define VERTEX_ATTRIB_COMPACT_TANGENT_OFFSET	(12+6)
#define VERTEX_ATTRIB_COMPACT_TEXCOORD_OFFSET	(18+8)

namespace js3d {
    
    void GLAPIENTRY OGLDebugOutputCallback(GLenum alSource, GLenum alType, GLuint alID, GLenum alSeverity, GLsizei alLength, const GLchar* apMessage, const void* apUserParam)
    {
        info("Source: %d Type: %d Id: %d Severity: %d '%s'", alSource, alType, alID, alSeverity, apMessage);
    }

    DisplayManager::DisplayManager()
	{
		_initialized = false;
        _sdl_window = nullptr;
        _gl_context = 0;
        _win_w = _win_h = 0;
        _glState = 0;
        _glVersion = 0;
        _running = false;
        _inputtHandler = [](const SDL_Event* e) {};
        _mainFunc = []() {};
        _activeIndexBuffer = nullptr;
        _activeVertexBuffer = nullptr;
        _activeVertexLayout = eVertexLayout::UNDEFINED;
	}

    DisplayManager::~DisplayManager()
	{
        shutdown();
	}

    bool DisplayManager::create_surface(const int w, const int h, const int multisample, const bool fullscreen)
    {
        // Initialise SDL
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) < 0)
        {
            fatal_error("SDL could not initialize video! SDL Error: %s\n", SDL_GetError());
        }

        //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

        if (multisample > 0)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample);
        }

        _sdl_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

        glCreateVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        if (glVersion() >= 430)
        {
            glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIB_COMPACT_POSITION_OFFSET);
            glVertexAttribFormat(1, 3, GL_UNSIGNED_SHORT, GL_TRUE, VERTEX_ATTRIB_COMPACT_NORMAL_OFFSET);
            glVertexAttribFormat(2, 4, GL_UNSIGNED_SHORT, GL_TRUE, VERTEX_ATTRIB_COMPACT_TANGENT_OFFSET);
            glVertexAttribFormat(3, 2, GL_UNSIGNED_SHORT, GL_TRUE, VERTEX_ATTRIB_COMPACT_TEXCOORD_OFFSET);

            glVertexAttribBinding(0, 0);
            glVertexAttribBinding(1, 0);
            glVertexAttribBinding(2, 0);
            glVertexAttribBinding(3, 0);
        }

        glViewport(0, 0, w, h);
        //glLineWidth(4.0f);

        _initialized = true;

		return true;
	}

    void DisplayManager::shutdown()
    {
        if (_initialized) {
            glDeleteVertexArrays(1, &_vao);
            SDL_GL_DeleteContext(_gl_context);
            SDL_DestroyWindow(_sdl_window);
        }
    }

    void DisplayManager::set_state(const uint64_t flags, const bool force)
    {
        if (flags == _glState && !force)
            return;

        uint64_t diff = flags ^ _glState;

        if (force)
            diff = 0xFFFFFFFFFFFFFFFF;

        if (diff == 0)
            return;

        // enable/disable face culling
        if (diff & GLS_CULL_FACE)
        {
            if (flags & GLS_CULL_FACE)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);
        }

        // setup culling
        if (diff & GLS_CULL_BITS)
        {
            switch (flags & GLS_CULL_BITS)
            {
            case GLS_CULL_FRONT:
                glCullFace(GL_FRONT);
                break;
            case GLS_CULL_BACK:
                glCullFace(GL_BACK);
                break;
            default:
                warning("GL_State: Invalid culling mode");
            }
        }

        // polygon mode
        if (diff & GLS_POLYGON_MODE_BITS)
        {
            switch (flags & GLS_POLYGON_MODE_BITS)
            {
            case GLS_POLYGON_MODE_FILL:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case GLS_POLYGON_MODE_LINE:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            default:
                warning("GL_State: Invalid plygon mode");
                break;
            }
        }

        // depth function
        if (diff & GLS_DEPTH_FUNC_BITS)
        {
            GLenum depthFunc = GL_LESS;

            switch (flags & GLS_DEPTH_FUNC_BITS)
            {
            case GLS_DEPTH_FUNC_LESS:
                depthFunc = GL_LESS;
                break;
            case GLS_DEPTH_FUNC_EQUAL:
                depthFunc = GL_EQUAL;
                break;
            case GLS_DEPTH_FUNC_NEVER:
                depthFunc = GL_NEVER;
                break;
            case GLS_DEPTH_FUNC_GEQUAL:
                depthFunc = GL_GEQUAL;
                break;
            case GLS_DEPTH_FUNC_LEQUAL:
                depthFunc = GL_LEQUAL;
                break;
            case GLS_DEPTH_FUNC_GREATER:
                depthFunc = GL_GREATER;
                break;
            case GLS_DEPTH_FUNC_NOTEQUAL:
                depthFunc = GL_NOTEQUAL;
                break;
            case GLS_DEPTH_FUNC_ALLWAYS:
                depthFunc = GL_ALWAYS;
                break;
            default:
                warning("GL_State: invalid depth func");
            }

            if (diff & GLS_DEPTH_MASK)
            {
                if (flags & GLS_DEPTH_MASK)
                    glDepthMask(false);
                else
                    glDepthMask(true);
            }

            glDepthFunc(depthFunc);
        }

        _glState = flags;
    }

    void DisplayManager::poll_input()
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

    void DisplayManager::post_quit_message()
    {
        _running = false;
    }

    void DisplayManager::run()
    {
        _running = true;

        while (_running)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            _mainFunc();
            glFinish();
            SDL_GL_SwapWindow(_sdl_window);
            poll_input();
        }
    }

    void DisplayManager::draw_surface(const drawSurface_t& surf)
    {
        if (!_initialized) return;
        // set material
        // ...

        triSurf_t* triangles = surf.vertexData;
        if (triangles == nullptr)
        {
            return;
        }

        if (!triangles->idxHandle)
        {
            triangles->idxHandle = g_vertexCache.alloc_static_index(triangles->indices, triangles->numIndices);
            triangles->vertHandle = g_vertexCache.alloc_static_vertex(triangles->vertices, triangles->numVerts);
        }

        uint32_t indexOffset, indexSize, vertexOffset, vertexSize;
        bool isStatic;
        g_vertexCache.decode_handle(triangles->idxHandle, indexOffset, indexSize, isStatic);
        g_vertexCache.decode_handle(triangles->vertHandle, vertexOffset, vertexSize, isStatic);

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

        if (isStatic)
        {
            if (glVersion() < 430)
            {
                //glBindVertexArray(vao);
                if (_activeVertexBuffer != &g_vertexCache._static_cache.vertexBuffer) {
                    _activeVertexBuffer = &g_vertexCache._static_cache.vertexBuffer;
                    _activeVertexBuffer->bind();
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(drawVert_t), (void*)VERTEX_ATTRIB_COMPACT_POSITION_OFFSET);
                    glVertexAttribPointer(1, 3, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(drawVert_t), (void*)VERTEX_ATTRIB_COMPACT_NORMAL_OFFSET);
                    glVertexAttribPointer(2, 4, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(drawVert_t), (void*)VERTEX_ATTRIB_COMPACT_TANGENT_OFFSET);
                    glVertexAttribPointer(3, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(drawVert_t), (void*)VERTEX_ATTRIB_COMPACT_TEXCOORD_OFFSET);
                }
            }
            else if(_activeVertexLayout != eVertexLayout::DRAW_VERT)
            {
                glBindVertexBuffer(0, g_vertexCache._static_cache.vertexBuffer._bufferId, 0, sizeof(drawVert_t));
                _activeVertexLayout = eVertexLayout::DRAW_VERT;
            }

            if (_activeIndexBuffer != &g_vertexCache._static_cache.indexBuffer)
            {
                _activeIndexBuffer = &g_vertexCache._static_cache.indexBuffer;
                _activeIndexBuffer->bind();
            }
        }

        glDrawElementsBaseVertex(elemType, triangles->numIndices, GL_UNSIGNED_SHORT, (void*)indexOffset, GLint(vertexOffset / sizeof(drawVert_t)));
    }

    DisplayManager g_displayManager;
}
