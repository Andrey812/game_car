/* 
DESCRIPTION:
This is simple 2D game source code
You drive the car using keyboard Up, Down, Right and Left arrows (ESC - exit)
collecting astersk which appears randomly at different places of screen
This game uses SDL library and developed under Linux 32bit platform

Developer: Andrey Nikolaev
Url: github.com/Andrey812/game_car

gcc -o car car.c `sdl-config --cflags --libs` -lSDL_ttf && ./car
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define WIDTH 1280
#define HEIGHT 800
#define BPP 4
#define DEPTH 32

// Car data: coordinates, direction, speed
struct Car {
	int direct; 								// Direction,  0 - Up, 1 - Right, 2 - Down, 3 - Left 
	int x;										// X coord the left upper corner of car sprite
	int y;										// Y coord the left upper corner of car sprite
	int accelerate;								// 0 - stop car, 1 - move car
	int speed;									// Coordinate addition when move in pixels
	SDL_Surface *spr_car; 						// Sprite of car
};

// Target "asterisk" data
struct Target {
	int 				x;						// X coord the left upper corner of car sprite
	int 				y;						// Y coord the left upper corner of car sprite
	int 				frame_num; 				// Number of picture from set of pictures
	Uint32 				frame_refresh_time; 	// Time of frame's refresh
	Uint32 				frame_last_refresh; 	// Last refresh time
	int					frames[2];				// Sprite frames
	SDL_Surface *		spr_target; 			// Sprite of target
};

struct Car car;
struct Target target;

// Ground map
int grd[15][18] = {
	{5,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,6},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{8,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7}
};

SDL_Surface *screen, *ground, *dhb;

// Prepare ground pieces picture
void init_ground() {
	
	ground = SDL_LoadBMP( "img/ground.bmp" );
    ground = SDL_DisplayFormat(ground);
}

// Prepare dashboard pieces picture
void init_dashboard() {
	dhb = SDL_LoadBMP( "img/dashboard.bmp" );
    dhb = SDL_DisplayFormat(dhb);
}

// Compile ground from pieces according ground map array
void draw_ground() {
	
	SDL_Rect src;
	SDL_Rect dest;
	
	int grd_col = 0;
	int grd_row = 0;
	
	for (grd_row = 0; grd_row < 15; grd_row++ ) {
		for ( grd_col = 0; grd_col < 18; grd_col++ ) {
			src.x = grd[grd_row][grd_col] * 50;
			src.y = 0;
			src.w = 50;
			src.h = 50;
			
			dest.x = grd_col * 50;
			dest.y = grd_row * 50;
			dest.w = 50;
			dest.h = 50;
			
			SDL_BlitSurface(ground, &src, screen, &dest );
		};
	}
}

void draw_dashboard() {
	SDL_Rect src;
	SDL_Rect dest;
	
	int grd_row = 0;
	int grd_col = 0;
	int type = 4;
	
	for (grd_row = 0; grd_row < 75; grd_row++ ) {
		for ( grd_col = 0; grd_col < 35; grd_col++ ) {
			
			if ( grd_row == 0 || grd_row == 10 ) {
				
				switch(grd_col) {
					case 0:
						type = 0;
						break;
					case 34:
						type = 1;
						break;
					default:
						type = 8;
						break;
				}
			}
			
			if ( grd_row == 9 || grd_row == 74 ) {
				
				switch(grd_col) {
					case 0:
						type = 2;
						break;
					case 34:
						type = 3;
						break;
					default:
						type = 7;
						break;
				}
			}
			
			if ( grd_row != 0 && grd_row != 9 && grd_row != 10 && grd_row != 74 ) {
				switch(grd_col) {
					case 0:
						type = 5;
						break;
					case 34:
						type = 6;
						break;
					default:
						type = 4;
						break;
				}
			}
			
			src.x = type * 10;
			src.y = 0;
			src.w = 10;
			src.h = 10;
			
			dest.x = 904 + ( grd_col * 10 );
			dest.y = grd_row * 10;
			dest.w = 10;
			dest.h = 10;
			
			SDL_BlitSurface(dhb, &src, screen, &dest );
		};
	}
}

// Set default values for car
void init_car() {
	
	int c;
	
	car.direct 	= 0; 		//Direction "Up"
	car.x 	= 450;
	car.y 	= 300;
	car.accelerate = 0; 	//Car stopped
	car.speed = 3;
	
    car.spr_car = SDL_LoadBMP( "img/car.bmp" );
    car.spr_car = SDL_DisplayFormat(car.spr_car);
    SDL_SetColorKey( car.spr_car, SDL_SRCCOLORKEY, 0xFF00FF );
}

// Generate new position for target
void set_target() {
	int x = rand() % 750;
	int y = rand() % 650;
	if ( x < 50 ) {
		x = 50;
	}
	if ( y < 50 ) {
		y = 50;
	}
	target.x = x;
	target.y = y;
}

// Load target pictures and set animation params
void init_target() {
	
	target.spr_target = SDL_LoadBMP( "img/target.bmp" );
    target.spr_target = SDL_DisplayFormat(target.spr_target);
	SDL_SetColorKey( target.spr_target, SDL_SRCCOLORKEY, 0xFF00FF );
	
	target.frame_num = 0;
	target.frame_refresh_time = 600;
	target.frame_last_refresh = 0;
	
	target.frames[0] = 0;
	target.frames[1] = 1;
	
	set_target();
}

// Display screen
void DrawScreen()
{ 
  
    if( SDL_MUSTLOCK(screen) ) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }
    
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  
    SDL_Flip(screen); 
}

// Put all objects to screen
// Generate common picture
void ComposeScreen() {
	SDL_Rect src;
	SDL_Rect dest;
	
	src.x = car.direct * 50;
	src.y = 0;
	src.w = 50;
	src.h = 50;
	
	dest.x = car.x;
	dest.y = car.y;
	dest.w = 50;
	dest.h = 50;
	
	/* Erase Screen */
	SDL_FillRect(screen, NULL, 0x000000);
	
	
	
	/* Add ground and dashboard background */
	draw_ground();
	draw_dashboard();
	
	/* Add target */
	SDL_Rect trg_src;
	SDL_Rect trg_dest;
	
	trg_src.x = target.frames[target.frame_num] * 40;
	trg_src.y = 0;
	trg_src.w = 40;
	trg_src.h = 40;
	trg_dest.x = target.x;
	trg_dest.y = target.y;
	
	SDL_BlitSurface( target.spr_target, &trg_src, screen, &trg_dest );
	
	/* Add car */
    SDL_BlitSurface( car.spr_car, &src, screen, &dest );
    
    //char string[64];
    //snprintf(string, sizeof string, "x: %d y: %d coll: %d", car.x, car.y, col);
    //snprintf(string, sizeof string, "x: %d y: %d", car.x, car.y);
    
    /* Text */
    //TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 20);
	//SDL_Color foregroundColor = { 255, 255, 255 };
    //SDL_Color backgroundColor = { 0, 0, 0 };
	//SDL_Surface* text = TTF_RenderText_Shaded(font, string, foregroundColor, backgroundColor);
	//SDL_Rect textLocation = { 960, 10, 0, 0 };
	//SDL_BlitSurface(text, NULL, screen, &textLocation);
    
    DrawScreen(screen);
}

//Change car position
void move_car() {
	
	switch(car.direct) {
		case 0:
			car.y = car.y - car.speed;
		break;
			case 1:
			car.x = car.x + car.speed;
		break;
			case 2:
			car.y = car.y + car.speed;
		break;
			case 3:
			car.x = car.x - car.speed;
			break;
		default:
			break;
	}
	
	/* simple temporary border detection */
	if ( car.x < 10 ) {
		car.x = 10;
	}
	if ( car.x > 840 ) {
		car.x = 840;
	}
	if ( car.y < 10 ) {
		car.y = 10;
	}
	if ( car.y > 690 ) {
		car.y = 690;
	}
	
	/* simple target capture detection */
	if ( target.x + 20 >= car.x && target.x + 20 <= car.x + 50 
		&& target.y + 20 >= car.y && target.y + 20 <= car.y + 50 ) 
	{
		set_target();
	}
}

int main(void)
{
    SDL_Event event;    
    
    TTF_Init();

    srand(time(NULL));
    
    //Check video initialisation
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    //Check screen initialisation
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE)))
    {
        SDL_Quit();
        return 1;
    }
    
    init_ground();
    init_dashboard();
    init_car();
    init_target();
    	
	//move_car();
	
	int exit_key = 0;
	int update_screen = 1;
	
    while(!exit_key) 
    {
		// Detect timer's events
		Uint32 now;
		now = SDL_GetTicks();
		
		// Target animation
		if ( now - target.frame_last_refresh > target.frame_refresh_time ) {
				target.frame_num++;
				
				if ( target.frame_num >= sizeof(target.frames) / sizeof(int) ) {
					target.frame_num = 0;
				}
				
				target.frame_last_refresh = now;
				
				update_screen = 1;
		}
		
		// Move car
		if ( car.accelerate ) {
			move_car();
			update_screen = 1;
		}
		
		// Redraw screen for show changes
		if ( update_screen ) {
			ComposeScreen();
			update_screen = 0;
		}
		
         while(SDL_PollEvent(&event)) 
         {      
              switch (event.type) 
              {
				  case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_UP:
							car.direct = 0;
							car.accelerate = 1;
							break;
						case SDLK_RIGHT:
							car.direct = 1;
							car.accelerate = 1;
							break;
						case SDLK_DOWN:
							car.direct = 2;
							car.accelerate = 1;
							break;
						case SDLK_LEFT:
							car.direct = 3;
							car.accelerate = 1;
							break;					
						case SDLK_ESCAPE:
							exit_key = 1;
							break;
						default:
							break;
					}
					break;
				    case SDL_KEYUP:
						car.accelerate = 0;
						break;
              }
         }
    }
    
    SDL_FreeSurface(screen);
	TTF_Quit();
    SDL_Quit();
  
    return 0;
}
