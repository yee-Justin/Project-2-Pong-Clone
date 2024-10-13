/**
* Author: Justin Yee
* Assignment: Simple 2D Scene: A Corny Game
* Date due: 2024-10-12, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>


/*Sources
https://pngtree.com/freepng/green-grass-clipart_13155454.html
https://pngtree.com/freepng/cartoon-pixel-corn-illustration_4586099.html
https://pngtree.com/freepng/corn-kernels-vegetable-color-meal_6624269.html (Pixelated using a pixelation website and brightened)
https://www.pngegg.com/en/png-fzcyd
https://pngtree.com/freepng/win-popup-victory-dialog-with-red-ribbon_6542902.html
https://opengameart.org/content/seamless-pattern-pack-dirtpng
*/

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 2.0f;

constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

//matrices
glm::mat4 g_view_matrix, g_projection_matrix,

g_corn_matrix, g_corn2_matrix,

g_kernel_matrix,

g_background_matrix,

g_top_grass_matrix, g_bottom_grass_matrix, g_left_grass_matrix, g_right_grass_matrix,

g_gameover_matrix,

g_win_matrix,

g_start_matrix;

//texture ids
GLuint g_corn_texture_id,
g_kernel_texture_id,
g_background_texture_id,
g_grass_texture_id,
g_vertical_grass_texture_id,
g_gameover_texture_id,
g_win_texture_id,
g_start_texture_id;

//constants
constexpr float MILLISECONDS_IN_SECOND = 1000.0;

float g_previous_ticks = 0.0f;

//file paths
constexpr char CORN_FILEPATH[] = "corn.png",
KERNEL_FILEPATH[] = "kernel.png",
BACKGROUND_FILEPATH[] = "background.png",
GRASS_FILEPATH[] = "grass.png",
VERTICAL_GRASS_FILEPATH[] = "vertical_grass.png",
GAMEOVER_FILEPATH[] = "gameover.png",
WIN_FILEPATH[] = "win.png",
START_FILEPATH[] = "start.png"; //Handdrawn by me

//Initial scaling 
constexpr glm::vec3 INIT_SCALE = glm::vec3(5.0f, 5.98f, 0.0f),
INIT_SCALE_CORN = INIT_SCALE * glm::vec3(0.025f, .20f, 0.0f),
INIT_SCALE_KERNEL = INIT_SCALE * glm::vec3(0.02f, 0.02f, 0.0f),
INIT_SCALE_GRASS = INIT_SCALE * glm::vec3(2.0f, .250f, 0.0f),
INIT_SCALE_VERTICAL_GRASS = INIT_SCALE * glm::vec3(.250f, 2.0f, 0.0f),
INIT_SCALE_BACKGROUND = INIT_SCALE * glm::vec3(3.0f, 3.0f, 0.0f),
INIT_SCALE_GAMEOVER = INIT_SCALE * glm::vec3(.50f, .50f, 0.0f),
INIT_SCALE_WIN = INIT_SCALE * glm::vec3(.25f, .25f, 0.0f),
INIT_SCALE_START = INIT_SCALE * glm::vec3(2.50f, .50f, 0.0f);

constexpr float KERNEL_SPEED = 6.0f; // speed of kernel
constexpr float CORN_SPEED = 8.0f; // speed of corn

//Initial positions
constexpr glm::vec3
INIT_POS_CORN = glm::vec3(-3.5f, 0.0f, 0.0f),
INIT_POS_CORN2 = glm::vec3(3.5f, 0.0f, 0.0f),
INIT_POS_KERNEL = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_POS_TOP_GRASS = glm::vec3(0.0f, 3.0f, 0.0f),
INIT_POS_BOTTOM_GRASS = glm::vec3(0.0f, -3.0f, 0.0f),
INIT_POS_LEFT_GRASS = glm::vec3(-4.5f, 0.0f, 0.0f),
INIT_POS_RIGHT_GRASS = glm::vec3(4.5f, 0.0f, 0.0f),
INIT_POS_BACKGROUND = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_POS_GAMEOVER = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_POS_WIN = glm::vec3(0.0f, 0.0f, 0.0f);


//position and movement vectors
glm::vec3 g_corn_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_corn_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_corn2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_corn2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_kernel_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_kernel_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_top_grass_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_bottom_grass_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_left_grass_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_right_grass_position = glm::vec3(0.0f, 0.0f, 0.0f);

void initialise();
void process_input();
void update();
void render();
void shutdown();

bool SINGLE_PLAYER = false;

//wall collisions
bool LEFT_COLLIDE = false;
bool RIGHT_COLLIDE = false;
bool p1_TOP_COLLIDE = false;
bool p1_BOTTOM_COLLIDE = false;
bool p2_TOP_COLLIDE = false;
bool p2_BOTTOM_COLLIDE = false;

bool GAME_START = false;
bool GAME_OVER = false;


GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Pong Clone: A Corny Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);


    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);



    //set the initial positions of objects
    g_corn_matrix = glm::mat4(1.0f);
    g_corn2_matrix = glm::mat4(1.0f);

    //move corn to left and right of background
    g_corn_position = INIT_POS_CORN;
    g_corn2_position = INIT_POS_CORN2;

    g_kernel_matrix = glm::mat4(1.0f);

    //set kernel to middle of screen
    g_kernel_position = INIT_POS_KERNEL;

    g_background_matrix = glm::mat4(1.0f);

    g_gameover_matrix = glm::mat4(1.0f);

    g_top_grass_matrix = glm::mat4(1.0f);
    g_bottom_grass_matrix = glm::mat4(1.0f);
    g_left_grass_matrix = glm::mat4(1.0f);
    g_right_grass_matrix = glm::mat4(1.0f);

    g_win_matrix = glm::mat4(1.0f);

    g_start_matrix = glm::mat4(1.0f);

    //set grass positions
    g_top_grass_position = INIT_POS_TOP_GRASS;
    g_bottom_grass_position = INIT_POS_BOTTOM_GRASS;
    g_left_grass_position = INIT_POS_LEFT_GRASS;
    g_right_grass_position = INIT_POS_RIGHT_GRASS;

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);


    //load textures
    g_corn_texture_id = load_texture(CORN_FILEPATH);
    g_kernel_texture_id = load_texture(KERNEL_FILEPATH);
    g_grass_texture_id = load_texture(GRASS_FILEPATH);
    g_vertical_grass_texture_id = load_texture(VERTICAL_GRASS_FILEPATH);
    g_background_texture_id = load_texture(BACKGROUND_FILEPATH);
    g_gameover_texture_id = load_texture(GAMEOVER_FILEPATH);
    g_win_texture_id = load_texture(WIN_FILEPATH);
    g_start_texture_id = load_texture(START_FILEPATH);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q: g_app_status = TERMINATED; break;
            default:
                break;
            }

        default:
            break;
        }

        //reset movement
        g_corn_movement.y = 0.0f;
        g_corn2_movement.y = 0.0f;

        //detect if a key is being pressed
        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        //If the game is not over and started, we can move the corns
        if (!GAME_OVER && GAME_START)
        {
            //if single player mode is off, we can move corn 2
            if (!SINGLE_PLAYER)
            {
                //detect if a key is being pressed
                if (key_state[SDL_SCANCODE_UP])
                {
                    if (p2_TOP_COLLIDE == false)
                    {
                        g_corn2_movement.y = CORN_SPEED;
                    }
                    else if (p2_TOP_COLLIDE)
                    {
                        g_corn2_movement.y = 0.0f;
                    }
                }

                else if (key_state[SDL_SCANCODE_DOWN])
                {
                    if (p2_BOTTOM_COLLIDE == false)
                    {
                        g_corn2_movement.y = -1.0f * CORN_SPEED;
                    }
                    else if (p2_BOTTOM_COLLIDE)
                    {
                        g_corn2_movement.y = 0.0f;
                    }
                }
            }

            //corn 1 controls
            if (key_state[SDL_SCANCODE_W])
            {
                if (p1_TOP_COLLIDE == false)
                {
                    g_corn_movement.y = CORN_SPEED;
                }
                else if (p1_TOP_COLLIDE)
                {
                    g_corn_movement.y = 0.0f;
                }
            }

            else if (key_state[SDL_SCANCODE_S])
            {
                if (p1_BOTTOM_COLLIDE == false)
                {
                    g_corn_movement.y = -CORN_SPEED;
                }
                else if (p1_BOTTOM_COLLIDE)
                {
                    g_corn_movement.y = 0.0f;
                }
            }

            //enable single player mode against corn2
            else if (key_state[SDL_SCANCODE_T])
            {
                SINGLE_PLAYER = true;
            }

        }

        //resets the game so I don't have to rerun it every time I test something
        if (key_state[SDL_SCANCODE_R])
        {
            g_corn_position = INIT_POS_CORN;
            g_corn2_position = INIT_POS_CORN2;
            g_kernel_position = INIT_POS_KERNEL;
            g_kernel_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            GAME_START = false;
            GAME_OVER = false;
            RIGHT_COLLIDE = false;
            LEFT_COLLIDE = false;
            SINGLE_PLAYER = false;
        }

        //start the game
        if (key_state[SDL_SCANCODE_SPACE])
        {
            GAME_START = true;
            //Kinda of randomly decide how the kernel will move when the game starts
            //I guess if you are really, really good at counting you can cheat the system      
            //check milliseconds since game start
            float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
            //if milliseconds are even, kernel moves left and up
            if ((int)ticks % 2 == 0)
            {
                g_kernel_movement.x = -KERNEL_SPEED;
                g_kernel_movement.y = KERNEL_SPEED;
            }
            //if milliseconds are odd, kernel moves right and down
            else
            {
                g_kernel_movement.x = KERNEL_SPEED;
                g_kernel_movement.y = -KERNEL_SPEED;
            }
        }
    }
    //normalize kernel diagonal movement
    if (glm::length(g_kernel_movement) > KERNEL_SPEED)
    {
        g_kernel_movement = KERNEL_SPEED * glm::normalize(g_kernel_movement);
    }
}

void update()
{
    // --- DELTA TIME CALCULATIONS --- //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    //updates corn 2 continuously in single player mode
    if (SINGLE_PLAYER)
    {
        //during single player mode, corn 2 will move up and down based on the kernel's position, basically cannot lose
        g_corn2_movement.y = 0.0f;
        g_corn2_position.y += (g_kernel_position.y - g_corn2_position.y) * 1.50f * CORN_SPEED * delta_time;
    }

    // --- COLLISION LOGIC --- //
   //Do for top, bottom(everything), left, right(only ball)
   //if the ball hits the top or bottom, reverse the x,y directions
   //W-S CORN
   //____________________________________________________________________________________________________
    float y_distance_top_corn = fabs(g_corn_position.y - g_top_grass_position.y) -
        ((INIT_SCALE_CORN.y + INIT_SCALE_GRASS.y) / 2.0f);

    //collision zone is slightly adjusted so that it looks like we are actually touching the grass
    //and not the invisible box
    //if there is a collision, we set collide to true to stop the player from moving it up/down more
    //then we move the corn to right before the collision zone so it doesn't clip through the grass
    if (y_distance_top_corn < -.50f)
    {
        p1_TOP_COLLIDE = true;
        g_corn_position.y = g_top_grass_position.y + .50f - (INIT_SCALE_GRASS.y + INIT_SCALE_CORN.y) / 2.0f;
    }

    else
    {
        p1_TOP_COLLIDE = false;
    }

    float y_distance_bottom_corn = fabs(g_corn_position.y - g_bottom_grass_position.y) -
        ((INIT_SCALE_CORN.y + INIT_SCALE_GRASS.y) / 2.0f);

    if (y_distance_bottom_corn < -.50f)
    {
        p1_BOTTOM_COLLIDE = true;
        g_corn_position.y = g_bottom_grass_position.y - .50f + (INIT_SCALE_GRASS.y + INIT_SCALE_CORN.y) / 2.0f;
    }
    else
    {
        p1_BOTTOM_COLLIDE = false;
    }
    //____________________________________________________________________________________________________

    //ARROW KEY CORN
    //____________________________________________________________________________________________________
    float y_distance_top_corn2 = fabs(g_corn2_position.y - g_top_grass_position.y) -
        ((INIT_SCALE_CORN.y + INIT_SCALE_GRASS.y) / 2.0f);

    if (y_distance_top_corn2 < -.50f)
    {
        p2_TOP_COLLIDE = true;
        g_corn2_position.y = g_top_grass_position.y + .50f - (INIT_SCALE_GRASS.y + INIT_SCALE_CORN.y) / 2.0f;
    }
    else
    {
        p2_TOP_COLLIDE = false;
    }

    float y_distance_bottom_corn2 = fabs(g_corn2_position.y - g_bottom_grass_position.y) -
        ((INIT_SCALE_CORN.y + INIT_SCALE_GRASS.y) / 2.0f);

    if (y_distance_bottom_corn2 < -.50f)
    {
        p2_BOTTOM_COLLIDE = true;
        g_corn2_position.y = g_bottom_grass_position.y - .50f + (INIT_SCALE_GRASS.y + INIT_SCALE_CORN.y) / 2.0f;
    }
    else
    {
        p2_BOTTOM_COLLIDE = false;
    }
    //____________________________________________________________________________________________________

    //KERNEL TOP WALL BOTTOM WALL
    //____________________________________________________________________________________________________
    float y_distance_top_kernel = fabs(g_kernel_position.y - g_top_grass_position.y) -
        ((INIT_SCALE_KERNEL.y + INIT_SCALE_GRASS.y) / 2.0f);
    float y_distance_bottom_kernel = fabs(g_kernel_position.y - g_bottom_grass_position.y) -
        ((INIT_SCALE_KERNEL.y + INIT_SCALE_GRASS.y) / 2.0f);
    //essentially if the kernel hits the top or bottom, we reverse the y direction
    if (y_distance_top_kernel < -.50f || g_kernel_position.y >= g_top_grass_position.y)
    {
        g_kernel_movement.y *= -1.0f;
        g_kernel_position.y = g_top_grass_position.y + .50f - (INIT_SCALE_KERNEL.y + INIT_SCALE_GRASS.y) / 2.0f;
    }
    else if (y_distance_bottom_kernel < -.50f || g_kernel_position.y <= g_bottom_grass_position.y)
    {
        g_kernel_movement.y *= -1.0f;
        g_kernel_position.y = g_bottom_grass_position.y - .50f + (INIT_SCALE_KERNEL.y + INIT_SCALE_GRASS.y) / 2.0f;
    }

    //____________________________________________________________________________________________________

    //KERNEL CORN1 CORN2 STUFF
    //____________________________________________________________________________________________________

    float x_distance_corn1_kernel = fabs(g_kernel_position.x - g_corn_position.x) -
        ((INIT_SCALE_KERNEL.x + INIT_SCALE_CORN.x) / 2.0f);
    float x_distance_corn2_kernel = fabs(g_kernel_position.x - g_corn2_position.x) -
        ((INIT_SCALE_KERNEL.x + INIT_SCALE_CORN.x) / 2.0f);

    float y_distance_corn1_kernel = fabs(g_kernel_position.y - g_corn_position.y) -
        ((INIT_SCALE_KERNEL.y + INIT_SCALE_CORN.y) / 2.0f);
    float y_distance_corn2_kernel = fabs(g_kernel_position.y - g_corn2_position.y) -
        ((INIT_SCALE_KERNEL.y + INIT_SCALE_CORN.y) / 2.0f);

    //if the kernel hits the corn, we reverse the x direction
    if (x_distance_corn1_kernel < 0.0f && y_distance_corn1_kernel < 0.0f)
    {
        g_kernel_movement.x *= -1.0f;
        //if the kernel somehow ends up left of corn1, we move it further towards the left
        if (g_kernel_position.x < g_corn_position.x)
        {
            g_kernel_position.x = g_corn_position.x - 0.1f - (INIT_SCALE_KERNEL.x + INIT_SCALE_CORN.x) / 2.0f;
        }

        //when hit from right side, move kernel to the right
        else if (g_kernel_position.x > g_corn_position.x)
        {

            g_kernel_position.x = g_corn_position.x + 0.1f + (INIT_SCALE_KERNEL.x + INIT_SCALE_CORN.x) / 2.0f;
        }
    }

    if (x_distance_corn2_kernel < 0.0f && y_distance_corn2_kernel < 0.0f)
    {

        g_kernel_movement.x *= -1.0f;
        //left side move left
        if (g_kernel_position.x < g_corn2_position.x)
        {
            g_kernel_position.x = g_corn2_position.x - 0.1f - (INIT_SCALE_KERNEL.x + INIT_SCALE_CORN.x) / 2.0f;
        }

        //right side move right
        else if (g_kernel_position.x > g_corn2_position.x)
        {
            g_kernel_position.x = g_corn2_position.x + 0.1f + (INIT_SCALE_KERNEL.x + INIT_SCALE_CORN.x) / 2.0f;
        }
    }
    //____________________________________________________________________________________________________


    // --- STOP GAME WHEN KERNEL ESCAPES --- //
    float x_distance_left_kernel = fabs(g_kernel_position.x + INIT_POS_KERNEL.x - INIT_POS_LEFT_GRASS.x) -
        ((INIT_SCALE_KERNEL.x + INIT_SCALE_VERTICAL_GRASS.x) / 2.0f);
    float x_distance_right_kernel = fabs(g_kernel_position.x + INIT_POS_KERNEL.x - INIT_POS_RIGHT_GRASS.x) -
        ((INIT_SCALE_KERNEL.x + INIT_SCALE_VERTICAL_GRASS.x) / 2.0f);
    if (x_distance_left_kernel < -.50f)
    {
        g_kernel_movement.x = 0.0f;
        g_kernel_movement.y = 0.0f;
        GAME_OVER = true;
        LEFT_COLLIDE = true;
    }
    else if (x_distance_right_kernel < -.50f)
    {
        g_kernel_movement.x = 0.0f;
        g_kernel_movement.y = 0.0f;
        GAME_OVER = true;
        RIGHT_COLLIDE = true;
    }
    else
    {
        GAME_OVER = false;
    }


    // --- ACCUMULATOR LOGIC --- //
    g_corn_position += g_corn_movement * delta_time;
    g_corn2_position += g_corn2_movement * delta_time;
    g_kernel_position += g_kernel_movement * delta_time;


    //---INITIALIZE MATRICES---//
    g_corn_matrix = glm::mat4(1.0f);
    g_corn2_matrix = glm::mat4(1.0f);
    g_kernel_matrix = glm::mat4(1.0f);
    g_top_grass_matrix = glm::mat4(1.0f);
    g_bottom_grass_matrix = glm::mat4(1.0f);
    g_left_grass_matrix = glm::mat4(1.0f);
    g_right_grass_matrix = glm::mat4(1.0f);
    g_background_matrix = glm::mat4(1.0f);

    //if the game hasn't started we load the start screen
    if (!GAME_START)
    {
        g_start_matrix = glm::mat4(1.0f);
        g_start_matrix = glm::scale(g_start_matrix, INIT_SCALE_START);
    }

    //Summons game over objects when gameover
    if (GAME_OVER)
    {
        g_gameover_matrix = glm::mat4(1.0f);
        g_win_matrix = glm::mat4(1.0f);
    }

    //Figures out which side won and moves the win png to the winning side
    if (GAME_OVER)
    {
        if (LEFT_COLLIDE)
        {
            g_win_matrix = glm::translate(g_win_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
        }
        else if (RIGHT_COLLIDE)
        {
            g_win_matrix = glm::translate(g_win_matrix, glm::vec3(-2.0f, 0.0f, 0.0f));
        }
        g_gameover_matrix = glm::scale(g_gameover_matrix, INIT_SCALE_GAMEOVER);
        g_win_matrix = glm::scale(g_win_matrix, INIT_SCALE_WIN);
    }

    // --- TRANSLATION --- //
    g_corn_matrix = glm::translate(g_corn_matrix, g_corn_position);
    g_corn2_matrix = glm::translate(g_corn2_matrix, g_corn2_position);
    g_kernel_matrix = glm::translate(g_kernel_matrix, g_kernel_position);
    g_top_grass_matrix = glm::translate(g_top_grass_matrix, g_top_grass_position);
    g_bottom_grass_matrix = glm::translate(g_bottom_grass_matrix, g_bottom_grass_position);
    g_left_grass_matrix = glm::translate(g_left_grass_matrix, g_left_grass_position);
    g_right_grass_matrix = glm::translate(g_right_grass_matrix, g_right_grass_position);

    // --- ROTATION --- //
    g_top_grass_matrix = glm::rotate(g_top_grass_matrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    g_right_grass_matrix = glm::rotate(g_right_grass_matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // --- SCALING --- //
    g_corn_matrix = glm::scale(g_corn_matrix, INIT_SCALE_CORN);
    g_corn2_matrix = glm::scale(g_corn2_matrix, INIT_SCALE_CORN);
    g_kernel_matrix = glm::scale(g_kernel_matrix, INIT_SCALE_KERNEL);
    g_top_grass_matrix = glm::scale(g_top_grass_matrix, INIT_SCALE_GRASS);
    g_bottom_grass_matrix = glm::scale(g_bottom_grass_matrix, INIT_SCALE_GRASS);
    g_left_grass_matrix = glm::scale(g_left_grass_matrix, INIT_SCALE_VERTICAL_GRASS);
    g_right_grass_matrix = glm::scale(g_right_grass_matrix, INIT_SCALE_VERTICAL_GRASS);
    g_background_matrix = glm::scale(g_background_matrix, INIT_SCALE_BACKGROUND);
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture

    draw_object(g_background_matrix, g_background_texture_id);

    draw_object(g_corn_matrix, g_corn_texture_id);

    draw_object(g_corn2_matrix, g_corn_texture_id);

    draw_object(g_kernel_matrix, g_kernel_texture_id);

    draw_object(g_top_grass_matrix, g_grass_texture_id);
    draw_object(g_bottom_grass_matrix, g_grass_texture_id);
    draw_object(g_left_grass_matrix, g_vertical_grass_texture_id);
    draw_object(g_right_grass_matrix, g_vertical_grass_texture_id);


    //draws game over objects
    if (GAME_OVER)
    {
        draw_object(g_gameover_matrix, g_gameover_texture_id);
        draw_object(g_win_matrix, g_win_texture_id);
    }

    //draws start screen
    if (!GAME_START)
    {
        draw_object(g_start_matrix, g_start_texture_id);
    }

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}

