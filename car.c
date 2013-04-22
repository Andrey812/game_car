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
	int 		direct;				// Direction,  0 - Up, 1 - Right, 2 - Down, 3 - Left 
	int 		x;				// X coord the left upper corner of car sprite
	int 		y;				// Y coord the left upper corner of car sprite
	int 		accelerate;			// 0 - stop car, 1 - move car
	int		default_speed;			// Coordinate addition when move in pixels (initial)
	int 		speed;				// Coordinate addition when move in pixels (current)
	int 		speed_up_cycles;		// Count of moving cycles before speed increasing
	int		max_speed;			// Maximum car moving pixels limitation
	int		current_speed_up_cycle;		// Counter for speed up cycle
	Uint32 		move_refresh_time; 		// Delay for move
	Uint32 		move_last_refresh; 		// Last time delay for move
	SDL_Surface 	*spr_car; 			// Sprite of car
};

// Target data (object that car must catch on the map)
struct Target {
	int 		x;			// X coord the left upper corner of car sprite
	int 		y;			// Y coord the left upper corner of car sprite
	int 		frame_num; 		// Number of picture from set of pictures
	Uint32 		frame_refresh_time; 	// Time of frame's refresh
	Uint32 		frame_last_refresh; 	// Last refresh time
	int		frames[2];		// Sprite frames
	SDL_Surface *	spr_target; 		// Sprite of target
};

struct Dashboard {
	SDL_Surface *	surf_dhb;		// Dashboard image compiled one time from pieces
	SDL_Surface *	wheel_points_spr;	// Picture with wheel points
	SDL_Surface *	damages_spr;		// Picture for damage value visualization
	int 		need_update;		// Flag of needing update
	int		points_count;		// Count of the collected wheel points
	int		max_damage;		// Damage points
	int		curr_damage;		// Current damage counter
	int		permit_dmg;		// flag, can damage be updated or not (for prevent multiple damages by one object)
};

struct Ground {
	SDL_Surface *	surf_grd;		// Bacground image compiled one time from pieces
	int 		need_update;		// Flag of update needed
};

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

struct Car car;
struct Target target;
struct Dashboard dashboard;
struct Ground ground;

SDL_Surface *screen; // Main surface for displaying

// Prepare ground pieces picture
void init_ground() {
	
	ground.surf_grd = SDL_CreateRGBSurface(0,900,800,32,0,0,0,0);
	
	SDL_Surface *grd_sprites;
	
	grd_sprites = SDL_LoadBMP( "img/ground.bmp" );
    	grd_sprites = SDL_DisplayFormat(grd_sprites);
    
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
			
			SDL_BlitSurface(grd_sprites, &src, ground.surf_grd, &dest );
		};
	}
	SDL_FreeSurface(grd_sprites);
	ground.need_update = 1;
    
}

// Prepare dashboard compiled image
void init_dashboard() {
	
	dashboard.points_count = 0;
	
	dashboard.max_damage 	= 5;
	dashboard.curr_damage 	= 0;
	dashboard.permit_dmg 	= 1;
	
	// Generate of the dashboard background
	dashboard.surf_dhb = SDL_CreateRGBSurface(0,380,800,32,0,0,0,0);
	
	SDL_Surface *dhb_sprites;
	
	dhb_sprites = SDL_LoadBMP( "img/dashboard.bmp" );
    	dhb_sprites = SDL_DisplayFormat(dhb_sprites);
    
    	SDL_Rect src;
	SDL_Rect dest;
	
	int grd_row = 0;
	int grd_col = 0;
	int type = 4;
	
	for (grd_row = 0; grd_row < 75; grd_row++ ) {
		for ( grd_col = 0; grd_col < 35; grd_col++ ) {
			
			if ( grd_row == 0 || grd_row == 10 || grd_row == 23 ) {
				
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
			
			if ( grd_row == 9 || grd_row == 22 || grd_row == 74 ) {
				
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
			
			if ( grd_row != 0 && grd_row != 9 && grd_row != 10 && grd_row != 74
				&& grd_row != 22 && grd_row != 23) {
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
			
			dest.x = grd_col * 10;
			dest.y = grd_row * 10;
			dest.w = 10;
			dest.h = 10;
			
			SDL_BlitSurface(dhb_sprites, &src, dashboard.surf_dhb, &dest );
		};
	}
	
	SDL_FreeSurface(dhb_sprites);
	
	// Load image for wheel points indicator
	dashboard.wheel_points_spr = SDL_LoadBMP( "img/dashboard_wheel_points.bmp" );
    	dashboard.wheel_points_spr = SDL_DisplayFormat(dashboard.wheel_points_spr);
    	SDL_SetColorKey( dashboard.wheel_points_spr, SDL_SRCCOLORKEY, 0xFF00FF );
    
    	// Load image for damage visualization
    	dashboard.damages_spr = SDL_LoadBMP( "img/dashboard_car_damage.bmp" );
    	dashboard.damages_spr = SDL_DisplayFormat(dashboard.damages_spr);
    	SDL_SetColorKey( dashboard.damages_spr, SDL_SRCCOLORKEY, 0xFF00FF );
    	
	dashboard.need_update = 1;
}

// Copy ground surface to screen surface
void draw_ground() {
	
	SDL_Rect dest;
	
	SDL_BlitSurface(ground.surf_grd, NULL, screen, NULL );
	
	ground.need_update = 0;

}

// Copy dashboard surface to screen surface
void draw_dashboard() {
	
	SDL_Rect src;
	SDL_Rect dest;
	
	// Update car damage inicator
	int dmg_counter = 0;
	int dmg_k = 0;
	for ( dmg_counter = 0; dmg_counter < dashboard.max_damage; dmg_counter++ ) {
		
		if ( dashboard.curr_damage > dmg_counter ) {
			dmg_k = 125;
		}
		else {
			dmg_k = 0;
		}
		src.x = 125 / dashboard.max_damage * dmg_counter + dmg_k ;
		src.y = 0;
		src.w = 125 / dashboard.max_damage;
		src.h = 75;
		
		dest.x = 50 + ( 125 / dashboard.max_damage ) * dmg_counter;
		dest.y = 130;
		
		SDL_BlitSurface(dashboard.damages_spr, &src, dashboard.surf_dhb, &dest );
	}
	
	// Update collected wheel points indicator
	
	src.x = 40;
	src.y = 0;
	src.w = 40;
	src.h = 40;
	
	int counter;
	
	for ( counter = 0; counter < 8; counter++ ) {
		
		if ( counter >= dashboard.points_count ) {
			src.x = 0;
		}
		
		dest.x = 10 + 40 * counter;
		dest.y = 30;
		
		SDL_BlitSurface(dashboard.wheel_points_spr, &src, dashboard.surf_dhb, &dest );
	};
	
	// Add dashboard background
	
	dest.x = 904;
	dest.y = 0;
	
	SDL_BlitSurface(dashboard.surf_dhb, NULL, screen, &dest );
	
	dashboard.need_update = 0;
}

// Set default values for car
void init_car() {
	
	car.direct 			= 0;	//Direction "Up"
	car.x 				= 450;
	car.y 				= 300;
	car.speed_up_cycles 		= 40;
	car.current_speed_up_cycle 	= 0;
	car.accelerate 			= 0; 	//Car stopped
	car.default_speed 		= 2;
	car.speed 			= 2;
	car.max_speed 			= 6;
	car.move_refresh_time 		= 8; 	//Greater - slowly car
	
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
	
	if ( ground.need_update ) {
		draw_ground();
	}
	
	if ( dashboard.need_update ) {
		draw_dashboard();
	}
	
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
	
	car.current_speed_up_cycle++;
	
	if ( car.current_speed_up_cycle > car.speed_up_cycles 
		&& car.speed < car.max_speed ) {
		car.speed++;
		car.current_speed_up_cycle = 0;
	}
	
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
	
	int damage_happened = 0;
	
	/* simple temporary border detection */
	if ( car.x < 10 ) {
		car.x = 10;
		damage_happened = 1;
	}
	if ( car.x > 840 ) {
		car.x = 840;
		damage_happened = 1;
	}
	if ( car.y < 10 ) {
		car.y = 10;
		damage_happened = 1;
	}
	if ( car.y > 690 ) {
		car.y = 690;
		damage_happened = 1;
	}
	
	if ( damage_happened && dashboard.permit_dmg ) {
		dashboard.curr_damage++;
		dashboard.permit_dmg = 0;
		dashboard.need_update = 1;
	}
	
	/* simple target capture detection */
	if ( target.x + 20 >= car.x && target.x + 20 <= car.x + 50 
		&& target.y + 20 >= car.y && target.y + 20 <= car.y + 50 ) 
	{
		dashboard.points_count++;
		dashboard.need_update = 1;
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
			if ( now - car.move_last_refresh > car.move_refresh_time ) {
				move_car();
				update_screen = 1;
				ground.need_update = 1;
				car.move_last_refresh = now;
			}
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
						car.current_speed_up_cycle = 0;
						car.speed = car.default_speed;
						dashboard.permit_dmg = 1;
						break;
              }
         }
    }
    
    SDL_FreeSurface(screen);
	TTF_Quit();
    SDL_Quit();
  
    return 0;
}
