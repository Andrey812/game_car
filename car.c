#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define WIDTH 1280
#define HEIGHT 800
#define BPP 4
#define DEPTH 32

/*
gcc -o car car.c `sdl-config --cflags --libs` -lSDL_ttf && ./car
*/

struct Car {
	int direct; 	// Direction,  0 - Up, 1 - Right, 2 - Down, 3 - Left 
	int x;			// X coord the left upper corner of car sprite
	int y;			// Y coord the left upper corner of car sprite
	int accelerate;	// 0 - stop car, 1 - move car
	int speed;		// Coordinate addition when move in pixels
};

struct Car car;

int target_x = 400;
int target_y = 670;

int coll_obj[5][4] = {
	{0, 0, 800, 14},
	{836, 0, 900, 836},
	{0, 636, 900, 736},
	{0, 0, 150, 150},
	{750, 0, 850, 150}
};

int is_collision(x,y) {
	int min_x = x;
	int max_x = x + 50;
	int min_y = y;
	int max_y = y + 50;
	
	int i;
	int x_c = 0;
	int y_c = 0;
	
	for (i = 0; i < sizeof(coll_obj); i++ ) {
		if (min_x >= coll_obj[i][0] && min_x <= coll_obj[i][2]) {
			x_c = 1;
		}
		if (max_x >= coll_obj[i][0] && max_x <= coll_obj[i][2]) {
			x_c = 1;
		}
		if (min_y >= coll_obj[i][1] && min_y <= coll_obj[i][3]) {
			y_c = 1;
		}
		if (max_y >= coll_obj[i][1] && max_y <= coll_obj[i][3]) {
			y_c = 1;
		}
		
		if (x_c && y_c){
			return 1;
		}
		else {
			x_c = 0;
			y_c = 0;
		}
	}
	
	return 0;
}

void DrawScreen(SDL_Surface* screen)
{ 
  
    if( SDL_MUSTLOCK(screen) ) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }
    
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  
    SDL_Flip(screen); 
}

void AddGround(SDL_Surface* ground, SDL_Surface* screen) {
	
	/* Ground structure */
	int grd[5][6] = {
		{5,1,1,1,1,5},
		{4,0,0,0,0,2},
		{4,0,0,0,0,2},
		{4,0,0,0,0,2},
		{5,3,3,3,3,5}
	};
	
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
			
			SDL_BlitSurface( ground, &src, screen, &dest );
		};
	}
}

void AddTarget(SDL_Surface* target, SDL_Surface* screen) {
	SDL_Rect dest;
	
	dest.x = target_x;
	dest.y = target_y;
	
	SDL_BlitSurface( target, NULL, screen, &dest );
}

void MoveCar(SDL_Surface* car_surface, SDL_Surface* ground, SDL_Surface* target, SDL_Surface* screen) {
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
	
	SDL_FillRect(screen, NULL, 0x000000);
	
	AddGround(ground, screen);
	AddTarget(target, screen);
    SDL_BlitSurface( car_surface, &src, screen, &dest ); /* car */
    
    int col = is_collision(car.x,car.y);
    
    char* string[64];
    snprintf(string, sizeof string, "x: %d y: %d coll: %d", car.x, car.y, col);
    
    /* Text */
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 20);
	SDL_Color foregroundColor = { 255, 255, 255 };
    SDL_Color backgroundColor = { 0, 0, 0 };
	SDL_Surface* text = TTF_RenderText_Shaded(font, string, foregroundColor, backgroundColor);
	SDL_Rect textLocation = { 960, 10, 0, 0 };
	SDL_BlitSurface(text, NULL, screen, &textLocation);
    
    DrawScreen(screen);
}

/* Set default values for car */
int init_car() {
	car.direct 	= 0; /* Direction Up */
	car.x 	= 600;
	car.y 	= 50;
	car.accelerate = 0; /* Stop */
	car.speed = 4;
}

int main(int argc, char* argv[])
{
    SDL_Surface *screen, *tmp_bmp, *car_surface, *ground, *target;
    SDL_Event event;
    
    TTF_Init();
    
    init_car();
	
	printf("%d\n", car.x);
  
    int exit_key = 0;
    
    //Check video initialisation
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    //Check screen initialisation
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE)))
    {
        SDL_Quit();
        return 1;
    }
    
    /* Load car picture */
    tmp_bmp = SDL_LoadBMP( "img/car.bmp" );
    car_surface = SDL_DisplayFormat(tmp_bmp);
	SDL_FreeSurface(tmp_bmp);
	
	/* Load ground picture */
	tmp_bmp = SDL_LoadBMP( "img/ground.bmp" );
    ground = SDL_DisplayFormat(tmp_bmp);
	SDL_FreeSurface(tmp_bmp);
	SDL_SetColorKey( car_surface, SDL_SRCCOLORKEY, 0xFF00FF );
	
	/* Load targe picture */
	tmp_bmp = SDL_LoadBMP( "img/target.bmp" );
    target = SDL_DisplayFormat(tmp_bmp);
	SDL_FreeSurface(tmp_bmp);
	SDL_SetColorKey( target, SDL_SRCCOLORKEY, 0xFF00FF );
		
	MoveCar(car_surface, ground, target, screen);
	   
    while(!exit_key) 
    {
		SDL_Delay(1);
		
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
			
			MoveCar(car_surface, ground, target, screen);
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





