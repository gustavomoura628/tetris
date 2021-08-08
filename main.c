#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

int randomInt(int b0, int b1){
	int rn;
	rn = rand();
	rn = (rn%(b1-b0+1))+b0;
	return rn;
}

int tetrominoes[7][4][4]= {
			0,0,0,0,
			1,1,1,1,
			0,0,0,0,
			0,0,0,0,

			0,0,0,0,
			0,1,1,0,
			0,1,1,0,
			0,0,0,0,

			0,0,0,0,
			0,1,1,0,
			1,1,0,0,
			0,0,0,0,
			
			0,0,0,0,
			1,1,0,0,
			0,1,1,0,
			0,0,0,0,

			1,1,1,0,
			0,0,1,0,
			0,0,0,0,
			0,0,0,0,
			
			0,0,0,0,
			0,0,1,0,
			1,1,1,0,
			0,0,0,0,

			0,1,0,0,
			0,1,1,0,
			0,1,0,0,
			0,0,0,0,
};
int main(){
	srand(time(0));

	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();

	al_install_keyboard();
	al_install_mouse();
	
	ALLEGRO_DISPLAY * display = al_create_display(550, 700);
	ALLEGRO_FONT * font = al_load_ttf_font("game_over.ttf", 64, 0);
	ALLEGRO_EVENT_QUEUE * queue = al_create_event_queue();
	ALLEGRO_TIMER * timer = al_create_timer(1.0 / 60);
	//ALLEGRO_BITMAP * bitmap = al_load_bitmap("image.png");
	//assert(bitmap != NULL);

	al_start_timer(timer);
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	
	int width = al_get_display_width(display);
	int height = al_get_display_height(display);
	int running = 1;
	int i,j;

	int gridx =10;
	int gridy = 20;
	int gridoffsetx=200;
	int gridoffsety=0;
	int gridwall=3;
	int gridwidth=width-gridoffsetx;
	int gridheight=height-gridoffsety;
	int gridsquarewidth=gridwidth/gridx;
	int gridsquareheight=gridheight/gridy;

	int getpixelpositionx(int x){
		return x*gridsquarewidth+gridoffsetx;
	}
	int getpixelpositiony(int y){
		return y*gridsquareheight+gridoffsety;
	}

	int **grid = malloc(sizeof grid	* gridx);
	for(i=0;i<gridx;i++){
		grid[i] = malloc(sizeof grid[i] * gridy);
		for(j=0;j<gridy;j++){
			grid[i][j]=0;//(i+j)%2;
		}
	}

	enum MYKEYS {
		KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
	};
	int key[4] = {0, 0, 0, 0};
	int previouskey[4] = {0, 0, 0, 0};

	void drawgrid(){
		int i, j;
		for(i=0;i<gridx;i++){
			for(j=0;j<gridy;j++){	
				al_draw_filled_rectangle(getpixelpositionx(i), getpixelpositiony(j), getpixelpositionx(i+1), getpixelpositiony(j+1), al_map_rgb(100, 100, 100));
				if(grid[i][j])al_draw_filled_rectangle(getpixelpositionx(i)+gridwall, getpixelpositiony(j)+gridwall, getpixelpositionx(i+1)-gridwall, getpixelpositiony(j+1)-gridwall, al_map_rgb(255, 255, 255));
				else if(!grid[i][j])al_draw_filled_rectangle(getpixelpositionx(i)+gridwall, getpixelpositiony(j)+gridwall, getpixelpositionx(i+1)-gridwall, getpixelpositiony(j+1)-gridwall, al_map_rgb(0, 0, 0));
			}
		}
	}
	void drawsquare(int x, int y){
		al_draw_filled_rectangle(getpixelpositionx(x)+gridwall, getpixelpositiony(y)+gridwall, getpixelpositionx(x+1)-gridwall, getpixelpositiony(y+1)-gridwall, al_map_rgb(255, 255, 255));
	}

	unsigned long int counting=1;

	int tetrominox=gridx/2 -2;
	int tetrominoy=0;
	int tetromino[4][4];
	int nexttetromino[4][4];
	int tetrominotype=randomInt(0,6);
	int nexttetrominotype=randomInt(0,6);
	int score=0;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			tetromino[j][i]=tetrominoes[tetrominotype][j][i];
		}
	}
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			nexttetromino[j][i]=tetrominoes[nexttetrominotype][j][i];
		}
	}

	void drawtetromino(int tetromino[4][4], int tx, int ty){
		int i,j;
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(tetromino[i][j])drawsquare(tx+i,ty+j);
			}
		}
	}
	int rotatetetromino(int **t){
		int aux[4][4];
		int i,j;
		int f0=0,f1=1;
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				aux[i][j]=tetromino[i][j];
			}
		}

		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(!(i!=0 && i!=3 && (i==j || 3-i == j)) && tetromino[i][j]!=0)f0=1;
				if(tetromino[i][j]!=tetrominoes[1][i][j])f1=0;
			}
		}
		if(f1)return ;
		for(i=0;i<3;i++){
			for(j=0;j<3;j++){
				tetromino[i][j]=aux[j][2-i];
			}
		}
		if(f0){
			int temp;
			temp=tetromino[1][3];
			tetromino[1][3]=tetromino[3][1];
			tetromino[3][1]=temp;
		}


	}

	int checkoutofbounds(){
		//printf("Here3\n");
		int i,j;
		int f=0;
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(tetromino[i][j]){
					if(tetrominox+i<0){
						//tetrominox++;
						f=1;
					}
					else if(tetrominox+i>=gridx){
						//tetrominox--;
						f=1;
					}
					else if(tetrominoy+j<0 || tetrominoy+j>gridy){
					}
					else if((grid[tetrominox+i][tetrominoy+j])){
						//tetrominox++;
						f=1;
					}
					else if(grid[tetrominox+i][tetrominoy+j]){
						//tetrominox--;
						f=1;
					}
				}/*
				if(tetromino[i][j] && tetrominoy+j<0){
					tetrominoy++;
					f=1;
				}
				if(tetromino[i][j] && tetrominoy+i<gridy){
					tetrominoy--;
					f=1;
				}*/
			}
		}
		//if(f)checkoutofbounds();
		//printf("Here4\n");
		return f;
	}
	int checkground(){
		//printf("Here1\n");
		int i,j;
		int f=0;
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(tetromino[i][j]){
					if(tetrominoy+j<0){
						tetrominoy++;
						f=1;
					}
					else if(tetrominoy+j>=gridy){
						tetrominoy--;
						f=1;
					}
					else if(tetrominox+i<0 || tetrominox+i>gridx){
					}
					//else if((grid[tetrominox+i][tetrominoy+j])){
					//	tetrominoy++;
					//	f=1;
					//}
					else if(grid[tetrominox+i][tetrominoy+j]){
						tetrominoy--;
						f=1;
					}
				}
				/*
				if(tetromino[i][j] && (tetrominoy+j<0)){
					tetrominoy++;
					f=1;
				}
				else if(tetromino[i][j] && (grid[i][tetrominoy+j])){
					tetrominoy++;
					f=1;
				}
				if(tetromino[i][j] && (tetrominoy+j>=gridy)){
					tetrominoy--;
					f=1;
				}
				else if(tetromino[i][j] && (grid[i][tetrominoy+j])){
					tetrominoy--;
					f=1;
				}*//*
				if(tetromino[i][j] && tetrominoy+j<0){
					tetrominoy++;
					f=1;
				}
				if(tetromino[i][j] && tetrominoy+i<gridy){
					tetrominoy--;
					f=1;
				}*/
			}
		}
		if(f)checkground();
		//printf("Here2\n");
		return f;
	}

	int removelines(){
		int i, j;
		int line=0;
		int hasstuff=1;
		for(i=gridy-1;i>0;i--){
			if(line){
				printf("Line %d is full\n", i);
				break;
			}
			if(!hasstuff)break;
			line=1;
			hasstuff=0;
			for(j=0;j<gridx;j++){
				if(!grid[j][i])line=0;
				else(hasstuff=1);
			}
		}
		if(line){
			score++;
			hasstuff=1;
			for(;i>=0;i--){
				if(!hasstuff)break;
				hasstuff=0;
				for(j=0;j<gridx;j++){
					grid[j][i+1]=grid[j][i];
					if(grid[j][i+1])hasstuff=1;
				}
			}
			if(hasstuff){
				for(i=0;i<gridx;i++){
					grid[i][0]=0;
				}
			}
			removelines();
		}
		return line;
	}


	
	int fallspeed = 120;
	int mousex,mousey;
	int downspeed=0;
	int hspeed=0;
	while(running){
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);


		if(event.type == ALLEGRO_EVENT_TIMER){
			
			//Direction Arrows
			
			


			//logic
			
			int ground=0;	
			if(counting % fallspeed == 0){
				tetrominoy++;
				ground=checkground();
			}
			if(key[KEY_UP] && key[KEY_UP]!=previouskey[KEY_UP]){
				rotatetetromino(tetromino);
				if(checkoutofbounds()){
					rotatetetromino(tetromino);
					rotatetetromino(tetromino);
					rotatetetromino(tetromino);
				}
				ground=checkground();
			}
			if(key[KEY_DOWN]){// && key[KEY_DOWN]!=previouskey[KEY_DOWN]){
				downspeed++;
				if(downspeed > 5 || key[KEY_DOWN]!=previouskey[KEY_DOWN]){
				tetrominoy++;
				ground=checkground();
				downspeed=0;
				}

			}
			if(key[KEY_LEFT]){
				hspeed++;
				if(hspeed>5 || key[KEY_LEFT]!=previouskey[KEY_LEFT]){
					tetrominox--;
					hspeed=0;
				}
				if(checkoutofbounds()){
					tetrominox++;
				}
			}
			if(key[KEY_RIGHT]){
				hspeed++;
				if(hspeed > 5 || key[KEY_RIGHT]!=previouskey[KEY_RIGHT]){
					tetrominox++;
					hspeed=0;
				}
				if(checkoutofbounds()){
					tetrominox--;
				}
			}
			checkoutofbounds();
			if(ground){
				//printf("Here0\n");
				for(i=0;i<4;i++){
					for(j=0;j<4;j++){
						if(tetrominox+i >= 0 && tetrominox + i < gridx && tetrominoy+j >= 0 && tetrominoy+j<gridy){
							grid[tetrominox+i][tetrominoy+j]+=tetromino[i][j];
							if(grid[tetrominox+i][tetrominoy+j])grid[tetrominox+i][tetrominoy+j]=1;	
						}
					}
				}
				tetrominox=gridx/2-2;
				tetrominoy=0;
				tetrominotype=nexttetrominotype;
				nexttetrominotype=randomInt(0,6);
				printf("Int = %d\n", tetrominotype);
				for(i=0;i<4;i++){
					for(j=0;j<4;j++){
						tetromino[j][i]=tetrominoes[tetrominotype][j][i];
					}
				}
				for(i=0;i<4;i++){
					for(j=0;j<4;j++){
						nexttetromino[j][i]=tetrominoes[nexttetrominotype][j][i];
					}
				}
				for(i=0;i<4;i++){
					for(j=0;j<4;j++){
						if(tetromino[i][j] && grid[tetrominox+i][tetrominoy+j]){
							printf("End\n");
							al_draw_text(font,al_map_rgb(0,0,0),getpixelpositionx(3)+2,getpixelpositiony(2),ALLEGRO_ALIGN_LEFT,"GAME OVER");
							al_draw_text(font,al_map_rgb(0,0,0),getpixelpositionx(3)-2,getpixelpositiony(2),ALLEGRO_ALIGN_LEFT,"GAME OVER");
							al_draw_text(font,al_map_rgb(0,0,0),getpixelpositionx(3),getpixelpositiony(2)+2,ALLEGRO_ALIGN_LEFT,"GAME OVER");
							al_draw_text(font,al_map_rgb(0,0,0),getpixelpositionx(3),getpixelpositiony(2)-2,ALLEGRO_ALIGN_LEFT,"GAME OVER");
							al_draw_text(font,al_map_rgb(255,255,255),getpixelpositionx(3),getpixelpositiony(2),ALLEGRO_ALIGN_LEFT,"GAME OVER");
							al_flip_display();
							running=0;
							sleep(3);
						}
					}
				}
				//removelines();
				if(removelines())system("~/c_programs/pasine/pasinesystem 500 500 &");

			}

			//DRAW
			al_clear_to_color(al_map_rgb(0, 0, 0));
			drawgrid();
			drawtetromino(tetromino, tetrominox, tetrominoy);
			drawtetromino(nexttetromino, -4, 0);
			//al_draw_filled_rectangle(mousex-5,mousey-5,mousex+5,mousey+5,al_map_rgb(255,0,0));
			char scoretext[1000]="Score: ";
			char scorevalue[1000];
			sprintf(scorevalue,"%d", score);
			strcat(scoretext,scorevalue);
			al_draw_text(font,al_map_rgb(255,255,255),getpixelpositionx(-5),getpixelpositiony(4),ALLEGRO_ALIGN_LEFT,scoretext);
			al_flip_display();
			
			
			for(i=0;i<4;i++){
				previouskey[i]=key[i];
			}
			if(counting % 120 == 0){
				//rotatetetromino(tetromino);
			}
			counting++;
		}
		else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)// || event.type == ALLEGRO_EVENT_KEY_UP)
			running = 0;
	
		//mouse
		else if(event.type == ALLEGRO_EVENT_MOUSE_AXES){
			mousex = event.mouse.x;
			mousey = event.mouse.y;
		}
		
		else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
			//xl=al_get_display_width(display)/2;
			//yl=al_get_display_height(display)/2;
			//al_set_mouse_xy(display, al_get_display_width(display)/2, al_get_display_height(display)/2);
		}		
		else if(event.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch(event.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = true;
					break;

				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = true;
					break;

				case ALLEGRO_KEY_LEFT:
					key[KEY_LEFT] = true;
					break;

				case ALLEGRO_KEY_RIGHT:
					key[KEY_RIGHT] = true;
					break;
			}
		}
		else if(event.type == ALLEGRO_EVENT_KEY_UP) {
			switch(event.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = false;
					break;

				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = false;
					break;

				case ALLEGRO_KEY_LEFT:
					key[KEY_LEFT] = false;
					break;

				case ALLEGRO_KEY_RIGHT:
					key[KEY_RIGHT] = false;
					break;

			}
		}
	
	}
	al_destroy_display(display);
	al_destroy_font(font);
	al_destroy_timer(timer);
	//al_destroy_bitmap(bitmap);

	al_uninstall_keyboard();
	al_uninstall_mouse();
	return 0;
}
