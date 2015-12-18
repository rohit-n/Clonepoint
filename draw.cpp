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

#include <fstream>
#include "draw.h"
#include "locator.h"

PFNGLATTACHSHADERPROC glAttachShader;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObject;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObject;
PFNGLDELETEOBJECTARBPROC glDeleteObject;
PFNGLGETINFOLOGARBPROC glGetInfoLog;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocation;
PFNGLLINKPROGRAMARBPROC glLinkProgram;
PFNGLSHADERSOURCEARBPROC glShaderSource;
PFNGLUNIFORM1IARBPROC glUniform1i;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
#if _WIN32
PFNGLACTIVETEXTUREARBPROC  glActiveTexture;
#endif
PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubData;

#define BACKGROUND_WIDTH 1280
#define BACKGROUND_HEIGHT 720

struct point
{
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

struct SpriteVertex
{
	float position[2];
	float texcoord[2];
};

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	LOGF((stdout, "Running Renderer destructor.\n"));
	if (pgmText)
		glDeleteObject(pgmText);
	if (pgmMap)
		glDeleteObject(pgmMap);
	if (pgmButton)
		glDeleteObject(pgmButton);
	if (pgmColoredSprite)
		glDeleteObject(pgmColoredSprite);
	if (pgmCrosslinkProp)
		glDeleteObject(pgmCrosslinkProp);
	if (text_vbo)
		glDeleteBuffers(1, &text_vbo);
	if (entRectVBO)
		glDeleteBuffers(1, &entRectVBO);
	if (pointVBO)
		glDeleteBuffers(1, &pointVBO);
	if (_screenVBO)
		glDeleteBuffers(1, &_screenVBO);

	delete font1;
	delete font2;

	deleteSpriteSheet(resPlayer);
	deleteSpriteSheet(resPlayerLeft);
	deleteSpriteSheet(resGuardRight);
	deleteSpriteSheet(resGuardLeft);
	deleteSpriteSheet(resEnforcerLeft);
	deleteSpriteSheet(resEnforcerRight);
	deleteSpriteSheet(resProfessionalLeft);
	deleteSpriteSheet(resProfessionalRight);
	deleteSpriteSheet(resSniperLeft);
	deleteSpriteSheet(resSniperRight);
	deleteSpriteSheet(resObjects);
	deleteSpriteSheet(resLinkables);
	deleteSpriteSheet(resInterface);
	deleteSpriteSheet(resGlass);

	if (bgClose > 0)
		glDeleteTextures(1, &bgClose);
	if (bgMiddle > 0)
		glDeleteTextures(1, &bgMiddle);
	if (bgFar > 0)
		glDeleteTextures(1, &bgFar);
	if (bgVeryFar > 0)
		glDeleteTextures(1, &bgVeryFar);
}

void Renderer::deleteSpriteSheet(SpriteSheet* sheet)
{
	if (sheet == NULL || sheet->getTexId() == 0)
	{
		return;
	}

	GLuint vbo = sheet->getVertexBuffer();
	if (sheet->getNumberOfSprites() > 0)
	{
		glDeleteBuffers(sheet->getNumberOfSprites(), sheet->getIndexBuffers());
	}
	glDeleteBuffers(1, &vbo);
	delete sheet;
}

void Renderer::toggleWireframe()
{
	if (!wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	wireframe = !wireframe;
}

bool Renderer::init(int x, int y)
{
	_screenVBO = 0;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glClearColor(0.3960784313725490196078431372549, 0.61176470588235294117647058823529, 0.93725490196078431372549019607843, 1);

	setResolution(x, y);

	pgmMap = compileShaders("shaders/main.vert", "shaders/main.frag");
	pgmText = compileShaders("shaders/text.vert", "shaders/text.frag");
	pgmButton = compileShaders("shaders/button.vert", "shaders/button.frag");
	pgmColoredSprite = compileShaders("shaders/sprite_colored.vert", "shaders/sprite_colored.frag");
	pgmLight = compileShaders("shaders/main.vert", "shaders/light.frag");
	pgmCamera = compileShaders("shaders/main.vert", "shaders/camera.frag");
	pgmCrosslinkProp = compileShaders("shaders/button.vert", "shaders/sprite_crosslink.frag");

	//initialize map shader variables

	glUseProgramObject(pgmMap);

	glLinkProgram(pgmMap); /*Must link program before uniforms and after attributes?*/
	gWorldLocation = glGetUniformLocation(pgmMap, "gWorld");

	//initialize text shader variables
	/*Not sure if needed...
	glUseProgram(pgmText);*/
	glBindAttribLocation(pgmText, 0,  "coord");
	glBindAttribLocation(pgmText, 1,  "texpos");
	uniform_color = glGetUniformLocation(pgmText, "color");
	uniform_alpha_scale = glGetUniformLocation(pgmText, "alpha_mod");
	glGenBuffers(1, &text_vbo);

	glBindAttribLocation(pgmButton, 0, "coord");
	glBindAttribLocation(pgmButton, 1,  "texpos");

	screenshotIndex = 1;
	wireframe = false;
	_enteredLightFlash = false;
	_crosslinkBlur = false;

	linkProgress = 0.0f;

	SDL_ShowCursor(SDL_DISABLE);

	bufferQuads();

	font1 = new Font("./data/fonts/VeraMono.ttf", 32.0f);
	font2 = new Font("./data/fonts/VeraMono.ttf", 16.0f);

	resPlayer = new SpriteSheet("./data/sprites/player.png", ENTDIM, false);
	resPlayerLeft = new SpriteSheet("./data/sprites/player.png", ENTDIM, true);
	resGuardRight = new SpriteSheet("./data/sprites/guard.png", ENTDIM, false);
	resGuardLeft = new SpriteSheet("./data/sprites/guard.png", ENTDIM, true);
	resEnforcerLeft = new SpriteSheet("./data/sprites/enforcer.png", ENTDIM, true);
	resEnforcerRight = new SpriteSheet("./data/sprites/enforcer.png", ENTDIM, false);
	resProfessionalLeft = new SpriteSheet("./data/sprites/professional.png", ENTDIM, true);
	resProfessionalRight = new SpriteSheet("./data/sprites/professional.png", ENTDIM, false);
	resSniperLeft = new SpriteSheet("./data/sprites/sniper.png", ENTDIM, true);
	resSniperRight = new SpriteSheet("./data/sprites/sniper.png", ENTDIM, false);
	resObjects = new SpriteSheet("./data/sprites/objects.png", ENTDIM, false);
	resLinkables = new SpriteSheet("./data/sprites/linkable.png", ENTDIM, false);
	resInterface = new SpriteSheet("./data/sprites/interface.png", 32, false);
	resGlass = new SpriteSheet("./data/sprites/glass.png", 8, false);

	//backgrounds
	bgClose = loadTexture("./data/backgrounds/bgClose.png");
	bgMiddle = loadTexture("./data/backgrounds/bgMiddle.png");
	bgFar = loadTexture("./data/backgrounds/bgFar.png");
	bgVeryFar = loadTexture("./data/backgrounds/bgVeryFar.png");

	if (resPlayer->getTexId() == 0 ||
		resPlayerLeft->getTexId() == 0 ||
		resGuardRight->getTexId() == 0 ||
		resGuardLeft->getTexId() == 0 ||
		resEnforcerLeft->getTexId() == 0 ||
		resEnforcerRight->getTexId() == 0 ||
		resProfessionalLeft->getTexId() == 0 ||
		resProfessionalRight->getTexId() == 0 ||
		resSniperLeft->getTexId() == 0 ||
		resSniperRight->getTexId() == 0 ||
		resObjects->getTexId() == 0 ||
		resLinkables->getTexId() == 0 ||
		resInterface->getTexId() == 0 ||
		resGlass->getTexId() == 0)
	{
		deleteSpriteSheet(resPlayer);
		deleteSpriteSheet(resPlayerLeft);
		deleteSpriteSheet(resGuardRight);
		deleteSpriteSheet(resGuardLeft);
		deleteSpriteSheet(resEnforcerLeft);
		deleteSpriteSheet(resEnforcerRight);
		deleteSpriteSheet(resProfessionalLeft);
		deleteSpriteSheet(resProfessionalRight);
		deleteSpriteSheet(resSniperLeft);
		deleteSpriteSheet(resSniperRight);
		deleteSpriteSheet(resObjects);
		deleteSpriteSheet(resLinkables);
		deleteSpriteSheet(resInterface);
		deleteSpriteSheet(resGlass);
		return false;
	}

	generateSheetBuffers(resPlayer, ENTDIM);
	generateSheetBuffers(resPlayerLeft, ENTDIM);
	generateSheetBuffers(resGuardRight, ENTDIM);
	generateSheetBuffers(resGuardLeft, ENTDIM);
	generateSheetBuffers(resEnforcerLeft, ENTDIM);
	generateSheetBuffers(resEnforcerRight, ENTDIM);
	generateSheetBuffers(resProfessionalLeft, ENTDIM);
	generateSheetBuffers(resProfessionalRight, ENTDIM);
	generateSheetBuffers(resSniperLeft, ENTDIM);
	generateSheetBuffers(resSniperRight, ENTDIM);
	generateSheetBuffers(resObjects, ENTDIM);
	generateSheetBuffers(resLinkables, ENTDIM);
	generateSheetBuffers(resInterface, 32);
	generateSheetBuffers(resGlass, 8);

	_mouseOverStrings[MO_CircuitBox] = "A circuit box. Use it to unlock its circuit.";
	_mouseOverStrings[MO_LightFixture] = "A light fixture. Toggles its light when activated.";
	_mouseOverStrings[MO_MainComputer] = "A computer terminal. Hack these to complete mission objectives.";
	_mouseOverStrings[MO_HandScanner] = "A hand scanner. Only guards can use these.";
	_mouseOverStrings[MO_Elevator] = "An elevator switch. Makes nearby enemies look when an elevator arrives.";
	_mouseOverStrings[MO_MotionScanner] = "A motion detector. Activates when any living entity passes through.";
	_mouseOverStrings[MO_Switch] = "A light switch.";
	_mouseOverStrings[MO_Door] = "A door. Opens or closes when activated.";
	_mouseOverStrings[MO_TrapDoor] = "A trap door. Opens when activated, then closes after a time.";
	_mouseOverStrings[MO_VaultDoor] = "A vault door. Opens when activated, then closes after a time.";
	_mouseOverStrings[MO_SoundDetector] = "A sound detector. Activated by loud sounds nearby.";
	_mouseOverStrings[MO_Alarm] = "An alarm. When activated, alerts nearby guard to turn it off.";
	_mouseOverStrings[MO_PowerSocket] = "A power socket. Knocks out guards nearby when activated.";
	_mouseOverStrings[MO_SecurityCamera] = "A security camera. Activates whenever you enter its field of vision.";
	_mouseOverStrings[MO_Guard] = "A Guard. Shoots on sight.";
	_mouseOverStrings[MO_Enforcer] = "An Enforcer. Cannot be pounced.";
	_mouseOverStrings[MO_Professional] = "A Professional. Has faster reflexes, can see in the dark, shoots when held at gunpoint.";
	_mouseOverStrings[MO_Sniper] = "A Sniper. You probably won't make it out.";

	for (size_t i = 0; i < NUMBER_OF_MOUSEOVER_OBJECTS; i++)
	{
		getBoxDimsAroundText(_mouseOverStrings[i], font2, &_mouseOverDims[i]);
	}

	_messageStrings[SM_Start] = "Welcome to Clonepoint.";
	_messageStrings[SM_Jumping] = "Use the mouse to jump on to the side of the building.";
	_messageStrings[SM_Falling] = "You can survive any fall.";
	_messageStrings[SM_Guards1] = "Avoid the guard's field of vision. Pounce on them to knock them out.";
	_messageStrings[SM_Guards2] = "Guards will search for the nearest switch to turn the light back on. Turn off the light.";
	_messageStrings[SM_Crosslink1] = "Enter Crosslink and drag a connection from the light switch to the door to proceed.";
	_messageStrings[SM_Crosslink2] = "Good job! Use Crosslink to get to your objectives in creative ways.";
	_messageStrings[SM_Objectives] = "Hack all computer terminals and go to the exit to end the level.";
	_messageStrings[SM_Elevators] = "Enemies cannot see you while you are in an elevator.";
	_messageStrings[SM_ObjectivesIncomplete] = "Objectives not completed.";

	handleSettingsChange();
	return true;
}

void Renderer::setResolution(int x, int y)
{
	winX = x;
	winY = y;
	orthographic = mat4f_orthographic(0.0f, winX, winY, 0.0f, -5.0f, 5.0f);
	glViewport(0, 0, winX, winY);
	bufferScreenVBO((float)winX, (float)winY);
}

bool CheckGLExtension(const char* ext, const char* list)
{
	const char *str = strstr(list, ext);
	if (!str)
	{
		return false;
	}
	const char ch = str[strlen(ext)];
	return ((ch == ' ') || (ch == '\t') || (ch == '\0'));
}

bool Renderer::initShaders()
{
	char* extensions = (char*)glGetString(GL_EXTENSIONS);

	bool shaders_supported = false;
	if (CheckGLExtension("GL_ARB_shader_objects", extensions) &&
	        CheckGLExtension("GL_ARB_shading_language_100", extensions) &&
	        CheckGLExtension("GL_ARB_vertex_shader", extensions) &&
	        CheckGLExtension("GL_ARB_fragment_shader", extensions))
	{
		glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
		glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
		glCreateProgramObject = (PFNGLCREATEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glCreateProgramObjectARB");
		glCreateShaderObject = (PFNGLCREATESHADEROBJECTARBPROC)SDL_GL_GetProcAddress("glCreateShaderObjectARB");
		glDeleteObject = (PFNGLDELETEOBJECTARBPROC)SDL_GL_GetProcAddress("glDeleteObjectARB");
		glGetInfoLog = (PFNGLGETINFOLOGARBPROC)SDL_GL_GetProcAddress("glGetInfoLogARB");
		glGetObjectParameteriv = (PFNGLGETOBJECTPARAMETERIVARBPROC)SDL_GL_GetProcAddress("glGetObjectParameterivARB");
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONARBPROC)SDL_GL_GetProcAddress("glGetUniformLocationARB");
		glLinkProgram = (PFNGLLINKPROGRAMARBPROC)SDL_GL_GetProcAddress("glLinkProgramARB");
		glShaderSource = (PFNGLSHADERSOURCEARBPROC)SDL_GL_GetProcAddress("glShaderSourceARB");
		glUniform1i = (PFNGLUNIFORM1IARBPROC)SDL_GL_GetProcAddress("glUniform1iARB");
		glUseProgramObject = (PFNGLUSEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glUseProgramObjectARB");
		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glDisableVertexAttribArray");
		glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)SDL_GL_GetProcAddress("glUniformMatrix4fv");
		glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
		glUniform3f = (PFNGLUNIFORM3FPROC)SDL_GL_GetProcAddress("glUniform3f");
		glUniform2f = (PFNGLUNIFORM2FPROC)SDL_GL_GetProcAddress("glUniform2f");
		glUniform1f = (PFNGLUNIFORM1FPROC)SDL_GL_GetProcAddress("glUniform1f");
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
		glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glBindAttribLocation");
		glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glGetAttribLocation");
		glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)SDL_GL_GetProcAddress("glValidateProgram");
#if _WIN32
		glActiveTexture = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glActiveTexture");
#endif
		glGetBufferSubData = (PFNGLGETBUFFERSUBDATAARBPROC)SDL_GL_GetProcAddress("glGetBufferSubData");

		if (glAttachShader &&
		        glCompileShader &&
		        glCreateProgramObject &&
		        glCreateShaderObject &&
		        glDeleteObject &&
		        glGetInfoLog &&
		        glGetObjectParameteriv &&
		        glGetUniformLocation &&
		        glLinkProgram &&
		        glShaderSource &&
		        glUniform1i &&
		        glUseProgramObject &&
		        glEnableVertexAttribArray &&
		        glDisableVertexAttribArray &&
		        glBindBuffer &&
		        glUniformMatrix4fv &&
		        glGenBuffers &&
		        glUniform3f &&
		        glUniform2f &&
		        glUniform1f &&
		        glDeleteBuffers &&
		        glBindAttribLocation &&
		        glBufferData &&
		        glVertexAttribPointer &&
		        glGetAttribLocation &&
		        glValidateProgram
#if _WIN32
		        && glActiveTexture
#endif
		   )
		{
			shaders_supported = true;
		}
		else
		{
			LOGF((stderr, "Error getting OpenGL function pointers!\n"));
		}
	}
	return shaders_supported;
}

void Renderer::takeScreenshot()
{
	sprintf(screenshot_filename, "screenshot%i.bmp", screenshotIndex);
	image = SDL_CreateRGBSurface(SDL_SWSURFACE, winX, winY, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, winX, winY, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	image = flipSurface(image, Flip_Vertical);
	SDL_SaveBMP(image, screenshot_filename);
	SDL_FreeSurface(image);
	screenshotIndex++;
}

void Renderer::drawState(BaseState* state)
{
	size_t i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (dynamic_cast<MenuState*>(state))
	{
		for (i = 0; i < static_cast<MenuState*>(state)->getButtonCount(); i++)
		{
			drawButton(static_cast<MenuState*>(state)->getButtonAt(i));
		}
	}
	else
	{
		drawScene(static_cast<GameState*>(state)->getScene());
	}

	for (i = 0; i < static_cast<MenuState*>(state)->getLabelCount(); i++)
	{
		drawTextLabel(state->getLabelAt(i));
	}
	drawMouseCursor(state);
}

void Renderer::drawMouseCursor(BaseState* state)
{
	int x, y;
	state->getMousePosition(&x, &y);
	bool safe = true;

	if (dynamic_cast<GameState*>(state) && static_cast<GameState*>(state)->isMouseCursorSeen())
	{
		safe = false;
	}

	drawSpriteBind(x, y, 4.0f, 0, resInterface, Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites",
	               safe ? "pointer_safe" : "pointer_unsafe"), SDM_Normal, 1, 1, 1);
}

void Renderer::drawText(float x, float y, const char* text, float red, float green, float blue, float alpha_scale, Font* font)
{
	float xCurr = x;
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
	glUseProgramObject(pgmText);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, font->getTexture());
	glUniformMatrix4fv(glGetUniformLocation(pgmText, "proj_mat"), 1, GL_TRUE, &orthographic.m[0][0]);
	glUniform3f(uniform_color, red, green, blue);
	glUniform1f(uniform_alpha_scale, alpha_scale);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	std::vector<point> coords;
	point p;
	point top_left;
	point bottom_right;
	stbtt_aligned_quad q;
	int c = 0;

	while (*text)
	{
		if ((*text >= 32 && *text < 127) || *text == '\n')
		{
			if (*text == '\n')
			{
				y += font->getSize();
				xCurr = x;
				++text;
				continue;
			}
			stbtt_GetBakedQuad(font->data(), 512, 512, *text - 32, &xCurr, &y, &q, 1);

			p.x = q.x0;
			p.y = q.y0;
			p.s = q.s0;
			p.t = q.t0;
			coords.push_back(p);

			top_left.x = q.x0;
			top_left.y = q.y1;
			top_left.s = q.s0;
			top_left.t = q.t1;

			bottom_right.x = q.x1;
			bottom_right.y = q.y0;
			bottom_right.s = q.s1;
			bottom_right.t = q.t0;

			coords.push_back(top_left);
			coords.push_back(bottom_right);
			coords.push_back(top_left);
			coords.push_back(bottom_right);

			p.x = q.x1;
			p.y = q.y1;
			p.s = q.s1;
			p.t = q.t1;
			coords.push_back(p);

			c += 6;
		}
		++text;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(point) * coords.size(), coords.data(), GL_DYNAMIC_DRAW);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, c);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::drawTextButton(TextButton* tb)
{
	float r, g, b;

	r = g = b = 0;
	tb->getColors(&r, &g, &b);
	drawText(tb->getX(), tb->getY() + 32, tb->getText(), r, g, b, 1.0f, font1);
}

void Renderer::drawImageButton(ImageButton* ib)
{
	drawSprite(ib->getX(), ib->getY(), 2, 0, resInterface, ib->getSpriteIndex(), SDM_Normal, 1, 1, 1);
}

void Renderer::drawButton(Button* button)
{
	glBindTexture(GL_TEXTURE_2D, resInterface->getTexId());
	if (dynamic_cast<TextButton*>(button))
	{
		drawTextButton(static_cast<TextButton*>(button));
	}

	if (dynamic_cast<ImageButton*>(button))
	{
		drawImageButton(static_cast<ImageButton*>(button));
	}
}

void Renderer::drawTextLabel(TextLabel* tl)
{
	if (!tl->isVisible())
	{
		return;
	}

	int x, y;
	float r, g, b, a;

	x = y = r = g = b = 0;

	tl->getPosition(&x, &y);
	tl->getColors(&r, &g, &b, &a);

	drawText(x, y, tl->getText().c_str(), r, g, b, a, font1);
}

void Renderer::bufferQuads()
{
	//2 triangles.
	float verts[] = {	0.0, 0.0,
	                    0.0, ENTDIM,
	                    ENTDIM, 0.0f,
	                    0.0, ENTDIM,
	                    ENTDIM, 0.0f,
	                    ENTDIM, ENTDIM
	                };

	glGenBuffers(1, &entRectVBO);
	glBindBuffer(GL_ARRAY_BUFFER, entRectVBO);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &verts[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	float vertsp[] = {	0.0, 0.0,
	                    0.0, 4.0f,
	                    4.0f, 0.0f,
	                    0.0, 4.0f,
	                    4.0, 0.0f,
	                    4.0f, 4.0f
	                 };

	glGenBuffers(1, &pointVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &vertsp[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::bufferScreenVBO(float x, float y)
{
	if (_screenVBO)
		glDeleteBuffers(1, &_screenVBO);

	float verts[] = {	0.0, 0.0,
	                    0.0, y,
	                    x, 0.0f,
	                    0.0, y,
	                    x, 0.0f,
	                    x, y
	                };

	glGenBuffers(1, &_screenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _screenVBO);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &verts[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::drawRect(float x, float y, float z, GLuint vbo, float red, float green, float blue, float alpha, bool fill)
{
	glUseProgramObject(pgmMap);
	glUniform3f(glGetUniformLocation(pgmMap, "color"), red, green, blue);
	glUniform1f(glGetUniformLocation(pgmMap, "alpha"), alpha);

	quadTransform = mat4f_mult(orthographic, mat4f_translate(x, y, z));
	glUniformMatrix4fv(glGetUniformLocation(pgmMap, "gWorld"), 1, GL_TRUE, &quadTransform.m[0][0]);
	glEnableVertexAttribArray(glGetAttribLocation(pgmMap, "Position"));
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(glGetAttribLocation(pgmMap, "Position"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	if (!fill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);
	if (!fill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableVertexAttribArray(glGetAttribLocation(pgmMap, "Position"));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgramObject(0);
}

void Renderer::drawSpriteBind(float x, float y, float z, float rotation, SpriteSheet* resource, unsigned int index, SpriteDrawMode mode, float red, float green, float blue)
{
	glBindTexture(GL_TEXTURE_2D, resource->getTexId());
	drawSprite(x, y, z, rotation, resource, index, mode, red, green, blue);
}

void Renderer::drawSprite(float x, float y, float z, float rotation, SpriteSheet* resource, unsigned int index, SpriteDrawMode mode, float red, float green, float blue)
{
	float xOff, yOff;
	GLuint pgm;
	resource->getClipPosition(index, &xOff, &yOff);

	if (mode == SDM_LinkableCross)
	{
		pgm = pgmColoredSprite;
		glUseProgramObject(pgm);
		glUniform3f(glGetUniformLocation(pgm, "color"), red, green, blue);
	}
	else
	{
		pgm = (mode == SDM_Normal) ? pgmButton : pgmCrosslinkProp;
		glUseProgramObject(pgm);
	}

	glBindBuffer(GL_ARRAY_BUFFER, resource->getVertexBuffer());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->getIndexBuffers()[index]);
	glActiveTexture(GL_TEXTURE0);

	//Cast position to int to prevent "shimmering" texture.
	quadTransform = mat4f_mult(orthographic, mat4f_translate((int)x - xOff, (int)y - yOff, z));
	quadTransform = mat4f_mult(quadTransform, mat4f_translate(xOff + (resource->getTileDim() / 2) , yOff + (resource->getTileDim() / 2), 0));
	quadTransform = mat4f_mult(quadTransform, mat4f_rotate(0, 0, rotation));
	quadTransform = mat4f_mult(quadTransform, mat4f_translate(-xOff - (resource->getTileDim() / 2), -yOff - (resource->getTileDim() / 2) , 0));

	glUniformMatrix4fv(glGetUniformLocation(pgm, "proj_mat"), 1, GL_TRUE, &quadTransform.m[0][0]);
	glEnableVertexAttribArray(glGetAttribLocation(pgm, "Position"));
	glEnableVertexAttribArray(glGetAttribLocation(pgm, "TexCoord"));

	SpriteVertex* vert = NULL;
	glVertexAttribPointer(glGetAttribLocation(pgm, "Position"), 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), vert->position);
	glVertexAttribPointer(glGetAttribLocation(pgm, "TexCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), vert->texcoord);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(glGetAttribLocation(pgm, "Position"));
	glDisableVertexAttribArray(glGetAttribLocation(pgm, "TexCoord"));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgramObject(0);
}

void Renderer::drawFieldOfView(Scene* scene, FieldOfView* fov, GLuint program)
{
	if (!fov->_active)
	{
		return;
	}

	float r, g, b;
	fov->getColors(&r, &g, &b);

	float* verts = fov->getVertData();

	glUseProgramObject(program);

	quadTransform = mat4f_mult(orthographic, mat4f_translate(-scene->getCamera().x, -scene->getCamera().y, 0));
	glUniformMatrix4fv(glGetUniformLocation(program, "gWorld"), 1, GL_TRUE, &quadTransform.m[0][0]);
	glUniform3f(glGetUniformLocation(program, "Color"), r, g, b);
	glUniform2f(glGetUniformLocation(program, "lightCenter"), fov->getPosition().x - scene->getCamera().x , winY - fov->getPosition().y + scene->getCamera().y);

	glEnableVertexAttribArray(glGetAttribLocation(program, "Position"));
	glVertexAttribPointer(glGetAttribLocation(program, "Position"), 2, GL_FLOAT, GL_FALSE, 0, verts);

	glDrawArrays(GL_TRIANGLE_FAN, 0, fov->getNumberOfVerts());

	glDisableVertexAttribArray(glGetAttribLocation(program, "Position"));
}

void Renderer::generateSheetBuffers(SpriteSheet* sheet, int tileDim)
{
	unsigned int spriteCount = sheet->getNumberOfSprites();

	unsigned int i, j;
	float clipX, clipY;
	float sheetWidth = sheet->getWidth();
	float sheetHeight = sheet->getHeight();
	std::vector<SpriteVertex> verts(spriteCount * 6);
	GLuint* ibos = sheet->getIndexBuffers();
	GLuint indices[6] = {0, 0, 0, 0};
	glGenBuffers(spriteCount, ibos);

	for (i = 0; i < spriteCount; i++)
	{
		for (j = 0; j < 6; j++)
		{
			indices[j] = i * 6 + j;
		}

		sheet->getClipPosition(i, &clipX, &clipY);

		verts[indices[0]].position[0] = clipX;
		verts[indices[0]].position[1] = clipY;
		verts[indices[0]].texcoord[0] = clipX / sheetWidth;
		verts[indices[0]].texcoord[1] = clipY / sheetHeight;

		verts[indices[1]].position[0] = clipX;
		verts[indices[1]].position[1] = clipY + tileDim;
		verts[indices[1]].texcoord[0] = clipX / sheetWidth;
		verts[indices[1]].texcoord[1] = (clipY + tileDim) / sheetHeight;

		verts[indices[2]].position[0] = clipX + tileDim;
		verts[indices[2]].position[1] = clipY;
		verts[indices[2]].texcoord[0] = (clipX + tileDim) / sheetWidth;
		verts[indices[2]].texcoord[1] = clipY / sheetHeight;

		verts[indices[3]] = verts[indices[1]];
		verts[indices[4]] = verts[indices[2]];

		verts[indices[5]].position[0] = clipX + tileDim;
		verts[indices[5]].position[1] = clipY + tileDim;
		verts[indices[5]].texcoord[0] = (clipX + tileDim) / sheetWidth;
		verts[indices[5]].texcoord[1] = (clipY + tileDim) / sheetHeight;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
		LOGF((stdout, "Created Element Buffer Object %i.\n", ibos[i]));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLuint vbo = sheet->getVertexBuffer();
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, spriteCount * 6 * sizeof(SpriteVertex), verts.data(), GL_STATIC_DRAW);
	LOGF((stdout, "Created Vertex Buffer Object %i.\n", vbo));
	sheet->setVertexBuffer(vbo);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Draw a Rect structure of arbitrary size - no VBO.
void Renderer::drawRect2(Rect rect, float red, float green, float blue, int z)
{
	glUseProgramObject(pgmMap);
	glUniform3f(glGetUniformLocation(pgmMap, "color"), red, green, blue);

	float verts[] = {	0.0f, 0.0f,
	                    0.0f, rect.h,
	                    0.0f, 0.0f,
	                    rect.w, 0.0f,
	                    0.0f, rect.h,
	                    rect.w, rect.h,
	                    rect.w, 0.0f,
	                    rect.w, rect.h
	                };

	quadTransform = mat4f_mult(orthographic, mat4f_translate(rect.x, rect.y, z));
	glUniformMatrix4fv(glGetUniformLocation(pgmMap, "gWorld"), 1, GL_TRUE, &quadTransform.m[0][0]);
	glEnableVertexAttribArray(glGetAttribLocation(pgmMap, "Position")); /*vertex coords*/
	glVertexAttribPointer(glGetAttribLocation(pgmMap, "Position"), 2, GL_FLOAT, GL_FALSE, 0, verts);

	glDrawArrays(GL_LINES, 0, 8);

	glDisableVertexAttribArray(glGetAttribLocation(pgmMap, "Position"));
	glUseProgramObject(0);
}

void Renderer::drawRect2Fill(Rect rect, float red, float green, float blue, int z)
{
	glUseProgramObject(pgmMap);
	glUniform3f(glGetUniformLocation(pgmMap, "color"), red, green, blue);

	float verts[] = {	0.0f, 0.0f,
	                    0.0f, rect.h,
	                    rect.w, 0.0f,
	                    0.0f, rect.h,
	                    rect.w, 0.0f,
	                    rect.w, rect.h
	                };

	quadTransform = mat4f_mult(orthographic, mat4f_translate(rect.x, rect.y, z));
	glUniformMatrix4fv(glGetUniformLocation(pgmMap, "gWorld"), 1, GL_TRUE, &quadTransform.m[0][0]);
	glEnableVertexAttribArray(glGetAttribLocation(pgmMap, "Position")); /*vertex coords*/
	glVertexAttribPointer(glGetAttribLocation(pgmMap, "Position"), 2, GL_FLOAT, GL_FALSE, 0, verts);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(glGetAttribLocation(pgmMap, "Position"));
	glUseProgramObject(0);
}

void Renderer::drawCollisionVols(Scene* scene)
{
	Map* map = scene->getMap();

	int numVols = map->getNumberOfCollideVols();
	Rect rect;
	for (int i = 0; i < numVols; i++)
	{
		if (map->getCollideVolAt(i).active())
		{
			rect = map->getCollideVolAt(i).rect;
			rect.x -= scene->getCamera().x;
			rect.y -= scene->getCamera().y;

			if (map->getCollideVolAt(i).glass())
				drawRect2(rect, 0, 1, 1, 1);
#ifdef DEBUG
			else
			{
				drawRect2(rect, 0.40625f, 0.71875f, 0.91796875f, 1);
				// drawRect2(rect, 0, 0, 0, scene->inCrosslinkMode(), 1);
			}
#endif
		}
	}
}

void Renderer::drawEntities(Scene* scene)
{
	Map* map = scene->getMap();
	Rect cam = scene->getCamera();

	size_t numEnts = map->getNumberOfEnts();
	size_t numParticles = scene->getNumberOfParticles();
	Entity* ent;
	Particle* particle;
	vec2f position;
	size_t i;
	Rect vol;
	glBindTexture(GL_TEXTURE_2D, resObjects->getTexId());
	for (i = 0; i < numEnts; i++)
	{
		ent = map->getEntAt(i);
		position = ent->getPosition();
		vol = ent->getCollisionRect();
		vol.x -= scene->getCamera().x;
		vol.y -= scene->getCamera().y;

		if (ent->_highlighted)
		{
			drawRect2(vol, 1, 1, 1, 1);
		}

		if (dynamic_cast<LinkableEntity*>(ent) || dynamic_cast<Enemy*>(ent))
		{

		}
		else if (dynamic_cast<ElevatorDoor*>(ent))
		{
			ElevatorDoor* ed = static_cast<ElevatorDoor*>(ent);
			if (ed->isOpening() || ed->isClosing())
			{
				drawSprite(	position.x - cam.x,
				            position.y - cam.y,
				            1,
				            ent->getRotation(),
				            resObjects,
				            Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "elevatoropen"),
				            scene->inCrosslinkMode() ? SDM_PropCross : SDM_Normal, 1, 0, 1);
			}

			drawSprite(	position.x - cam.x,
			            position.y - cam.y,
			            (ed->isOpening() || ed->isClosing()) ? 2.75f : 1.5f,
			            ent->getRotation(),
			            resObjects,
			            ent->getCurrentSprite(),
			            scene->inCrosslinkMode() ? SDM_PropCross : SDM_Normal, 1, 0, 1);
		}
		else
		{
			drawSprite(	position.x - cam.x,
			            position.y - cam.y,
			            1,
			            ent->getRotation(),
			            resObjects,
			            ent->getCurrentSprite(),
			            scene->inCrosslinkMode() ? SDM_PropCross : SDM_Normal, 1, 0, 1);
		}
	}

	if (map->subwayFound())
	{
		drawSprite(	map->getSubwayPosition().x - cam.x,
		            map->getSubwayPosition().y - cam.y - ENTDIM,
		            3,
		            0,
		            resObjects,
		            Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "subwayexit"),
		            scene->inCrosslinkMode() ? SDM_PropCross : SDM_Normal, 1, 0, 1);
	}

	for (i = 0; i < map->getNumberOfStairs(); i++)
	{
		position = map->getStairsAt(i)->getPosition();

		drawSprite(	position.x - cam.x,
		            position.y - cam.y,
		            1,
		            0,
		            resObjects,
		            Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "stairs"), scene->inCrosslinkMode() ? SDM_PropCross : SDM_Normal, 0, 0, 0);
	}

	drawLinkableEntities(scene);

	for (i = 0; i < map->getNumberOfShafts(); i++)
	{
		if (map->getShaftAt(i)->_moving)
		{
			vol = map->getShaftAt(i)->getRect();
			vol.x -= scene->getCamera().x;
			vol.y -= scene->getCamera().y;
			drawRect2(vol, 0, 1, 1, 1);
		}
	}

	for (i = 0; i < numEnts; i++)
	{
		ent = map->getEntAt(i);
		position = ent->getPosition();
		vol = ent->getCollisionRect();
		Rect rect = {vol.x + 1 - scene->getCamera().x, vol.y + 4 - scene->getCamera().y, 4, 4};
		if (dynamic_cast<CircuitBox*>(ent))
		{
			CircuitBox* cb = static_cast<CircuitBox*>(ent);

			if (cb->isHacked())
			{
				drawRect2Fill(rect, 0, 0, 0, 2);
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, resGlass->getTexId());
	for (i = 0; i < numParticles; i++)
	{
		particle = scene->getParticleAt(i);
		position = particle->getPosition();
		vol = particle->getCollisionRect();
		vol.x -= scene->getCamera().x;
		vol.y -= scene->getCamera().y;
		if (particle->_alive)
		{
			drawSprite(	position.x - cam.x,
			            position.y - cam.y,
			            1,
			            0,
			            resGlass,
			            particle->getCurrentSprite(),
			            SDM_Normal, 1, 0, 1);
		}
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (i = 0; i < numEnts; i++)
	{
		ent = map->getEntAt(i);
		position = ent->getPosition();
		if (dynamic_cast<SecurityCamera*>(ent))
		{
			SecurityCamera* camera = static_cast<SecurityCamera*>(ent);
			if (scene->inCrosslinkMode() && !scene->isCircuitUnlocked(camera->getCircuitType()))
			{
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
			}
			drawFieldOfView(scene, ((SecurityCamera*)ent)->getFOV(), pgmCamera);
		}
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Renderer::drawEnemies(Scene* scene, bool crosslink)
{
	Map* map = scene->getMap();
	Rect cam = scene->getCamera();

	size_t numEnemies = map->getNumberOfEnemies();
	Enemy* enemy;
	SpriteSheet* sheet;
	vec2f position;
	size_t i;
	for (i = 0; i < numEnemies; i++)
	{
		enemy = map->getEnemyAt(i);
		position = enemy->getPosition();
		switch (enemy->getType())
		{
		case Enemy_Sniper:
			sheet = enemy->getDirection() == Right ? resSniperRight : resSniperLeft;
			break;
		case Enemy_Enforcer:
			sheet = enemy->getDirection() == Right ? resEnforcerRight : resEnforcerLeft;
			break;
		case Enemy_Professional:
			sheet = enemy->getDirection() == Right ? resProfessionalRight : resProfessionalLeft;
			break;
		case Enemy_Guard:
		default:
			sheet = enemy->getDirection() == Right ? resGuardRight : resGuardLeft;
			break;
		}
		glBindTexture(GL_TEXTURE_2D, sheet->getTexId());
		drawSprite(	position.x - cam.x,
		            position.y - cam.y,
		            2.5f,
		            enemy->getRotation(),
		            sheet,
		            enemy->getCurrentSprite(),
		            crosslink ? SDM_LinkableCross : SDM_Normal, 0, 0, 0);
#ifdef DEBUG

		sprintf(print, "%i", enemy->getAlertType());
		drawText(position.x - cam.x, position.y - cam.y, print, RGB_WHITE, 1.0f, font1);

		if (enemy->getTargetType() != TARGET_NONE)
		{
			drawLine(scene, 1, 0, 1, 1, enemy->getCollisionRectPosition(), enemy->getTarget(), 1);

		}
		if (enemy->getSecondaryTarget() != NULL)
		{
			drawLine(scene, 1, 1, 0, 1, enemy->getCollisionRectPosition(), enemy->getSecondaryTarget()->getCollisionRectPosition(), 1);
		}

		if (enemy->getStrongestLight() != NULL)
		{
			drawLine(scene, 1, 1, 1, 1, enemy->getCollisionRectCenterPosition(), enemy->getStrongestLight()->getPosition(), 1);
		}

		if (enemy->getLightToActivate() != NULL)
		{
			drawLine(scene, 0, 1, 0, 1, enemy->getCollisionRectCenterPosition(), enemy->getLightToActivate()->getPosition(), 1);
		}

		if (enemy->getState() != KNOCKED_OUT && enemy->getState() != FALLING /*&& scene->inCrosslinkMode()*/)
			drawEnemyFOV(scene, enemy);

#endif
	}
}

void Renderer::drawLinkableEntities(Scene* scene)
{
	Map* map = scene->getMap();
	std::vector<LinkableEntity*>::iterator linkBegin;
	std::vector<LinkableEntity*>::iterator linkEnd;
	std::vector<LinkableEntity*>::iterator linkIter;
	LinkableEntity* ent;
	map->getLinkableIters(&linkBegin, &linkEnd);
	float r, g, b;
	Rect vol;
	vec2f position;
	Rect cam = scene->getCamera();
	glBindTexture(GL_TEXTURE_2D, resLinkables->getTexId());
	for (linkIter = linkBegin; linkIter != linkEnd; ++linkIter)
	{
		ent = *linkIter;
		vol = ent->getCollisionRect();
		vol.x -= scene->getCamera().x;
		vol.y -= scene->getCamera().y;
		position = ent->getPosition();
		if (!scene->inCrosslinkMode())
		{
			r = 1.0f;
			g = 0.0f;
			b = 1.0f;
		}
		else
		{
			if (!scene->isCircuitUnlocked(ent->getCircuitType()) && ent->getCircuitType() != RED)
			{
				r = g = b = 0;
			}
			else
			{
				scene->getCircuitColor(ent->getCircuitType(), r, g, b);
			}
		}
		if (!dynamic_cast<EnemyGun*>(ent))
		{
			drawSprite(position.x - cam.x, position.y - cam.y, 1.5f, 0, resLinkables, ent->getCurrentSprite(), scene->inCrosslinkMode() ? SDM_LinkableCross : SDM_Normal, r, g, b);
		}
		else
		{
			if (scene->inCrosslinkMode())
				drawRect2(vol, 1, 0, 1, 2);
		}
	}
}

void Renderer::drawTutorialMarks(Scene* scene)
{
	Map* map = scene->getMap();
	std::vector<TutorialMark*>::iterator tutBegin;
	std::vector<TutorialMark*>::iterator tutEnd;
	std::vector<TutorialMark*>::iterator it;
	TutorialMark* tm;
	map->getTutorialIters(&tutBegin, &tutEnd);
	vec2f position;
	Rect cam = scene->getCamera();
	for (it = tutBegin; it != tutEnd; ++it)
	{
		tm = *it;
		position = tm->getPosition();
		drawSprite(	position.x - cam.x,
					position.y - cam.y,
					1,
					tm->getRotation(),
					resObjects,
					tm->getCurrentSprite(),
					SDM_Normal, 1, 0, 1);
	}
}

void Renderer::drawLights(Scene* scene)
{
	size_t numLights = scene->getMap()->getNumberOfLights();
	size_t i;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (i = 0; i < numLights; i++)
	{
		if (scene->getMap()->getLightAt(i)->getType() == FOV_LIGHT)
		{
			drawFieldOfView(scene, scene->getMap()->getLightAt(i), pgmLight);
		}
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Renderer::drawJumpTrajectory(Scene* scene)
{
	Rect cam = scene->getCamera();
	size_t numPoints = scene->getNumberOfTrajPoints();
	vec2f point;
	for (size_t i = 0; i < numPoints; i++)
	{
		point = scene->getTrajPointAt(i);
		drawRect(point.x - cam.x, point.y - cam.y, 2, pointVBO, 1, 1, 1, 1, true);
	}
}

void Renderer::drawLink(Scene* scene, Circuit c, vec2f pA, vec2f pB)
{
	float r, g, b;
	r = g = b = 0;

	if (scene->isCircuitUnlocked(c))
	{
		scene->getCircuitColor(c, r, g, b);
	}

	drawLine(scene, r, g, b, 1, pA, pB, 1.5);
}

void Renderer::drawLine(Scene* scene, float r, float g, float b, float alpha, vec2f pA, vec2f pB, float z)
{
	float verts[] = {pB.x - pA.x, pB.y - pA.y, 0.0f, 0.0f};
	glUseProgramObject(pgmMap);
	glUniform3f(glGetUniformLocation(pgmMap, "color"), r, g, b);
	glUniform1f(glGetUniformLocation(pgmMap, "alpha"), alpha);

	quadTransform = mat4f_mult(orthographic, mat4f_translate(pA.x - scene->getCamera().x, pA.y - scene->getCamera().y, z));
	glUniformMatrix4fv(glGetUniformLocation(pgmMap, "gWorld"), 1, GL_TRUE, &quadTransform.m[0][0]);

	glEnableVertexAttribArray(glGetAttribLocation(pgmMap, "Position")); /*vertex coords*/
	glVertexAttribPointer(glGetAttribLocation(pgmMap, "Position"), 2, GL_FLOAT, GL_FALSE, 0, verts);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_LINES, 0, 2);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(glGetAttribLocation(pgmMap, "Position"));
}

void Renderer::drawCrossLink(Scene* scene)
{
	if (!scene->inCrosslinkMode())
		return;

	Map* map = scene->getMap();
	Rect cam = scene->getCamera();
	Circuit c;
	std::vector<LinkableEntity*>::iterator linkBegin;
	std::vector<LinkableEntity*>::iterator linkEnd;
	std::vector<LinkableEntity*>::iterator linkIter;
	LinkableEntity* le;
	map->getLinkableIters(&linkBegin, &linkEnd);
	for (linkIter = linkBegin; linkIter != linkEnd; ++linkIter)
	{
		le = *linkIter;
		c = le->getCircuitType();

		if (le->getTarget())
		{
			vec2f v1  = le->getCollisionRectCenterPosition();
			vec2f v2 = le->getTarget()->getCollisionRectCenterPosition();

			distance = vec2f(v2.x - v1.x, v2.y - v1.y);
			progress = vec2f_normalize(distance);
			progress = progress + v1;
			distance = distance * linkProgress;
			progress = progress + distance;

			if (scene->isCircuitUnlocked(c))
			{
				drawRect(progress.x - cam.x, progress.y - cam.y, 1.5, pointVBO, 1, 1, 1, 1, true);
			}
			else
			{
				drawRect(progress.x - cam.x, progress.y - cam.y, 1.5, pointVBO, 0, 0, 0, 1, true);
			}
			drawLink(scene, c, v1, v2);
		}
		else
		{
			if (le == scene->getLinker() && scene->isPlayerSelecting())
			{
				drawLink(scene, c, le->getCollisionRectCenterPosition(), scene->getMouseDragPosition());
			}
		}
	}
	glUseProgramObject(0);
}

void Renderer::drawEnemyFOV(Scene* scene, Enemy* enemy)
{
	vec2f position = enemy->getCollisionRectCenterPosition();
	int negMod;
	int radius = enemy->getType() == Enemy_Sniper ? ENEMY_FOV_RADIUS_SNIPER : scene->isPlayerInLight() ? ENEMY_FOV_RADIUS_LIT : ENEMY_FOV_RADIUS_DARK;
	float angle = enemy->getType() == Enemy_Sniper ? ENEMY_FOV_HALFANGLE_SEEN : enemy->canSeePlayer() ? ENEMY_FOV_HALFANGLE_SEEN : ENEMY_FOV_HALFANGLE;

	if (enemy->getDirection() == Right)
	{
		negMod = 1;
	}
	else
	{
		negMod = -1;
	}

	float verts[] = {	position.x, position.y,
	                    radius * sinf(ToRadian(-angle + 90)) * negMod  + position.x, radius * cosf(ToRadian(-angle + 90)) + position.y,
	                    radius * sinf(ToRadian( 90)) * negMod + position.x, radius * cosf(ToRadian( 90)) + position.y,
	                    radius * sinf(ToRadian(angle + 90)) * negMod + position.x, radius * cosf(ToRadian(angle + 90)) + position.y
	                };

	glUseProgramObject(pgmMap);
	glUniform3f(glGetUniformLocation(pgmMap, "color"), 0, 0, 1);

	quadTransform = mat4f_mult(orthographic, mat4f_translate(-scene->getCamera().x, -scene->getCamera().y, 1.5));

	glUniformMatrix4fv(glGetUniformLocation(pgmMap, "gWorld"), 1, GL_TRUE, &quadTransform.m[0][0]);

	glEnableVertexAttribArray(glGetAttribLocation(pgmMap, "Position")); /*vertex coords*/
	glVertexAttribPointer(glGetAttribLocation(pgmMap, "Position"), 2, GL_FLOAT, GL_FALSE, 0, verts);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableVertexAttribArray(glGetAttribLocation(pgmMap, "Position"));
}

void Renderer::drawTileLayer(Scene* scene, int z)
{
	glUseProgramObject(pgmButton);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene->inCrosslinkMode() ? scene->getMap()->getCrosslinkTexture() : scene->getMap()->getMapTexture());

	float verts[] = {	0.0f, 0.0f,
	                    0.0f, (float)scene->getMap()->getMapHeight(),
	                    (float)scene->getMap()->getMapWidth(), 0.0f,
	                    0.0f, (float)scene->getMap()->getMapHeight(),
	                    (float)scene->getMap()->getMapWidth(), 0.0f,
	                    (float)scene->getMap()->getMapWidth(), (float)scene->getMap()->getMapHeight(),
	                };

	float texes[] = {	0.0f, 0.0f,
	                    0.0f, 1.0f,
	                    1.0f, 0.0f,
	                    0.0f, 1.0f,
	                    1.0f, 0.0f,
	                    1.0f, 1.0f,
	                };

	quadTransform = mat4f_mult(orthographic, mat4f_translate(0 - scene->getCamera().x, 0 - scene->getCamera().y, z));
	glUniformMatrix4fv(glGetUniformLocation(pgmButton, "proj_mat"), 1, GL_TRUE, &quadTransform.m[0][0]);
	glEnableVertexAttribArray(glGetAttribLocation(pgmButton, "Position")); /*vertex coords*/
	glEnableVertexAttribArray(glGetAttribLocation(pgmButton, "TexCoord")); /*texture coords*/
	glVertexAttribPointer(glGetAttribLocation(pgmButton, "Position"), 2, GL_FLOAT, GL_FALSE, 0, verts);
	glVertexAttribPointer(glGetAttribLocation(pgmButton, "TexCoord"), 2, GL_FLOAT, GL_FALSE, 0, texes);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(glGetAttribLocation(pgmButton, "Position"));
	glDisableVertexAttribArray(glGetAttribLocation(pgmButton, "TexCoord"));

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgramObject(0);
}

void Renderer::drawBackground(Scene* scene, GLuint tex, int x, int z, float offset)
{
	GLuint pgm;
	if (scene->inCrosslinkMode() && !_crosslinkBlur)
	{
		pgm = pgmCrosslinkProp;
	}
	else
	{
		pgm = pgmButton;
	}
	glUseProgramObject(pgm);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	unsigned int should_blur = scene->inCrosslinkMode() && _crosslinkBlur ? 1 : 0;
	glUniform1i(glGetUniformLocation(pgmButton, "use_blur"), should_blur);
	float xOffset = scene->getCamera().x * offset;

	float verts[] = {	0.0f, 0.0f,
	                    0.0f, (float)BACKGROUND_HEIGHT,
	                    (float)BACKGROUND_WIDTH, 0.0f,
	                    0.0f, (float)BACKGROUND_HEIGHT,
	                    (float)BACKGROUND_WIDTH, 0.0f,
	                    (float)BACKGROUND_WIDTH, (float)BACKGROUND_HEIGHT,
	                };

	float texes[] = {	0.0f, 0.0f,
	                    0.0f, 1.0f,
	                    1.0f, 0.0f,
	                    0.0f, 1.0f,
	                    1.0f, 0.0f,
	                    1.0f, 1.0f,
	                };

	quadTransform = mat4f_mult(orthographic, mat4f_translate(x - xOffset,
		scene->getMap()->getBackGroundYOffset() - BACKGROUND_HEIGHT - scene->getCamera().y, z));
	glUniformMatrix4fv(glGetUniformLocation(pgmButton, "proj_mat"), 1, GL_TRUE, &quadTransform.m[0][0]);
	glEnableVertexAttribArray(glGetAttribLocation(pgmButton, "Position")); /*vertex coords*/
	glEnableVertexAttribArray(glGetAttribLocation(pgmButton, "TexCoord")); /*texture coords*/
	glVertexAttribPointer(glGetAttribLocation(pgmButton, "Position"), 2, GL_FLOAT, GL_FALSE, 0, verts);
	glVertexAttribPointer(glGetAttribLocation(pgmButton, "TexCoord"), 2, GL_FLOAT, GL_FALSE, 0, texes);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(glGetAttribLocation(pgmButton, "Position"));
	glDisableVertexAttribArray(glGetAttribLocation(pgmButton, "TexCoord"));

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(pgmButton, "use_blur"), 0);
	glUseProgramObject(0);
}

void Renderer::getBoxCoordsAroundText(const char* text, float x, float y, Font* font, Rect* rect)
{
	int longestWidth = 0;
	int widthCounter = 0;
	int height = 1;
	const char* c;
	for (c = text; *c != '\0'; c++)
	{
		if (*c != '\n')
		{
			widthCounter++;
		}
		else
		{
			if (widthCounter > longestWidth)
			{
				longestWidth = widthCounter;
				widthCounter = 0;
			}
			height++;
		}
	}

	if (longestWidth == 0)
	{
		longestWidth = strlen(text);
	}

	rect->x = x - font->getSize();
	rect->y = y -  font->getSize();
	rect->w = ((longestWidth / 2.0f) + 2.5f) * font->getSize();
	rect->h = (height * font->getSize()) + 16;
}

//more expensive than above function - used only when strings are changed.
void Renderer::getBoxDimsAroundText(const char* text, Font* font, vec2f* dims)
{
	int height = font->getSize();
	const char* c;
	float x, y, longestWidth;
	x = y = longestWidth = 0;
	stbtt_aligned_quad q;
	for (c = text; *c != '\0'; c++)
	{
			if (*c == '\n')
			{
				height += font->getSize();
				y += font->getSize();
				x = 0;
				continue;
			}

			stbtt_GetBakedQuad(font->data(), 512, 512, *c - 32, &x, &y, &q, 1);
			if (q.x1 > longestWidth)
			{
				longestWidth = q.x1;
			}
	}

	if (longestWidth == 0)
	{
		longestWidth = strlen(text) * font->getSize();
	}

	dims->x = longestWidth + (2 * font->getSize());
	dims->y = height + (font->getSize() / 2);
}

void Renderer::drawScene(Scene* scene)
{
	unsigned int mapWidth = scene->getMap()->getMapWidth();
	unsigned int bgCounter = 0;
	Rect cam = scene->getCamera();
	Player* player = scene->getPlayer();
	Rect playerRect = player->getCollisionRect();

	drawTileLayer(scene, 0);

	while (bgCounter < mapWidth)
	{
		drawBackground(scene, bgClose, bgCounter, 0, 1.0f);
		drawBackground(scene, bgMiddle, bgCounter, -1, 0.75f);
		drawBackground(scene, bgFar, bgCounter, -2, 0.5f);
		drawBackground(scene, bgVeryFar, bgCounter, -3, 0.25f);
		bgCounter += BACKGROUND_WIDTH;
	}

	if (!scene->inCrosslinkMode())
	{
		if ((!player->isInElevator() || !player->getElevatorDoor()->_shaft->_moving) && !player->isPinning())
		{
			glBindTexture(GL_TEXTURE_2D, player->getDirection() == Right ? resPlayer->getTexId() : resPlayerLeft->getTexId());
			drawSprite(	scene->getPlayerPosition().x - cam.x,
			            scene->getPlayerPosition().y - cam.y,
			            player->isInElevator() ? 2.5f : 2.8f,
			            player->getRotation(),
			            player->getDirection() == Right ? resPlayer : resPlayerLeft,
			            player->getCurrentSprite(),
			            SDM_Normal, 0, 0, 0);
			if (player->isAimingGun())
			{
				unsigned int spr = Locator::getSpriteManager()->getIndex("./data/sprites/player.sprites", "player_arm_gun");
				drawSprite(	scene->getPlayerPosition().x - cam.x,
				            scene->getPlayerPosition().y - cam.y - 4,
				            2.8f,
				            player->_armRotation,
				            player->getDirection() == Right ? resPlayer : resPlayerLeft,
				            spr,
				            SDM_Normal, 0, 0, 0);
				drawLine(scene, 1, 0, 0, 0.5f, player->getCollisionRectCenterPosition(), scene->getLaserEnd(), 2.5f);
			}
		}
		drawEnemies(scene, false);
	}

	drawCollisionVols(scene);

	if (scene->inCrosslinkMode())
	{
		drawLights(scene);
		drawEntities(scene);
	}
	else
	{
		drawEntities(scene);
		drawLights(scene);
	}

	if (Locator::getConfigManager()->getBool("tutorial_popups"))
	{
		glBindTexture(GL_TEXTURE_2D, resObjects->getTexId());
		drawTutorialMarks(scene);
	}

	drawCrossLink(scene);
	drawInterface(scene);

	if (scene->inCrosslinkMode())
	{
		//if in crosslink mode, draw black sprites of enemies and player last so that color is not affected by lights.
		if ((!player->isInElevator() || !player->getElevatorDoor()->_shaft->_moving) && !player->isPinning())
		{
			drawSpriteBind( scene->getPlayerPosition().x - cam.x,
			                scene->getPlayerPosition().y - cam.y,
			                2.5f,
			                player->getRotation(),
			                player->getDirection() == Right ? resPlayer : resPlayerLeft,
			                player->getCurrentSprite(),
			                SDM_LinkableCross, 0, 0, 0);
		}
		drawEnemies(scene, true);
		sprintf(print, "Energy: %i", scene->_playerEnergy);
		drawText(32, 160, print, RGB_WHITE, 1.0f, font1);
	}

	drawJumpTrajectory(scene);

	MouseOverObject moo = scene->getObjectMousedOver();
	if (moo < NUMBER_OF_MOUSEOVER_OBJECTS)
	{
		float xOff = (strlen(_mouseOverStrings[moo]) * font2->getSize()) / 4.0f;
		messageBox.x = (winX / 2) - xOff - font2->getSize();
		messageBox.y = winY - 32 - font2->getSize();
		messageBox.w = _mouseOverDims[moo].x;
		messageBox.h = _mouseOverDims[moo].y;
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		drawRect2Fill(messageBox, 0.5f, 0.5f, 0.5f, 3.0f);
		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_BLEND);
		drawText((winX / 2) - xOff, winY - 32, _mouseOverStrings[moo], RGB_WHITE, 1.0f, font2);
	}
	if (scene->getStringMessageTime() >= 0)
	{
		StringMessage sm = scene->getStringMessage();
		float xOff = (strlen(_messageStrings[sm]) * font2->getSize()) / 4.0f;
		getBoxCoordsAroundText(_messageStrings[sm], (winX / 2) - xOff, winY - 32, font2, &messageBox);
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		drawRect2Fill(messageBox, 0.5f, 0.5f, 0.5f, 3.0f);
		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_BLEND);
		drawText((winX / 2) - xOff, winY - 32, _messageStrings[sm], RGB_WHITE, 1.0f, font2);
	}

	if (scene->hasPlayerFiredShot() || player->isAimingGun())
	{
		sprintf(print, "%i", (scene->_timeToSniper / 1000));
		if (scene->hasPlayerFiredShot())
		{
			drawText(winX - 64, winY - 32, print, RGB_RED, 1.0f, font1);
		}
		else
		{
			drawText(winX - 64, winY - 32, print, RGB_WHITE, 1.0f, font1);
		}
		if (player->isAimingGun())
		{
			sprintf(print, "Ammo: %i", scene->_numPlayerBullets);
			drawText(32, 128, print, RGB_WHITE, 1.0f, font1);
		}
	}

	if (scene->getFirstOverlappedEnt() < NumUsableEnts)
	{
		float oneMinusAlpha = 1.0f - (0.5f * scene->getInputPopupAlpha());
		messageBox.x = playerRect.x - cam.x - font2->getSize();
		messageBox.y = playerRect.y + playerRect.h - cam.y;
		messageBox.w = _bindingBoxDims[scene->getFirstOverlappedEnt()].x;
		messageBox.h = _bindingBoxDims[scene->getFirstOverlappedEnt()].y;
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		drawRect2Fill(messageBox, oneMinusAlpha, oneMinusAlpha, oneMinusAlpha, 3.0f);
		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_BLEND);
		drawText(playerRect.x - cam.x, playerRect.y + playerRect.h + font2->getSize() - cam.y, _bindingStrings[scene->getFirstOverlappedEnt()].c_str(), 0.56f, 0.44f, 0.33f, scene->getInputPopupAlpha(), font2);
	}

	if (_enteredLightFlash)
	{
		drawRect(0, 0, 1.5f, _screenVBO, 1, 1, 1, scene->getLightEnteredAlpha(), true);
	}
#ifdef DEBUG
	drawDebugSceneText(scene);
#endif
}

void Renderer::updateLinkProgress(unsigned int dT)
{
	linkProgress += ((float)dT / 1000.0f);
	if (linkProgress > 1.0f)
		linkProgress = 0.0f;
}

void Renderer::drawDebugSceneText(Scene* scene)
{
	px = scene->getPlayer()->getCollisionRectPosition().x;
	py = scene->getPlayer()->getCollisionRectPosition().y;

	sprintf(print, "(%f, %f)", px, py);
	px = scene->getPlayerPosition().x;
	py = scene->getPlayerPosition().y;
	drawText(px - scene->getCamera().x, py - scene->getCamera().y, print, RGB_WHITE, 1.0f, font1);
	drawText(32, 32, (char*)"On Ground: ", RGB_WHITE, 1.0f, font1);
	if (scene->getPlayer()->_onGround)
		drawText(192, 32, (char*)"Yes", 0.0f, 1.0f, 0.0f, 1.0f, font1);
	else
		drawText(192, 32, (char*)"No", RGB_RED, 1.0f, font1);

	sprintf(print, "Attach Type: %i", scene->getPlayer()->getAttachType());
	drawText(32, 64, print, RGB_WHITE, 1.0f, font1);
	sprintf(print, "Light: %i", scene->getPlayer()->_lightVisibility);
	drawText(32, 96, print, RGB_WHITE, 1.0f, font1);
}

void Renderer::drawInterface(Scene* scene)
{
	Rect gem = {32, 32, 32, 32};
	if (scene->isPlayerInLight())
	{
		drawRect2Fill(gem, 1, 1, 1, 2);
	}
	else
	{
		drawRect2(gem, 1, 1, 1, 2);
	}

	if (scene->isLoadMenuVisible())
	{
		drawLoadMenu(/*scene*/);
	}
}

void Renderer::drawLoadMenu(/*Scene* scene*/)
{
	int incr = 32;
	// int index = 1;
	// unsigned int i;

	// drawText(winX / 2, winY / 2, (char*)"Select a save to load.", RGB_WHITE, 1.0f, 1.0f, atlas);

	// for (i = 0; i < MAX_SAVES; i++)
	// {
	// if (scene->getSaveTimeAt(i) >= 0)
	// {
	// sprintf(print, (char*)"%i: Autosave %i seconds ago", index, scene->getSecondsSince(i));
	// drawText(winX / 2, winY / 2 + incr, print, RGB_WHITE, 1.0f, 1.0f, atlas);
	// index++;
	// incr += 32;
	// }
	// }

	drawText(winX / 2, winY / 2 + incr, (char*)"R: Restart", RGB_WHITE, 1.0f, font2);
	incr += 32;
	drawText(winX / 2, winY / 2 + incr, (char*)"F9: Load quick save", RGB_WHITE, 1.0f, font2);
	incr += 32;
	drawText(winX / 2, winY / 2 + incr, (char*)"ESC: Abort", RGB_WHITE, 1.0f, font2);
}

unsigned int Renderer::getScreenshotIndex()
{
	return screenshotIndex;
}

void Renderer::setScreenshotIndex(unsigned int value)
{
	screenshotIndex = value;
}

void Renderer::handleSettingsChange()
{
	_enteredLightFlash = (Locator::getConfigManager()->getBool("entered_light_flash"));
	_crosslinkBlur = (Locator::getConfigManager()->getBool("crosslink_blur"));
	_bindingStrings[UESwitch] = "Flip Switch: " + Locator::getBindingsManager()->getFirstKeyBound(Bind_MoveUp);
	_bindingStrings[UETerminal] = "Hack: " + Locator::getBindingsManager()->getFirstKeyBound(Bind_MoveUp);
	_bindingStrings[UEElevator] = "Move Up:   " + Locator::getBindingsManager()->getFirstKeyBound(Bind_MoveUp) + "\nMove Down: " +
	                              Locator::getBindingsManager()->getFirstKeyBound(Bind_MoveDown);
	_bindingStrings[UEStairs] = _bindingStrings[UEElevator];
	_bindingStrings[UECircuitBox] = "Bypass: " + Locator::getBindingsManager()->getFirstKeyBound(Bind_MoveUp);
	_bindingStrings[UEEnemy] = "Punch: LMB";
	_bindingStrings[UEEnemyKnockedOut] = "Punch:   LMB\nGet off: " + Locator::getBindingsManager()->getFirstKeyBound(Bind_MoveLeft)
	+ "/" + Locator::getBindingsManager()->getFirstKeyBound(Bind_MoveRight);

	for (size_t i = 0; i < NumUsableEnts; i++)
	{
		getBoxDimsAroundText(_bindingStrings[i].c_str(), font2, &_bindingBoxDims[i]);
	}
}

void Renderer::addShader(GLuint shader_program, const char* shader_text, GLuint shader_id)
{
	if (shader_id == 0)
	{
		LOGF((stderr, "Error creating shader!\n"));
		exit(0);
	}

	const GLchar* p[1];
	p[0] = shader_text;
	GLint Lengths[1];
	Lengths[0]= strlen(shader_text);
	glShaderSource(shader_id, 1, p, Lengths);
	glCompileShader(shader_id);
	GLint success;
	glGetObjectParameteriv(shader_id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];
		glGetInfoLog(shader_id, 1024, NULL, InfoLog);
		LOGF((stderr, "Error compiling shader type: '%s'\n", InfoLog));
		exit(1);
	}
	else
	{
		LOGF((stdout, "Compiled shader type!\n"));
	}

	glAttachShader(shader_program, shader_id);
}

GLuint Renderer::compileShaders(const char* vert_filename, const char* frag_filename)
{
	GLuint program = glCreateProgramObject();

	if (!program)
	{
		LOGF((stderr, "Error creating shader program from %s and %s!\n", vert_filename, frag_filename));
		exit(1);
	}

	GLuint vert_shader = glCreateShaderObject(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShaderObject(GL_FRAGMENT_SHADER);

	char* vertString = file_read(vert_filename, NULL);
	char* fragString = file_read(frag_filename, NULL);

	addShader(program, vertString, vert_shader);
	addShader(program, fragString, frag_shader);

	GLint link_successful = 0;
	GLchar error[1024] = {0};

	glLinkProgram(program);
	glGetObjectParameteriv(program, GL_LINK_STATUS, &link_successful);
	if (!link_successful)
	{
		glGetInfoLog(program, sizeof(error), NULL, error);
		LOGF((stderr, "Error linking shader program: '%s'\n", error));
		Assert(false);
	}

	glValidateProgram(program);
	glGetObjectParameteriv(program, GL_VALIDATE_STATUS, &link_successful);

	if (!link_successful)
	{
		glGetInfoLog(program, sizeof(error), NULL, error);
		LOGF((stderr, "Invalid shader program: '%s'\n", error));
		Assert(false);
	}

	delete [] vertString;
	delete [] fragString;

	glDeleteObject(vert_shader);
	glDeleteObject(frag_shader);

	return program;
}