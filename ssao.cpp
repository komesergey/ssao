#ifdef _WIN32
#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include "gl3w.h"
#define GLFW_NO_GLU 1
#define GLFW_INCLUDE_GLCOREARB 1
#include "glfw3.h"
#endif

#ifdef __linux__
#include <unistd.h>
#include "gl3w.h"
#include <GLFW/glfw3.h>
#endif

#include <cstdio>
#include <cstring>
#include <cmath>
#include "shader.h"
#include "object.h"
#include "vmath.h"

static unsigned int seed = 0x13371337;
static inline float random_float() {
	float res;
	unsigned int tmp;
	seed *= 16807;
	tmp = seed ^ (seed >> 4) ^ (seed << 15);
	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;
	return (res - 1.0f);
}

class ssao_app {
public:
	ssao_app()
		: render_program(0),
		ssao_program(0),
		paused(false),
		ssao_level(1.0f),
		ssao_radius(0.05f),
		show_shading(true),
		show_ao(true),
		weight_by_angle(true),
		randomize_points(true),
		point_count(10) {}

	void initFirst() {
		strcpy(info.title, "SSAO");
		info.windowWidth = 800;
		info.windowHeight = 600;
		info.majorVersion = 4;
		info.minorVersion = 1;
		info.samples = 0;
		info.flags.all = 0;
		info.flags.cursor = 1;
	}

	void run(ssao_app* the_app) {
		bool running = true;
		app = the_app;
		if (!glfwInit()) {
			fprintf(stderr, "Failed to initialize GLFW\n");
			return;
		}
		initFirst();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.minorVersion);

		if (info.flags.robust) {
			glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
		}
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_SAMPLES, info.samples);
		glfwWindowHint(GLFW_STEREO, info.flags.stereo ? GL_TRUE : GL_FALSE);
		{
			window = glfwCreateWindow(info.windowWidth, info.windowHeight, info.title, info.flags.fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
			if (!window) {
				fprintf(stderr, "Failed to open window\n");
				return;
			}
		}
		glfwMakeContextCurrent(window);
		glfwSetWindowSizeCallback(window, glfw_onResize);
		glfwSetKeyCallback(window, glfw_onKey);
		if (!info.flags.cursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		gl3wInit();
		startup();
		do {
			render(glfwGetTime());
			glfwSwapBuffers(window);
			glfwPollEvents();
			running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
			running &= (glfwWindowShouldClose(window) != GL_TRUE);
		} while (running);
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void startup();
	void render(double currentTime);
	void onKey(int key, int action);

	struct APPINFO {
		char title[128];
		int windowWidth;
		int windowHeight;
		int majorVersion;
		int minorVersion;
		int samples;
		union{
			struct {
				unsigned int    fullscreen : 1;
				unsigned int    vsync : 1;
				unsigned int    cursor : 1;
				unsigned int    stereo : 1;
				unsigned int    debug : 1;
				unsigned int    robust : 1;
			};
			unsigned int        all;
		} flags;
	};

protected:
	APPINFO     info;
	static      ssao_app * app;
	GLFWwindow* window;

	void load_shaders();

	GLuint      render_program;
	GLuint      ssao_program;
	bool        paused;
	GLuint      render_fbo;
	GLuint      fbo_textures[3];
	GLuint      quad_vao;
	GLuint      points_buffer;
	sb7::object object;
	sb7::object cube;

	struct {
		struct {
			GLint           mv_matrix;
			GLint           proj_matrix;
			GLint           shading_level;
		} render;
		struct {
			GLint           ssao_level;
			GLint           object_level;
			GLint           ssao_radius;
			GLint           weight_by_angle;
			GLint           randomize_points;
			GLint           point_count;
		} ssao;
	} uniforms;

	bool  show_shading;
	bool  show_ao;
	float ssao_level;
	float ssao_radius;
	bool  weight_by_angle;
	bool randomize_points;
	unsigned int point_count;

	struct SAMPLE_POINTS {
		vmath::vec4     point[256];
		vmath::vec4     random_vectors[256];
	};

	void onResize(int w, int h) {
		info.windowWidth = w;
		info.windowHeight = h;
	}

	static void glfw_onResize(GLFWwindow* window, int w, int h) {
		app->onResize(w, h);
	}

	static void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		app->onKey(key, action);
	}
};

void ssao_app::startup() {
	load_shaders();
	glGenFramebuffers(1, &render_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	glGenTextures(3, fbo_textures);
	glBindTexture(GL_TEXTURE_2D, fbo_textures[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, fbo_textures[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, fbo_textures[2]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo_textures[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fbo_textures[1], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo_textures[2], 0);
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, draw_buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);
	object.load("../media/objects/dragon.sbm");
	cube.load("../media/objects/cube.sbm");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	int i;
	SAMPLE_POINTS point_data;

	for (i = 0; i < 256; i++) {
		do {
			point_data.point[i][0] = random_float() * 2.0f - 1.0f;
			point_data.point[i][1] = random_float() * 2.0f - 1.0f;
			point_data.point[i][2] = random_float(); //  * 2.0f - 1.0f;
			point_data.point[i][3] = 0.0f;
		} while (length(point_data.point[i]) > 1.0f);
		normalize(point_data.point[i]);
	}
	for (i = 0; i < 256; i++) {
		point_data.random_vectors[i][0] = random_float();
		point_data.random_vectors[i][1] = random_float();
		point_data.random_vectors[i][2] = random_float();
		point_data.random_vectors[i][3] = random_float();
	}
	glGenBuffers(1, &points_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, points_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &point_data, GL_STATIC_DRAW);
}

void ssao_app::render(double currentTime) {
	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat one = 1.0f;
	static double last_time = 0.0;
	static double total_time = 0.0;

	if (!paused)
		total_time += (currentTime - last_time);
	last_time = currentTime;

	auto f = (float)total_time;

	glViewport(0, 0, info.windowWidth, info.windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	glEnable(GL_DEPTH_TEST);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_COLOR, 1, black);
	glClearBufferfv(GL_DEPTH, 0, &one);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, points_buffer);
	glUseProgram(render_program);

	const vmath::mat4 lookat_matrix = vmath::lookat(vmath::vec3(0.0f, 3.0f, 15.0f),
                                      vmath::vec3(0.0f, 0.0f, 0.0f),
                                      vmath::vec3(0.0f, 1.0f, 0.0f));

	vmath::mat4 proj_matrix = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
	glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, proj_matrix);
	vmath::mat4 mv_matrix = vmath::translate(0.0f, -5.0f, 0.0f) *
							vmath::rotate(f * 5.0f, 0.0f, 1.0f, 0.0f) *
							vmath::mat4::identity();
	glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, lookat_matrix * mv_matrix);
	glUniform1f(uniforms.render.shading_level, show_shading ? (show_ao ? 0.7f : 1.0f) : 0.0f);
	object.render();
	mv_matrix = vmath::translate(0.0f, -4.5f, 0.0f) *
		vmath::rotate(f * 5.0f, 0.0f, 1.0f, 0.0f) *
		vmath::scale(4000.0f, 0.1f, 4000.0f) *
		vmath::mat4::identity();
	glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, lookat_matrix * mv_matrix);
	cube.render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(ssao_program);
	glUniform1f(uniforms.ssao.ssao_radius, ssao_radius * float(info.windowWidth) / 1000.0f);
	glUniform1f(uniforms.ssao.ssao_level, show_ao ? (show_shading ? 0.3f : 1.0f) : 0.0f);
	glUniform1i(uniforms.ssao.randomize_points, randomize_points ? 1 : 0);
	glUniform1ui(uniforms.ssao.point_count, point_count);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_textures[1]);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ssao_app::load_shaders() {
	GLuint shaders[2];

	shaders[0] = sb7::shader::load("../media/shaders/ssao/render.vs.glsl", GL_VERTEX_SHADER);
	shaders[1] = sb7::shader::load("../media/shaders/ssao/render.fs.glsl", GL_FRAGMENT_SHADER);

	if (render_program)
		glDeleteProgram(render_program);

	render_program = sb7::program::link_from_shaders(shaders, 2, true);
	uniforms.render.mv_matrix = glGetUniformLocation(render_program, "mv_matrix");
	uniforms.render.proj_matrix = glGetUniformLocation(render_program, "proj_matrix");
	uniforms.render.shading_level = glGetUniformLocation(render_program, "shading_level");
	shaders[0] = sb7::shader::load("../media/shaders/ssao/ssao.vs.glsl", GL_VERTEX_SHADER);
	shaders[1] = sb7::shader::load("../media/shaders/ssao/ssao.fs.glsl", GL_FRAGMENT_SHADER);
	ssao_program = sb7::program::link_from_shaders(shaders, 2, true);
	uniforms.ssao.ssao_radius = glGetUniformLocation(ssao_program, "ssao_radius");
	uniforms.ssao.ssao_level = glGetUniformLocation(ssao_program, "ssao_level");
	uniforms.ssao.object_level = glGetUniformLocation(ssao_program, "object_level");
	uniforms.ssao.weight_by_angle = glGetUniformLocation(ssao_program, "weight_by_angle");
	uniforms.ssao.randomize_points = glGetUniformLocation(ssao_program, "randomize_points");
	uniforms.ssao.point_count = glGetUniformLocation(ssao_program, "point_count");
}

void ssao_app::onKey(int key, int action) {
	if (action) {
		switch (key) {
		case 'N':
			weight_by_angle = !weight_by_angle;
			break;
		case 'R':
			randomize_points = !randomize_points;
			break;
		case 'S':
			point_count++;
			break;
		case 'X':
			point_count--;
			break;
		case 'Q':
			show_shading = !show_shading;
			break;
		case 'W':
			show_ao = !show_ao;
			break;
		case 'A':
			ssao_radius += 0.01f;
			break;
		case 'Z':
			ssao_radius -= 0.01f;
			break;
		case 'P':
			paused = !paused;
			break;
		case 'L':
			load_shaders();
			break;
            default:
                break;
		}
	}
}

ssao_app * ssao_app::ssao_app::app = nullptr;

#ifdef __linux__
int main() {
    auto app = new ssao_app;
    app->run(app);
    delete app;
    return 0;
}
#endif


#ifdef _WIN32
#define DECLARE_MAIN(a)                             \
ssao_app *app = 0;                                  \
int CALLBACK WinMain(HINSTANCE hInstance,           \
                     HINSTANCE hPrevInstance,       \
                     LPSTR lpCmdLine,               \
                     int nCmdShow)                  \
{                                                   \
    a *app = new a;                                 \
    app->run(app);                                  \
    delete app;                                     \
    return 0;                                       \
}
DECLARE_MAIN(ssao_app)
#endif


