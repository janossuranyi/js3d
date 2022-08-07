#ifndef JS3D_DISPLAY_MANAGER_H
#define JS3D_DISPLAY_MANAGER_H

#include <SDL.h>
#include <GL/glew.h>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>

#include "draw_surf.h"
#include "render_common.h"
#include "render_mesh.h"

namespace js3d {

	struct frameData_t
	{
		uint8_t* frameMemory;
		std::atomic_long frameMemoryAllocated{ 0 };
		emptyCommand_t* cmdHead, *cmdTail;
	};

	typedef struct {
		GLuint index;
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLuint relOffset;
	} vertexAttribDef_t;

	typedef struct {
		GLuint stride;
		std::vector<vertexAttribDef_t> attribs;
	} vertexLayoutDef_t;

	class RenderSystem
	{
		friend class VertexBuffer;
		friend class IndexBuffer;
		friend class UniformBuffer;
		friend class ShaderManager;
		friend class ShaderProgram;
	public:
		RenderSystem();
		~RenderSystem();

		bool create_surface(const int w, const int h, const int multisample, const bool fullscreen);
		void shutdown();
		void set_state(const uint64_t flags, const bool force);
		void set_input_handler(std::function<void(const SDL_Event*)> callback) { _inputtHandler = callback; }
		void set_main_callback(std::function<void()> callback) { _mainFunc = callback; }
		void poll_input();
		void post_quit_message();
		void* create_command(renderCommand_t rc, unsigned int bytes);
		void* alloc_frame_mem(unsigned int bytes);

		void draw_frame(const emptyCommand_t* cmds);

		void set_viewport(int x, int y, int w, int h);
		void set_scissor(int x, int y, int w, int h);

		void draw_surface(const drawSurface_t& surf);

		inline int height() const { return _win_h; }
		inline int width() const { return _win_w; }
		inline bool is_running() const { return _running; }

		// internal TODO: make it private
		void create_mesh(createMeshCommand_t* cmd);
		const emptyCommand_t* swap_command_buffers();

	private:
		struct tmu_t {
			tmu_t() {}
			int texture2DId{-1};
			int texture2DArrayId{-1};
			int textureCubeMapId{-1};
		};

		
		int glVersion() const { return _glVersion; }


		bool _initialized;
		SDL_Window* _sdl_window;
		SDL_GLContext _gl_context;
		int _win_w, _win_h;
		uint64_t _glStateBits;
		GLfloat _polyOfsScale, _polyOfsBias;

		int _glVersion;
		std::function<void(const SDL_Event*)> _inputtHandler;
		std::function<void()> _mainFunc;
		bool _running;

		GLuint _activeVertexBuffer;
		GLuint _activeIndexBuffer;
		GLuint _activeTmu;
		GLuint _activeProgram;

		vertexLayout_t _activeVertexLayout;

		vertexLayoutDef_t _vertexLayouts[VERTEX_LAYOUT_LAST_ENUM];

		GLuint _vao;

		tmu_t _tmus[16];

		std::vector<RenderMesh> _render_meshes;
		
		unsigned int _frameCount{ 0 };
		unsigned int _smpFrame{ 0 };
		frameData_t _smpFrameData[NUM_FRAME_DATA];
		frameData_t* _frameData;

	};

	extern RenderSystem g_renderSystem;
}
#endif // !JS3D_DISPLAY_MANAGER_H
