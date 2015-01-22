/*
Copyright 2013-2015 Rohit Nirmal

This file is part of Clonepoint.

Clonepoint is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Clonepoint is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Clonepoint.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DRAW_H
#define DRAW_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "file.h"
#include "sprite.h"
#include "scene.h"
#include "gamestate.h"
#include "menustate.h"
#include "font.h"
#include "stb_truetype.h"
#include "button.h"
#include "matrix.h"

#define RGB_WHITE 1.0f, 1.0f, 1.0f
#define RGB_RED 1.0f, 0.0f, 0.0f

extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObject;
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObject;
extern PFNGLDELETEOBJECTARBPROC glDeleteObject;
extern PFNGLGETINFOLOGARBPROC glGetInfoLog;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv;
extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocation;
extern PFNGLLINKPROGRAMARBPROC glLinkProgram;
extern PFNGLSHADERSOURCEARBPROC glShaderSource;
extern PFNGLUNIFORM1IARBPROC glUniform1i;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
#if _WIN32
extern PFNGLACTIVETEXTUREARBPROC  glActiveTexture;
#endif
extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubData;

class Renderer
{
public:
	Renderer();
	~Renderer();
	void init(int x, int y);
	void setResolution(int x, int y);
	bool initShaders();
	void drawText(float x, float y, const char* text, float red, float green, float blue, float alpha_scale, std::shared_ptr<Font> font);
	void drawTextLabel(std::shared_ptr<TextLabel> tl);
	void drawTextButton(std::shared_ptr<TextButton> tb);
	void drawImageButton(std::shared_ptr<ImageButton> ib);
	void drawButton(std::shared_ptr<Button> button);
	void drawState(std::shared_ptr<BaseState> state);
	void drawMouseCursor(std::shared_ptr<BaseState> state);
	void drawScene(Scene* scene);
	void drawDebugSceneText(Scene* scene);
	void toggleWireframe();
	vec3f getCameraPosition();
	void setCameraPosition(vec3f newPos);
	void takeScreenshot();
	void bufferQuads();
	void bufferScreenVBO(float x, float y);
	void drawRect(float x, float y, float z, GLuint vbo, float red, float green, float blue, float alpha, bool fill);
	void drawRect2(Rect rect, float red, float green, float blue, int z);
	void drawRect2Fill(Rect rect, float red, float green, float blue, int z);
	void drawSprite(float x, float y, float z, float rotation, SpriteSheet* resource, unsigned int index, bool colorOverride, float red, float green, float blue);
	void drawSpriteBind(float x, float y, float z, float rotation, SpriteSheet* resource, unsigned int index, bool colorOverride, float red, float green, float blue);
	void drawFieldOfView(Scene* scene, FieldOfView* fov, GLuint program);
	void drawTileLayer(Scene* scene, int z);
	void updateLinkProgress(unsigned int dT);

	void generateSheetBuffers(SpriteSheet* sheet, int tileDim);

	void drawCollisionVols(Scene* scene);
	void drawEntities(Scene* scene);
	void drawEnemies(Scene* scene, bool crosslink);
	void drawLinkableEntities(Scene* scene);
	void drawTutorialMarks(Scene* scene);
	void drawLights(Scene* scene);
	void drawJumpTrajectory(Scene* scene);
	void drawCrossLink(Scene* scene);
	void drawLink(Scene* scene, Circuit c, vec2f pA, vec2f pB);
	void drawLine(Scene* scene, float r, float g, float b, float a, vec2f pA, vec2f pB, float z);
	void drawInterface(Scene* scene);
	void drawLoadMenu(/*Scene* scene*/);

	//test functions.
	void drawEnemyFOV(Scene* scene, Enemy* enemy);

	//shaders
	void addShader(GLuint shader_program, const char* shader_text, GLuint shader_id);
	GLuint compileShaders(const char* vert_filename, const char* frag_filename);

	unsigned int getScreenshotIndex();
	void setScreenshotIndex(unsigned int value);

	void handleSettingsChange();
	void getBoxCoordsAroundText(const char* text, float x, float y, std::shared_ptr<Font> font, Rect* rect);
	void getBoxDimsAroundText(const char* text, std::shared_ptr<Font> font, vec2f* dims);

	std::function<void()> getScreenshotFunc();

private:
	GLuint entRectVBO; //vbo for storing tile-sized quads.
	GLuint pointVBO; //vbo for storing quad for jump trajectory.
	GLuint _screenVBO; //vbo for quad with dimensions of window.

	bool wireframe;
	GLuint attribute_coord;
	GLuint uniform_color;
	GLuint uniform_alpha_scale;
	GLuint text_vbo;
	GLuint gWorldLocation;

	//textures

	GLuint buttonTex;
	//programs
	GLuint pgmText;
	GLuint pgmButton;
	GLuint pgmColoredSprite; //for use with crosslink.
	GLuint pgmMap;
	GLuint pgmLight;
	GLuint pgmCamera;

	//transformations
	mat4f orthographic;
	mat4f transformation;
	mat4f camera_rotation;
	mat4f quadTransform;

	//for taking screenshots
	SDL_Surface* image;

	char screenshot_filename[32];
	unsigned int screenshotIndex;
	int winX;
	int winY;

	GLuint notex;

	bool _enteredLightFlash;

	//for animating crosslink lines
	float linkProgress;
	vec2f progress;
	vec2f distance;

	float px, py;
	char print[256];

	struct SpriteSheetDeleter
	{
		void operator()(SpriteSheet* sheet) const
		{
			GLuint vbo = sheet->getVertexBuffer();
			glDeleteBuffers(sheet->getNumberOfSprites(), sheet->getIndexBuffers());
			glDeleteBuffers(1, &vbo);
			delete sheet;
		}
	};

	//Fonts
	std::shared_ptr<Font> font1;
	std::shared_ptr<Font> font2;

	//Sprite Resources
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resPlayer;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resPlayerLeft;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resGuardRight;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resGuardLeft;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resEnforcerRight;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resEnforcerLeft;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resProfessionalRight;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resProfessionalLeft;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resSniperRight;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resSniperLeft;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resObjects;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resLinkables;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resInterface;
	std::unique_ptr<SpriteSheet, SpriteSheetDeleter> resGlass;

	//Gameplay strings
	const char* _mouseOverStrings[NUMBER_OF_MOUSEOVER_OBJECTS];
	const char* _messageStrings[NUMBER_OF_STRING_MESSAGES];
	std::string _bindingStrings[NumUsableEnts];
	vec2f _bindingBoxDims[NumUsableEnts];
	vec2f _mouseOverDims[NUMBER_OF_MOUSEOVER_OBJECTS];
	std::string objectivesNotCompleted;
	Rect messageBox;
};

#endif
