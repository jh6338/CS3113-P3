#define GL_SILENCE_DEPRECATION
#include <vector> 

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 14
#define LANDING_COUNT 2 

/*
Joyce Huang
February 16th, 2020
CS-UY 3113 Intro to Game Programming
Project 3: Lunar Lander 
*/

/*
Instructions: Make a simple version of Lunar Lander 
- the player should fall with gravity (make it very low so it falls slowly) 
- Moving left or right should change acceleration instead of velocity 
- If the player touches a wall or rock show text "Mission Failed" 
- If player touches the platform show "Mission Successful" 
- You can use whatever graphics/themes as long as you meet the requirements 
*/

struct GameState {
	Entity* player; 
	Entity* platforms;
	Entity* landing; 
};

GameState state;

SDL_Window * displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;
glm::mat4 modelMatrix;

bool success = false; 
bool gameEnd = false; 

GLuint LoadTexture(const char* filepath);
void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position); 

GLuint fontTextureID;

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Project 3: Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);
	// Load the shaders for handling textures 
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl"); // for the textures 

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	// program.SetColor(0.0f, 0.0f, 1.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// Enable blending 
	glEnable(GL_BLEND);

	// Good for transparency 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize Game Objects

	// Initialize Player
	state.player = new Entity();
	state.player->position = glm::vec3(0, 3.25f, 0);
	state.player->movement = glm::vec3(0);
	state.player->acceleration = glm::vec3(0, -0.1f, 0);
	state.player->speed = 1.0f;
	state.player->textureID = LoadTexture("playerShip3_orange.png");

	/*
	state.player->animRight = new int[4]{ 3, 7, 11, 15 };
	state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
	state.player->animUp = new int[4]{ 2, 6, 10, 14 };
	state.player->animDown = new int[4]{ 0, 4, 8, 12 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 4;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 4;
	state.player->animRows = 4;

	state.player->height = 0.8f;
	state.player->width = 0.8f;

	state.player->jumpPower = 5.0f;
	*/

	// Initialize Platforms

	state.platforms = new Entity[PLATFORM_COUNT];
	GLuint platformTextureID = LoadTexture("platformPack_tile040.png");

	for (int i = 0; i < PLATFORM_COUNT; ++i) {
		state.platforms[i].entityType = PLATFORM; 
	}

	state.platforms[0].textureID = platformTextureID;
	state.platforms[0].position = glm::vec3(-1, -3.25f, 0);

	state.platforms[1].textureID = platformTextureID;
	state.platforms[1].position = glm::vec3(0, -3.25f, 0);

	state.platforms[2].textureID = platformTextureID;
	state.platforms[2].position = glm::vec3(1, -3.25f, 0);

	state.platforms[3].textureID = platformTextureID;
	state.platforms[3].position = glm::vec3(-2, -3.25f, 0);

	state.platforms[4].textureID = platformTextureID;
	state.platforms[4].position = glm::vec3(2.4f, 0.5f, 0);

	state.platforms[5].textureID = platformTextureID;
	state.platforms[5].position = glm::vec3(-3, -3.25f, 0);

	state.platforms[6].textureID = platformTextureID;
	state.platforms[6].position = glm::vec3(3.4f, 0.5f, 0);

	state.platforms[7].textureID = platformTextureID;
	state.platforms[7].position = glm::vec3(-4, -3.25f, 0);

	state.platforms[8].textureID = platformTextureID;
	state.platforms[8].position = glm::vec3(4, -3.25f, 0);

	state.platforms[9].textureID = platformTextureID;
	state.platforms[9].position = glm::vec3(-5, -3.25f, 0);

	state.platforms[10].textureID = platformTextureID;
	state.platforms[10].position = glm::vec3(5, -3.25f, 0);

	state.platforms[11].textureID = platformTextureID;
	state.platforms[11].position = glm::vec3(-2.4f, 0.5f, 0);

	state.platforms[12].textureID = platformTextureID;
	state.platforms[12].position = glm::vec3(-3.4f, 0.5f, 0);

	state.platforms[13].textureID = platformTextureID; 
	state.platforms[13].position = glm::vec3(-0.5f, -0.5f, 0); 

	// Initialize Landings 

	state.landing = new Entity[LANDING_COUNT];
	GLuint landingTextureID = LoadTexture("platformPack_tile013.png");

	for (int i = 0; i < LANDING_COUNT; ++i) {
		state.landing[i].entityType = LANDING; 
	}

	state.landing[0].textureID = landingTextureID; 
	state.landing[0].position = glm::vec3(2, -3.25f, 0); 

	state.landing[1].textureID = landingTextureID; 
	state.landing[1].position = glm::vec3(3, -3.25f, 0); 

	for (int i = 0; i < PLATFORM_COUNT; ++i) {
		state.platforms[i].Update(0, NULL, 0);
	}

	for (int i = 0; i < LANDING_COUNT; ++i) {
		state.landing[i].Update(0, NULL, 0); 
	}

	fontTextureID = LoadTexture("font1.png");

}

void ProcessInput() {
	state.player->movement = glm::vec3(0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
				// Move the player left
				break;

			case SDLK_RIGHT:
				// Move the player right
				break;

			case SDLK_SPACE:
				// Some sort of action
				break;
			}
			break; // SDL_KEYDOWN
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	// Using the arrow keys
	if (keys[SDL_SCANCODE_LEFT]) {
		state.player->movement.x = -1.0f;
		// state.player->animIndices = state.player->animLeft;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		state.player->movement.x = 1.0f;
		// state.player->animIndices = state.player->animRight;
	}

	/*
	if (keys[SDL_SCANCODE_UP]) { 
		state.player->movement.y = 1.0f;
		// state.player->animIndices = state.player->animUp;
	}
	else if (keys[SDL_SCANCODE_DOWN]) { 
		state.player->movement.y = -1.0f;
		// state.player->animIndices = state.player->animDown;
	}

	
	// WASD 
	if (keys[SDL_SCANCODE_A]) {
		state.player->movement.x = -1.0f;
	}
	else if (keys[SDL_SCANCODE_D]) {
		state.player->movement.x = 1.0f;
	}

	if (keys[SDL_SCANCODE_W]) { // up
		state.player->movement.y = 1.0f;
	}
	else if (keys[SDL_SCANCODE_S]) { // down
		state.player->movement.y = -1.0f;
	}
	

	if (keys[SDL_SCANCODE_SPACE]) {
		startGame = true;
	}
	*/

	// holding x and y axis movement keys won't make you go faster 
	if (glm::length(state.player->movement) > 1.0f) {
		state.player->movement = glm::normalize(state.player->movement);
	}

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {
		// Update. Notice it's FIXED_TIMESTEP. Not deltaTime
		state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
		state.player->Update(FIXED_TIMESTEP, state.landing, LANDING_COUNT);

		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;

	/*
	if (state.player->lastCollided == PLATFORM) {
		state.player->isActive = false; 
		gameEnd = true; 
		success = false; 
	}
	else if (state.player->lastCollided == LANDING) {
		state.player->isActive = false; 
		gameEnd = true; 
		success = true; 
	}
	*/

	// /*
	switch (state.player->lastCollided) {
		case NONE: 
			break; 
		case PLATFORM: 
			gameEnd = true; 
			success = false;
			state.player->isActive = false;
			break; 
		case LANDING: 
			gameEnd = true; 
			success = true; 
			state.player->isActive = false;
			break; 
	}
	// */

	/*
	if (!gameEnd) {
		for (int i = 0; i < PLATFORM_COUNT; ++i) {

			Entity* object = &state.platforms[i];

			// if (state.player->CheckCollision(object)) { // if we collided with wrong platforms 
			if (state.player->collidedBottom) { // if we collided with wrong platforms 
				// mission failed  
				state.player->isActive = false;
				gameEnd = true;
				success = false;
				break;
			}
		}
	}

	if (!gameEnd) {
		for (int i = 0; i < LANDING_COUNT; ++i) {
			
			Entity* object = &state.landing[i]; 

			// if (state.player->CheckCollision(object)) { // if we collided with proper landing 
			if (state.player->collidedBottom) { // if we collided with proper landing 
				// mission success 
				state.player->isActive = false;
				gameEnd = true;
				success = true;
				break;
			}
		}
	}
	
	*/
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < PLATFORM_COUNT; ++i) {
		state.platforms[i].Render(&program);
	}

	for (int i = 0; i < LANDING_COUNT; ++i) {
		state.landing[i].Render(&program); 
	}

	if (gameEnd) {
		if (success) {
			DrawText(&program, fontTextureID, "Mission Successful!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.4f, 0));
		}
		else {
			DrawText(&program, fontTextureID, "Mission Failed", 0.5f, -0.25f, glm::vec3(-4.75f, 3.4f, 0));
		}
	}

	state.player->Render(&program);

	SDL_GL_SwapWindow(displayWindow);
}

GLuint LoadTexture(const char* filepath) {
	int w, h, n;
	unsigned char* image = stbi_load(filepath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
	float size, float spacing, glm::vec3 position)
{
	float width = 1.0f / 16.0f;
	float height = 1.0f / 16.0f;

	std::vector<float> vertices;
	std::vector<float> texCoords;

	for (int i = 0; i < text.size(); i++) {

		int index = (int)text[i];
		float offset = (size + spacing) * i;
		float u = (float)(index % 16) / 16.0f;
		float v = (float)(index / 16) / 16.0f;
		vertices.insert(vertices.end(), {
		 offset + (-0.5f * size), 0.5f * size,
		 offset + (-0.5f * size), -0.5f * size,
		 offset + (0.5f * size), 0.5f * size,
		 offset + (0.5f * size), -0.5f * size,
		 offset + (0.5f * size), 0.5f * size,
		 offset + (-0.5f * size), -0.5f * size,
			});
		texCoords.insert(texCoords.end(), {
		u, v,
		u, v + height,
		u + width, v,
		u + width, v + height,
		u + width, v,
		u, v + height,
			});

	} // end of for loop

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);

	glUseProgram(program->programID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, fontTextureID);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}