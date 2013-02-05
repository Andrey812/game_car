#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define WIDTH 1280
#define HEIGHT 800
#define BPP 4
#define DEPTH 32

/*
gcc -o car car.c `sdl-config --cflags --libs` && ./car
*/

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

void MoveCar(int x, int y, int way, SDL_Surface* car, SDL_Surface* ground, SDL_Surface* target, SDL_Surface* screen) {
	SDL_Rect src;
	SDL_Rect dest;
	
	src.x = way * 50;
	src.y = 0;
	src.w = 50;
	src.h = 50;
	
	dest.x = x;
	dest.y = y;
	dest.w = 50;
	dest.h = 50;
	
	SDL_FillRect(screen, NULL, 0x000000);
	
	AddGround(ground, screen);
	AddTarget(target, screen);
    SDL_BlitSurface( car, &src, screen, &dest ); /* car */
    
    int col = is_collision(x,y);
    
    char* string[64];
    snprintf(string, sizeof string, "x: %d y: %d coll: %d", x, y, col);
    
    /* Text */
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 20);
	SDL_Color foregroundColor = { 255, 255, 255 };
    SDL_Color backgroundColor = { 0, 0, 0 };
	SDL_Surface* text = TTF_RenderText_Shaded(font, string, foregroundColor, backgroundColor);
	SDL_Rect textLocation = { 960, 10, 0, 0 };
	SDL_BlitSurface(text, NULL, screen, &textLocation);
    
    DrawScreen(screen);
}

int main(int argc, char* argv[])
{
    SDL_Surface *screen, *tmp_bmp, *car, *ground, *target;
    SDL_Event event;
    
    TTF_Init();
  
    int exit_key = 0;
    int car_way = 0;
	int car_x = 600;
	int car_y = 50;
	int car_accelerate = 0;
	int car_speed = 4;
  
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
    car = SDL_DisplayFormat(tmp_bmp);
	SDL_FreeSurface(tmp_bmp);
	
	/* Load ground picture */
	tmp_bmp = SDL_LoadBMP( "img/ground.bmp" );
    ground = SDL_DisplayFormat(tmp_bmp);
	SDL_FreeSurface(tmp_bmp);
	SDL_SetColorKey( car, SDL_SRCCOLORKEY, 0xFF00FF );
	
	/* Load targe picture */
	tmp_bmp = SDL_LoadBMP( "img/target.bmp" );
    target = SDL_DisplayFormat(tmp_bmp);
	SDL_FreeSurface(tmp_bmp);
	SDL_SetColorKey( target, SDL_SRCCOLORKEY, 0xFF00FF );
		
	MoveCar(car_x, car_y, car_way, car, ground, target, screen);
	   
    while(!exit_key) 
    {
		SDL_Delay(1);
		
		if ( car_accelerate ) {
			switch(car_way) {
				case 0:
					car_y = car_y - car_speed;
					
					break;
				case 1:
					car_x = car_x + car_speed;
					break;
				case 2:
					car_y = car_y + car_speed;
					break;
				case 3:
					car_x = car_x - car_speed;
					break;
				default:
					break;
			}
			
			MoveCar(car_x, car_y, car_way, car, ground, target, screen);
		}
		
         while(SDL_PollEvent(&event)) 
         {      
              switch (event.type) 
              {
				  case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_UP:
							car_way = 0;
							car_accelerate = 1;
							break;
						case SDLK_RIGHT:
							car_way = 1;
							car_accelerate = 1;
							break;
						case SDLK_DOWN:
							car_way = 2;
							car_accelerate = 1;
							break;
						case SDLK_LEFT:
							car_way = 3;
							car_accelerate = 1;
							break;					
						case SDLK_ESCAPE:
							car_x--;
							exit_key = 1;
							break;
						default:
							break;
					}
					break;
				    case SDL_KEYUP:
						car_accelerate = 0;
						break;
              }
         }
    }
    
    SDL_FreeSurface(screen);
	TTF_Quit();
    SDL_Quit();
  
    return 0;
}





