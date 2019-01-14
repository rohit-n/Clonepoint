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
#include "statemanager.h"
#include "gamestate.h"
#include "menustate.h"
#include "font.h"
#include "stb_truetype.h"
#include "button.h"
#include "matrix.h"

#define RGB_WHITE 1.0f, 1.0f, 1.0f
#define RGB_RED 1.0f, 0.0f, 0.0f

enum SpriteDrawMode
{
	SDM_Normal = 0,
	SDM_LinkableCross,
	SDM_PropCross
};

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
extern PFNGLACTIVETEXTUREARBPROC glActiveTexture_t;
#define glActiveTexture glActiveTexture_t
#endif
extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubData;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void safeDeleteBuffer(GLuint buf);
	void safeDeleteProgram(GLuint pgm);
	void safeDeleteTexture(GLuint tex);

	bool init(int x, int y);
	void setResolution(int x, int y);
	bool initShaders();
	void drawText(float x, float y, const char* text, float red, float green, float blue, float alpha_scale, Font* font);
	void drawTextLabel(TextLabel* tl);
	void drawTextButton(TextButton* tb);
	void drawImageButton(ImageButton* ib);
	void drawButton(Button* button);
	void drawState(BaseState* state);
	void drawMouseCursor(BaseState* state);
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
	void drawSprite(float x, float y, float z, float rotation, SpriteSheet* resource, unsigned int index, SpriteDrawMode mode, float red, float green, float blue);
	void drawSpriteBind(float x, float y, float z, float rotation, SpriteSheet* resource, unsigned int index, SpriteDrawMode mode, float red, float green, float blue);
	void drawFieldOfView(Scene* scene, FieldOfView* fov, GLuint program);
	void drawTileLayer(Scene* scene, int z);
	void drawSceneBackgrounds(Scene* scene);
	void drawBackground(Scene* scene, GLuint tex, int x, int z, float offset);
	void drawMenuBackground(GLuint tex);
	void updateLinkProgress(unsigned int dT);
	void deleteSpriteSheet(SpriteSheet* sheet);

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
	void changeMenuBackground(eState state);

	//test functions.
	void drawEnemyFOV(Scene* scene, Enemy* enemy);

	//shaders
	void addShader(GLuint shader_program, const char* shader_text, GLuint shader_id);
	GLuint compileShaders(const char* vert_filename, const char* frag_filename);

	unsigned int getScreenshotIndex();
	void setScreenshotIndex(unsigned int value);

	void handleSettingsChange();
	void getBoxCoordsAroundText(const char* text, float x, float y, Font* font, Rect* rect);
	void getBoxDimsAroundText(const char* text, Font* font, vec2f* dims);

private:
	GLuint entRectVBO; //vbo for storing tile-sized quads.
	GLuint pointVBO; //vbo for storing quad for jump trajectory.
	GLuint _screenVBO; //vbo for quad with dimensions of window.
	GLuint _screenTexVBO;

	bool wireframe;
	bool _drawSceneBackgrounds;
	bool _drawCollisionOutlines;
	GLuint attribute_coord;
	GLuint uniform_color;
	GLuint uniform_alpha_scale;
	GLuint text_vbo;
	GLuint gWorldLocation;

	//textures
	GLuint bgClose;
	GLuint bgMiddle;
	GLuint bgFar;
	GLuint bgVeryFar;
	//menus
	GLuint bgMainMenu;
	GLuint bgCredits;
	GLuint bgOptions;
	GLuint bgLoadMap;
	GLuint bgPaused;
	GLuint bgUpgrades;
	GLuint bgActive;

	//programs
	GLuint pgmText;
	GLuint pgmButton;
	GLuint pgmColoredSprite; //for linkable entities in crosslink.
	GLuint pgmMap;
	GLuint pgmLight;
	GLuint pgmCamera;
	GLuint pgmCrosslinkProp; //for props in crosslink.

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
	bool _crosslinkBlur;

	//for animating crosslink lines
	float linkProgress;
	vec2f progress;
	vec2f distance;

	float px, py;
	char print[256];

	//Fonts
	Font* font1;
	Font* font2;

	//Sprite Resources
	SpriteSheet* resPlayer;
	SpriteSheet* resPlayerLeft;
	SpriteSheet* resGuardRight;
	SpriteSheet* resGuardLeft;
	SpriteSheet* resEnforcerRight;
	SpriteSheet* resEnforcerLeft;
	SpriteSheet* resProfessionalRight;
	SpriteSheet* resProfessionalLeft;
	SpriteSheet* resSniperRight;
	SpriteSheet* resSniperLeft;
	SpriteSheet* resObjects;
	SpriteSheet* resLinkables;
	SpriteSheet* resInterface;
	SpriteSheet* resGlass;

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
