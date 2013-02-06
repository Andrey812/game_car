#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define WIDTH 1280
#define HEIGHT 800
#define BPP 4
#define DEPTH 32

/*
gcc -o car car.c `sdl-config --cflags --libs` -lSDL_ttf && ./car
*/

/* Car data: coordinates, direction, speed */
struct Car {
	int direct; 	// Direction,  0 - Up, 1 - Right, 2 - Down, 3 - Left 
	int x;			// X coord the left upper corner of car sprite
	int y;			// Y coord the left upper corner of car sprite
	int accelerate;	// 0 - stop car, 1 - move car
	int speed;		// Coordinate addition when move in pixels
	SDL_Surface *spr_car; // Sprite of car
};

/* Target "asterisk" data */
struct Target {
	int x;	// X coord the left upper corner of car sprite
	int y;	// Y coord the left upper corner of car sprite
	SDL_Surface *spr_target; //Sprite of target
};

struct Car car;
struct Target target;

/* Ground map */
/*
int grd[5][6] = {
	{5,1,1,1,1,5},
	{4,0,0,0,0,2},
	{4,0,0,0,0,2},
	{4,0,0,0,0,2},
	{5,3,3,3,3,5}
};
*/

/* tmp ground for easy borders detection */
int grd[5][6] = {
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0}
};

SDL_Surface *screen, *ground;

void init_ground() {
	
	/* Load ground picture */
	ground = SDL_LoadBMP( "img/ground.bmp" );
    ground = SDL_DisplayFormat(ground);
}

void draw_ground() {
	
	SDL_Rect src;
	SDL_Rect dest;
	
	int grd_col = 0;
	int grd_row = 0;
	
	for (grd_row = 0; grd_row < 5; grd_row++ ) {
		for ( grd_col = 0; grd_col < 6; grd_col++ ) {
			src.x = grd[grd_row][grd_col] * 150;
			src.y = 0;
			src.w = 150;
			src.h = 150;
			
			dest.x = grd_col * 150;
			dest.y = grd_row * 150;
			dest.w = 150;
			dest.h = 150;
			
			SDL_BlitSurface(ground, &src, screen, &dest );
		};
	}
}

/* Set default values for car */
void init_car() {
	
	/* Car data */
	car.direct 	= 0; /* Direction Up */
	car.x 	= 450;
	car.y 	= 300;
	car.accelerate = 0; /* Stop */
	car.speed = 6;
	
	/* Load car picture */
    car.spr_car = SDL_LoadBMP( "img/car.bmp" );
    car.spr_car = SDL_DisplayFormat(car.spr_car);
    SDL_SetColorKey( car.spr_car, SDL_SRCCOLORKEY, 0xFF00FF );
}

/* generate new position for target */
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

void init_target() {
	
	/* Load target picture */
	target.spr_target = SDL_LoadBMP( "img/target.bmp" );
    target.spr_target = SDL_DisplayFormat(target.spr_target);
	SDL_SetColorKey( target.spr_target, SDL_SRCCOLORKEY, 0xFF00FF );
	
	set_target();
}

void DrawScreen()
{ 
  
    if( SDL_MUSTLOCK(screen) ) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }
    
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  
    SDL_Flip(screen); 
}

void move_car() {
	SDL_Rect src;
	SDL_Rect dest;
	
	/* simple temporary border detection */
	if ( car.x < 0 ) {
		car.x = 0;
	}
	if ( car.x > 850 ) {
		car.x = 850;
	}
	if ( car.y < 0 ) {
		car.y = 0;
	}
	if ( car.y > 700 ) {
		car.y = 700;
	}
	
	/* simple target capture detection */
	if ( target.x + 20 >= car.x && target.x + 20 <= car.x + 50 
		&& target.y + 20 >= car.y && target.y + 20 <= car.y + 50 ) 
	{
		set_target();
	}
	
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
	
	/* Add ground */
	draw_ground();
	
	/* Add target */
	SDL_Rect trg_dest;
	trg_dest.x = target.x;
	trg_dest.y = target.y;
	SDL_BlitSurface( target.spr_target, NULL, screen, &trg_dest );
	
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
    init_car();
    init_target();
    	
	move_car();
	
	int exit_key = 0;
	   
    while(!exit_key) 
    {
		SDL_Delay(10);
		
		if ( car.accelerate ) {
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
			
			move_car();
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





