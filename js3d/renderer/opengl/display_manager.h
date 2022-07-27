#ifndef JS3D_DISPLAY_MANAGER_H
#define JS3D_DISPLAY_MANAGER_H

#include <SDL.h>
#include <GL/glew.h>
#include <functional>
#include "draw_surf.h"
#include "render_common.h"
namespace js3d {

	class DisplayManager
	{
		friend class VertexBuffer;
		friend class IndexBuffer;
		friend class UniformBuffer;
		friend class ShaderManager;
		friend class ShaderProgram;
	public:
		DisplayManager();
		~DisplayManager();

		bool create_surface(const int w, const int h, const int multisample, const bool fullscreen);
		void shutdown();
		void set_state(const uint64_t flags, const bool force);
		void set_input_handler(std::function<void(const SDL_Event*)> callback) { _inputtHandler = callback; }
		void set_main_callback(std::function<void()> callback) { _mainFunc = callback; }
		void poll_input();
		void post_quit_message();
		void run();

		void set_viewport(int x, int y, int w, int h);
		void draw_surface(const drawSurface_t& surf);

		inline int height() const { return _win_h; }
		inline int width() const { return _win_w; }

	private:

		int glVersion() const { return _glVersion; }

		bool _initialized;
		SDL_Window* _sdl_window;
		SDL_GLContext _gl_context;
		int _win_w, _win_h;
		uint64_t _glState;
		int _glVersion;
		std::function<void(const SDL_Event*)> _inputtHandler;
		std::function<void()> _mainFunc;
		bool _running;

		VertexBuffer* _activeVertexBuffer;
		IndexBuffer* _activeIndexBuffer;
		eVertexLayout _activeVertexLayout;

		GLuint _vao;
	};

	extern DisplayManager g_displayManager;
}
#endif // !JS3D_DISPLAY_MANAGER_H
