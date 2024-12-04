#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <random>
#include <math.h>

#define SCREEN_WIDTH 380
#define SCREEN_HEIGHT 650
#define PIPE_GAP 145

float speed = 0;
float bird_angle = 0.0;
int bird_height_diff = 0;
float hover_direction = 0;
float hover_mag = 0;
float hover_time = 0.0f;
float hover_speed = 2.0f;

struct score_struct
{
    SDL_Texture *score_n;
    SDL_Rect score_n_rect_1;
    SDL_Rect score_n_rect_2;
};

inline void hover_bird(SDL_Rect *bird)
{
    bird->y = (int)(SCREEN_HEIGHT / 2 + sin(hover_time) * 8);
    hover_time += hover_speed * 0.06f;
}

inline void flap_bird(SDL_Rect *bird)
{
    bird->x == 0 ? bird->x = 56 : bird->x = 0;
}

void move_platform(SDL_Rect *platform_1_rect_2, SDL_Rect *platform_2_rect_2)
{
    if (platform_1_rect_2->x < -SCREEN_WIDTH)
        platform_1_rect_2->x = SCREEN_WIDTH - 20;
    if (platform_2_rect_2->x < -SCREEN_WIDTH)
        platform_2_rect_2->x = SCREEN_WIDTH - 20;
    platform_2_rect_2->x = platform_2_rect_2->x - 3;
    platform_1_rect_2->x = platform_1_rect_2->x - 3;
}

void move_pipes(SDL_Rect *pipe_1, SDL_Rect *pipe_2, SDL_Rect *pipe_3, SDL_Rect *pipe_4, SDL_Rect *pipe_5, SDL_Rect *pipe_6, int *pipe_1_y, int *pipe_3_y, int *pipe_5_y, std::uniform_int_distribution<> distr, std::mt19937 *gen, bool &pipe_1_crossed, bool &pipe_2_crossed, bool &pipe_3_crossed)
{
    if (pipe_1->x < -100)
    {
        pipe_1->x = pipe_5->x + 300;
        *pipe_1_y = distr(*gen);
        pipe_1->y = *pipe_1_y;
        pipe_1_crossed = false;
    }
    if (pipe_2->x < -100)
    {
        pipe_2->x = pipe_6->x + 300;
        pipe_2->y = 400 + *pipe_1_y + PIPE_GAP;
    }
    if (pipe_3->x < -100)
    {
        pipe_3->x = pipe_1->x + 300;
        *pipe_3_y = distr(*gen);
        pipe_3->y = *pipe_3_y;
        pipe_2_crossed = false;
    }
    if (pipe_4->x < -100)
    {
        pipe_4->x = pipe_2->x + 300;
        pipe_4->y = 400 + *pipe_3_y + PIPE_GAP;
    }
    if (pipe_5->x < -100)
    {
        pipe_5->x = pipe_3->x + 300;
        *pipe_5_y = distr(*gen);
        pipe_5->y = *pipe_5_y;
        pipe_3_crossed = false;
    }
    if (pipe_6->x < -100)
    {
        pipe_6->x = pipe_4->x + 300;
        pipe_6->y = 400 + *pipe_5_y + PIPE_GAP;
    }
    pipe_1->x = pipe_1->x - 3;
    pipe_2->x = pipe_2->x - 3;
    pipe_3->x = pipe_3->x - 3;
    pipe_4->x = pipe_4->x - 3;
    pipe_5->x = pipe_5->x - 3;
    pipe_6->x = pipe_6->x - 3;
}

void free_fall(SDL_Rect *bird)
{
    if (bird->y < SCREEN_HEIGHT - 130 - 38)
    {
        speed = speed + 0.5;
        bird->y = bird->y + speed;
        if (bird->y >= SCREEN_HEIGHT - 130 - 38)
        {
            bird->y = SCREEN_HEIGHT - 130 - 38;
            speed = 0;
        }
        if (bird->y - bird_height_diff >= 7)
            bird_angle = bird_angle + 10;
        if (bird_angle > 90)
            bird_angle = 90;
    }
}

void show_score_board(SDL_Renderer *renderer, SDL_Rect *score_board, Mix_Chunk *die_sound, std::vector<score_struct> score_array, int score_pointer_1, int score_pointer_2, SDL_Rect *score_board_score_rect_1, SDL_Rect *score_board_score_rect_2)
{
    score_board->y = score_board->y - 8;
    if (score_board->y < SCREEN_HEIGHT / 2 - 75)
    {
        score_board->y = SCREEN_HEIGHT / 2 - 75;
        if (score_pointer_1 == 0)
            SDL_RenderCopy(renderer, score_array[score_pointer_2].score_n, &score_array[score_pointer_2].score_n_rect_1, score_board_score_rect_1);
        else
        {
            score_board_score_rect_1->x = score_board->x + score_board->w - 65 - 10;
            SDL_RenderCopy(renderer, score_array[score_pointer_1].score_n, &score_array[score_pointer_1].score_n_rect_1, score_board_score_rect_1);
            SDL_RenderCopy(renderer, score_array[score_pointer_2].score_n, &score_array[score_pointer_2].score_n_rect_1, score_board_score_rect_2);
        }
    }
    if (score_board->y == SCREEN_HEIGHT - 160)
        Mix_PlayChannel(-1, die_sound, 0);
}

int main(int argc, char *argv[])
{

    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;

    int FPS = 60;
    Uint32 elapsed = 0;
    int frametime = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Error while initializing SDL due to : " << SDL_GetError() << std::endl;
        return 1;
    }

    win = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!win)
    {
        std::cout << "Failed to create window due to : " << SDL_GetError() << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        std::cout << "Failed to create the renderer due to : " << SDL_GetError() << std::endl;
        return 1;
    }

    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0)
    {
        std::cerr << "Mix_Init failed: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
        Mix_Quit();
        SDL_Quit();
        return -1;
    }

    Mix_Chunk *wing_sound = Mix_LoadWAV("./sounds/sfx_wing.wav");
    Mix_Chunk *hit_sound = Mix_LoadWAV("./sounds/sfx_hit.wav");
    Mix_Chunk *die_sound = Mix_LoadWAV("./sounds/sfx_die.wav");

    if (wing_sound == nullptr)
    {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
        Mix_Quit();
        SDL_Quit();
        return -1;
    }

    if (hit_sound == nullptr)
    {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
        Mix_Quit();
        SDL_Quit();
        return -1;
    }

    if (die_sound == nullptr)
    {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
        Mix_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-250, -70);

    int pipe_1_y = distr(gen);
    int pipe_3_y = distr(gen);
    int pipe_5_y = distr(gen);

    SDL_Surface *sprite_sheet_image = IMG_Load("spritesheet.png");
    SDL_Texture *background_texture = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect bt_src_rect = {0, 0, 144, 250};

    SDL_Texture *pipe_texture_1 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect pipe_1_rect_1 = {55, 373, 28, 110};
    SDL_Rect pipe_1_rect_2 = {SCREEN_WIDTH + 200, pipe_1_y, 100, 400};

    SDL_Texture *pipe_texture_2 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect pipe_2_rect_1 = {83, 323, 28, 110};
    SDL_Rect pipe_2_rect_2 = {SCREEN_WIDTH + 200, 400 + pipe_1_y + PIPE_GAP, 100, 400};

    SDL_Texture *pipe_texture_3 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect pipe_3_rect_1 = {55, 373, 28, 110};
    SDL_Rect pipe_3_rect_2 = {pipe_1_rect_2.x + 300, pipe_3_y, 100, 400};

    SDL_Texture *pipe_texture_4 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect pipe_4_rect_1 = {83, 323, 28, 110};
    SDL_Rect pipe_4_rect_2 = {pipe_2_rect_2.x + 300, 400 + pipe_3_y + PIPE_GAP, 100, 400};

    SDL_Texture *pipe_texture_5 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect pipe_5_rect_1 = {55, 373, 28, 110};
    SDL_Rect pipe_5_rect_2 = {pipe_3_rect_2.x + 300, pipe_5_y, 100, 400};

    SDL_Texture *pipe_texture_6 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect pipe_6_rect_1 = {83, 323, 28, 110};
    SDL_Rect pipe_6_rect_2 = {pipe_4_rect_2.x + 300, 400 + pipe_5_y + PIPE_GAP, 100, 400};

    SDL_Texture *platform_texture_1 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect platform_1_rect_1 = {300, 0, 160, 50};
    SDL_Rect platform_1_rect_2 = {0, SCREEN_HEIGHT - 130, SCREEN_WIDTH, 130};

    SDL_Texture *platform_texture_2 = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect platform_2_rect_1 = {300, 0, 160, 50};
    SDL_Rect platform_2_rect_2 = {SCREEN_WIDTH - 10, SCREEN_HEIGHT - 130, SCREEN_WIDTH, 130};

    SDL_Texture *initial_bird = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect initial_bird_rect_1 = {28, 490, 20, 15};
    SDL_Rect initial_bird_rect_2 = {SCREEN_WIDTH / 2 - 45, SCREEN_HEIGHT / 2, 55, 45};

    SDL_Texture *bird = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect bird_rect_1 = {0, 490, 20, 15};
    SDL_Rect bird_rect_2 = {SCREEN_WIDTH / 2 - 45, SCREEN_HEIGHT / 2, 55, 45};

    SDL_Texture *score_board = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    SDL_Rect score_board_rect_1 = {0, 258, 120, 60};
    SDL_Rect score_board_rect_2 = {SCREEN_WIDTH / 2 - (SCREEN_WIDTH - 100) / 2, SCREEN_HEIGHT, SCREEN_WIDTH - 100, 150};

    score_struct score_n_0;
    score_n_0.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_0.score_n_rect_1 = {495, 60, 15, 20};
    score_n_0.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_1;
    score_n_1.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_1.score_n_rect_1 = {136, 455, 15, 20};
    score_n_1.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_2;
    score_n_2.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_2.score_n_rect_1 = {290, 160, 15, 20};
    score_n_2.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_3;
    score_n_3.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_3.score_n_rect_1 = {305, 160, 15, 20};
    score_n_3.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_4;
    score_n_4.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_4.score_n_rect_1 = {320, 160, 12, 20};
    score_n_4.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_5;
    score_n_5.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_5.score_n_rect_1 = {333, 160, 15, 20};
    score_n_5.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_6;
    score_n_6.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_6.score_n_rect_1 = {290, 184, 15, 20};
    score_n_6.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_7;
    score_n_7.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_7.score_n_rect_1 = {305, 184, 15, 20};
    score_n_7.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_8;
    score_n_8.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_8.score_n_rect_1 = {318, 184, 15, 20};
    score_n_8.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    score_struct score_n_9;
    score_n_9.score_n = SDL_CreateTextureFromSurface(renderer, sprite_sheet_image);
    score_n_9.score_n_rect_1 = {334, 184, 15, 20};
    score_n_9.score_n_rect_2 = {SCREEN_WIDTH / 2, 100, 40, 40};

    std::vector<score_struct> score_array;
    score_array.push_back(score_n_0);
    score_array.push_back(score_n_1);
    score_array.push_back(score_n_2);
    score_array.push_back(score_n_3);
    score_array.push_back(score_n_4);
    score_array.push_back(score_n_5);
    score_array.push_back(score_n_6);
    score_array.push_back(score_n_7);
    score_array.push_back(score_n_8);
    score_array.push_back(score_n_9);

    SDL_Rect score_board_score_rect_1 = {score_board_rect_2.x + score_board_rect_2.w - 65, SCREEN_HEIGHT / 2 - 75 + 45, 20, 20};
    SDL_Rect score_board_score_rect_2 = {score_board_score_rect_1.x + score_board_score_rect_1.w - 10, SCREEN_HEIGHT / 2 - 75 + 45, 20, 20};

    int score_pointer_1 = 0;
    int score_pointer_2 = 0;

    bool pipe_1_crossed = false;
    bool pipe_2_crossed = false;
    bool pipe_3_crossed = false;

    bool running = true;
    int x1 = 0, y1 = 529;
    int x2 = SCREEN_WIDTH, y2 = 529;
    bool isDrawing = false;

    SDL_Event event;
    SDL_MouseMotionEvent mme;

    float time = SDL_GetTicks();
    int last_time = SDL_GetTicks();

    bool start = false;
    bool player_died = false;
    int player_died_for_blink = 0;
    int last_time_blink = 0;

    while (running)
    {
        elapsed = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                SDL_Quit();
                running = false;
            }
            if (!player_died && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
            {
                speed = 6.5;
                speed = -(speed + 2);
                bird_angle = -20;
                bird_height_diff = bird_rect_2.y;
                start = true;
                Mix_PlayChannel(-1, wing_sound, 0);
            }
            if (player_died && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
            {
                pipe_1_rect_2.x = SCREEN_WIDTH + 200;
                pipe_2_rect_2.x = SCREEN_WIDTH + 200;
                pipe_3_rect_2.x = pipe_1_rect_2.x + 300;
                pipe_4_rect_2.x = pipe_2_rect_2.x + 300;
                pipe_5_rect_2.x = pipe_3_rect_2.x + 300;
                pipe_6_rect_2.x = pipe_4_rect_2.x + 300;
                score_pointer_1 = 0;
                score_pointer_2 = 0;
                bird_rect_2.y = SCREEN_HEIGHT / 2;
                score_board_rect_2.y = SCREEN_HEIGHT;
                start = false;
                player_died = false;
                pipe_1_crossed = false;
                pipe_2_crossed = false;
                pipe_3_crossed = false;
            }
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m)
                player_died = true;
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);

        if (!start)
        {
            hover_bird(&initial_bird_rect_2);
        }

        if (start && !player_died)
        {
            free_fall(&bird_rect_2);
            move_pipes(&pipe_1_rect_2, &pipe_2_rect_2, &pipe_3_rect_2, &pipe_4_rect_2, &pipe_5_rect_2, &pipe_6_rect_2, &pipe_1_y, &pipe_3_y, &pipe_5_y, distr, &gen, pipe_1_crossed, pipe_2_crossed, pipe_3_crossed);
        }
        if (!player_died)
            move_platform(&platform_1_rect_2, &platform_2_rect_2);

        if (!player_died_for_blink)
        {
            SDL_RenderCopy(renderer, background_texture, &bt_src_rect, NULL);
            SDL_RenderCopy(renderer, pipe_texture_1, &pipe_1_rect_1, &pipe_1_rect_2);
            SDL_RenderCopy(renderer, pipe_texture_2, &pipe_2_rect_1, &pipe_2_rect_2);
            SDL_RenderCopy(renderer, pipe_texture_3, &pipe_3_rect_1, &pipe_3_rect_2);
            SDL_RenderCopy(renderer, pipe_texture_4, &pipe_4_rect_1, &pipe_4_rect_2);
            SDL_RenderCopy(renderer, pipe_texture_5, &pipe_5_rect_1, &pipe_5_rect_2);
            SDL_RenderCopy(renderer, pipe_texture_6, &pipe_6_rect_1, &pipe_6_rect_2);
            if (score_pointer_1 == 0)
                SDL_RenderCopy(renderer, score_array[score_pointer_2].score_n, &score_array[score_pointer_2].score_n_rect_1, &score_array[score_pointer_2].score_n_rect_2);
            else
            {
                score_array[score_pointer_1].score_n_rect_2.x = SCREEN_WIDTH / 2 - 15;
                SDL_RenderCopy(renderer, score_array[score_pointer_1].score_n, &score_array[score_pointer_1].score_n_rect_1, &score_array[score_pointer_1].score_n_rect_2);
                score_array[score_pointer_2].score_n_rect_2.x = score_array[score_pointer_1].score_n_rect_2.w + score_array[score_pointer_1].score_n_rect_2.x;
                SDL_RenderCopy(renderer, score_array[score_pointer_2].score_n, &score_array[score_pointer_2].score_n_rect_1, &score_array[score_pointer_2].score_n_rect_2);
            }

            if (!start)
                SDL_RenderCopy(renderer, initial_bird, &initial_bird_rect_1, &initial_bird_rect_2);
            else
                SDL_RenderCopyEx(renderer, bird, &bird_rect_1, &bird_rect_2, bird_angle, NULL, SDL_FLIP_NONE);
            SDL_RenderCopy(renderer, platform_texture_1, &platform_1_rect_1, &platform_1_rect_2);
            SDL_RenderCopy(renderer, platform_texture_2, &platform_2_rect_1, &platform_2_rect_2);

            if (player_died)
            {
                SDL_RenderCopy(renderer, score_board, &score_board_rect_1, &score_board_rect_2);
                show_score_board(renderer, &score_board_rect_2, die_sound, score_array, score_pointer_1, score_pointer_2, &score_board_score_rect_1, &score_board_score_rect_2);
            }
        }
        if (SDL_GetTicks() - last_time >= 150 && start)
        {
            last_time = SDL_GetTicks();
            flap_bird(&bird_rect_1);
        }
        if (SDL_GetTicks() - last_time_blink >= 70)
            player_died_for_blink = false;
        // std::cout<<player_died_for_blink<<std::endl;
        // if (!player_died && (SDL_HasIntersection(&bird_rect_2, &pipe_1_rect_2) || SDL_HasIntersection(&bird_rect_2, &pipe_2_rect_2) || SDL_HasIntersection(&bird_rect_2, &pipe_3_rect_2) || SDL_HasIntersection(&bird_rect_2, &pipe_4_rect_2) || SDL_HasIntersection(&bird_rect_2, &pipe_5_rect_2) || SDL_HasIntersection(&bird_rect_2, &pipe_6_rect_2) || SDL_HasIntersection(&bird_rect_2, &platform_1_rect_2) || SDL_HasIntersection(&bird_rect_2, &platform_2_rect_2)))
        // {
        //     if (!player_died)
        //     {
        //         Mix_PlayChannel(-1, hit_sound, 0);
        //         // SDL_Delay(300);
        //         // Mix_PlayChannel(-1, die_sound, 0);
        //     }
        //     player_died = true;
        //     player_died_for_blink = true;
        //     last_time_blink = SDL_GetTicks();
        // }
        if (player_died)
            free_fall(&bird_rect_2);
        if (!pipe_1_crossed && pipe_1_rect_2.x < bird_rect_2.x)
        {
            pipe_1_crossed = true;
            if (score_pointer_2 >= score_array.size() - 1)
            {
                score_pointer_2 = 0;
                score_pointer_1 = score_pointer_1 + 1;
            }
            else
                score_pointer_2 = score_pointer_2 + 1;
            if (score_pointer_1 > score_array.size() - 1)
                score_pointer_1 = 0;
        }
        if (!pipe_2_crossed && pipe_3_rect_2.x < bird_rect_2.x)
        {
            pipe_2_crossed = true;
            if (score_pointer_2 >= score_array.size() - 1)
            {
                score_pointer_2 = 0;
                score_pointer_1 = score_pointer_1 + 1;
            }
            else
                score_pointer_2 = score_pointer_2 + 1;
            if (score_pointer_1 > score_array.size() - 1)
                score_pointer_1 = 0;
        }
        if (!pipe_3_crossed && pipe_5_rect_2.x < bird_rect_2.x)
        {
            pipe_3_crossed = true;
            if (score_pointer_2 >= score_array.size() - 1)
            {
                score_pointer_2 = 0;
                score_pointer_1 = score_pointer_1 + 1;
            }
            else
                score_pointer_2 = score_pointer_2 + 1;
            if (score_pointer_1 > score_array.size() - 1)
                score_pointer_1 = 0;
        }

        SDL_RenderPresent(renderer);

        frametime = SDL_GetTicks() - elapsed;

        if (1000 / FPS > frametime)
        {
            SDL_Delay((1000 / FPS) - frametime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(sprite_sheet_image);
    SDL_DestroyTexture(background_texture);
    SDL_DestroyTexture(pipe_texture_1);
    SDL_DestroyTexture(pipe_texture_2);
    SDL_DestroyTexture(pipe_texture_3);
    SDL_DestroyTexture(pipe_texture_4);
    SDL_DestroyTexture(pipe_texture_5);
    SDL_DestroyTexture(pipe_texture_6);
    SDL_DestroyTexture(bird);
    SDL_DestroyTexture(platform_texture_1);
    SDL_DestroyTexture(platform_texture_2);
    SDL_DestroyWindow(win);
    Mix_FreeChunk(wing_sound);
    Mix_FreeChunk(hit_sound);
    Mix_FreeChunk(die_sound);
    Mix_Quit();

    SDL_Quit();

    return 0;
}