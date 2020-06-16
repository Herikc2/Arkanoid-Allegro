#include <allegro.h>

//funções
void sair();
void menu();
void enemys();
void actBol();
void blocos();
void control();
void restart();
int colidir(int, int, int, int, int, int, int, int);

//objetos
struct obj{ int wx, wy, x, y, w, h;};
struct obj
	bloco[10][10],
	life = {0,77,30,682,45,18},
	nave = {0,47,300,650,86,28},	
	bola = {48,77,330,630,20,20}
;	
struct obj2{ int wx, wy, x, y, w, h, ex, ey, base;};
struct obj2 enemy[4] = {0,0,200,200,28,44,0,0,500};

//Variáveis Globais
int sai    =   0;
int width  = 900;
int height = 700;
int pontos =   0;
int velx   =  -2;
int vely   =  -4;
int jogar  =   0;
int bAct   =   0;
int tAct   =   0;
int vidas  =   3;
int vida   =   0;
int play   =   0;
int out    =-500; 
int eFace  =   0;
int sIntro =   0;

BITMAP *buffer, *itens, *fundo, *logo, *log0;
FONT *f32;
SAMPLE *morto, *gover, *sBloco, *sNave, *sEnemy,
       *theme, *intro, *jogo, *end;

int main() {
	
    //Iniciação
	allegro_init();
    install_keyboard();
	set_color_depth(32);
	set_window_title("Arkanoid");
	set_close_button_callback(sair);
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);  
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, width, height, 0, 0);
	
	//Variáveis Locais
	buffer = create_bitmap(width, height);
	f32    = load_font("img/f32.pcx", NULL, NULL);
	itens  = load_bitmap("img/itens.bmp", NULL);
	fundo  = load_bitmap("img/fundo.bmp", NULL);
	logo   = load_bitmap("img/arkanoid.bmp", NULL);
	log0  = load_bitmap("img/arkanoib.bmp", NULL);
	morto  = load_sample("sons/morto.wav");
	intro  = load_sample("sons/intro.wav");
	jogo   = load_sample("sons/jogar.wav");
	sEnemy = load_sample("sons/enemy.wav");
	sBloco = load_sample("sons/bloco.wav");
	theme  = load_sample("sons/theme.wav");
	gover  = load_sample("sons/gover.wav");
	sNave  = load_sample("sons/nave.wav" );
	end    = load_sample("sons/end.wav"  );
	int i;
	
	menu();
	
	while (!(sai || key[KEY_ESC]))
	{
		draw_sprite(buffer, fundo, 0, 0);
		if(!play) restart();
		actBol();
		blocos();
		enemys();
		control();
		
		for(i = 0; i < vida -1; i++){
			masked_blit(itens, buffer, life.wx, life.wy, life.x + i * 50, life.y, life.w, life.h);
		}
		if(!play && !vida)  textout_centre_ex(buffer, f32, "Game Over", width/2.5, height/1.5, 0xffff, -1); 		
		masked_blit(itens, buffer, nave.wx, nave.wy, nave.x, nave.y, nave.w, nave.h);		
		masked_blit(itens, buffer, bola.wx, bola.wy, bola.x, bola.y, bola.w, bola.h);	
		textprintf_right_ex(buffer, f32, 880, 190, 0xffffff, -1," %i", pontos);
		textprintf_right_ex(buffer, f32, 880, 310, 0xffffff, -1," %i", pontos);
		draw_sprite(screen, buffer, 0, 0);
		rest(10);
		if(sIntro){rest(3000); sIntro = 0;} // pausa para intro
		if(!play && !vida){
			play_sample(gover, 255, 128, 1000, 0);
			rest(4000);
			menu();
		}
		clear(buffer);
				
	}
	
	//Finalização
	destroy_bitmap(buffer);
	destroy_sample(sBloco);
	destroy_sample(sEnemy);
	destroy_sample(intro);
	destroy_sample(morto);
	destroy_sample(gover);
	destroy_sample(sNave);
	destroy_bitmap(logo);
	destroy_bitmap(log0);
	destroy_bitmap(fundo);	
	destroy_sample(theme);
	destroy_sample(jogo);
	destroy_sample(end);
	destroy_bitmap(itens);
	destroy_font(f32);	
	return 0;
}
END_OF_MAIN();

void menu(){
	int time = 0, flash = 0;
	
	play_sample(theme, 255, 128, 1000, 1);	
	
	while (!key[KEY_SPACE]){
		if(key[KEY_ESC] || sai){
			sai = 1;
			return ;
		}
		if(time > 10)   time = 0;
		draw_sprite(buffer, logo, 0, 0);
		if(time < 5){
			draw_sprite(buffer, log0, 0, 0);
			textout_centre_ex(buffer, f32, "Press Start", width/2, height/1.3, 0xffff, -1);
		}
		draw_sprite(screen, buffer, 0, 0);	
		rest(100);
		clear(buffer);
		time++;
	}
	stop_sample(theme);
	play_sample(intro, 255, 128, 1000, 0);
	rest(7000);	
}

void enemys(){
	int i;
	eFace++;
	if(eFace >= 90) eFace = 0;
	for(i = 0; i < 4; i++){
		if(enemy[i].y >= 0){
			masked_blit(itens, buffer, enemy[i].wx + (eFace/30) * enemy[i].w, enemy[i].wy, enemy[i].x, enemy[i].y, enemy[i].w, enemy[i].h);
			if(colidir( enemy[i].x, enemy[i].y, enemy[i].w, enemy[i].h, bola.x, bola.y, bola.w, bola.h)){	
				if(bola.x + bola.w <= enemy[i].x || bola.x + 1 >= enemy[i].x + enemy[i].w) velx = -velx;
				else vely = -vely;
				play_sample(sEnemy, 255, 128, 1000, 0); 
		    	enemy[i].y    = out*2;
		    	enemy[i].base = out*2;
				pontos += 40;
			}
			if(colidir( enemy[i].x, enemy[i].y, enemy[i].w, enemy[i].h, nave.x, nave.y, nave.w, nave.h)){
				play_sample(sEnemy, 255, 128, 1000, 0);			
		 		enemy[i].y = out*2;
		    	enemy[i].base = out*2;
				pontos += 20;
			}	
		}	
		if(enemy[i].x <= 30) enemy[i].ex = 1;
		if(enemy[i].x >=665) enemy[i].ex = 0;
		if(enemy[i].y >= enemy[i].base){ enemy[i].base+= 100; enemy[i].ey = 0;}           
		if(enemy[i].y <= enemy[i].base - 300) enemy[i].ey = 1;
		enemy[i].ex ? enemy[i].x++ : enemy[i].x--;
		enemy[i].ey ? enemy[i].y++ : enemy[i].y--;
	}
}
void blocos(){
	int i, j;
	
	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++ ){	
			if(bloco[i][j].x != out){
				masked_blit(itens, buffer, bloco[i][j].wx, bloco[i][j].wy, bloco[i][j].x, bloco[i][j].y, bloco[i][j].w, bloco[i][j].h);
				if(colidir(bloco[i][j].x, bloco[i][j].y, bloco[i][j].w, bloco[i][j].h, bola.x, bola.y, bola.w, bola.h)){
					if(bola.x + bola.w <= bloco[i][j].x || bola.x + 1 >= bloco[i][j].x + bloco[i][j].w) velx = -velx;
					else vely = -vely;
					play_sample(sBloco, 255, 128, 1000, 0); 
					bloco[i][j].x = out;
					pontos += 10;	
				}						
			}
		}
	}	
}

void restart(){
	int i, j;
	
	if(!vida){
		vida = vidas;      
		
		for(i = 0; i < 10; i++){
			for(j = 0; j < 10; j++ ){
			
				bloco[i][j].wx = 98;
				bloco[i][j].wy =      j*29;
				bloco[i][j].x  = 36 + j*64;
				bloco[i][j].y  = 32 + i*30;
				bloco[i][j].w  = 64;
				bloco[i][j].h  = 30;			
			}
		}			
	}  // reposiciona os inimigos
	for(i = 0; i < 4; i++){
		enemy[i].base =-500 + (rand() % 5)*-200;
		enemy[i].x  = i*100;
		enemy[i].y  = -500  + (rand() % 5)*-200;
		enemy[i].w  = 28;
		enemy[i].h  = 44;
	}	
	jogar  =   0;
	nave.x = 300;
	bola.x = 333;
	bola.y = 630;
	velx   =  -2;
	vely   =  -4;
	bAct   =   0;
	tAct   =   0;
	play   =   1;
	play_sample(jogo, 255, 128, 1000, 0);
	sIntro = 1;		
}

void actBol(){
	if(bAct){
		bola.x += velx;
		bola.y += vely;
		if(bola.x < 30 || bola.x > 665) velx = -velx;
		if(bola.y < 30) vely = -vely;
		if(colidir(nave.x, nave.y, nave.w, 2, bola.x, bola.y + bola.h, bola.w, 2)){	
			if(bola.x + bola.w <= nave.x + nave.w/2 || bola.x >= nave.x && bola.x + bola.w < nave.x + nave.w/2) velx = -2;
			else velx = 2;
			play_sample(sNave, 255, 128, 1000, 0);
			vely = -vely;
		}
	}
	if(bola.y > 700){
		play_sample(morto, 255, 128, 1000, 0);
		nave.x = out;
		vida--;
		play = 0;
		rest(3000);	
	}	
}

void control(){
	if(key[KEY_LEFT ] && nave.x >  30) nave.x -= 10;	
	if(key[KEY_RIGHT] && nave.x < 597) nave.x += 10;
	if(!bAct && !jogar) bola.x = nave.x + 32;
	if((key[KEY_SPACE] || tAct == 300) && !jogar){
		play_sample(sNave, 255, 128, 1000, 0);
		bAct  = 1;
		jogar = 1;
	}
	tAct++;	
}	

int colidir(int Ax, int Ay, int Aw, int Ah, int Bx, int By, int Bw, int Bh){
	if(Ax + Aw >= Bx && Ax <= Bx + Bw && Ay + Ah >= By && Ay <= By + Bh || (Ax + Aw >= Bx && Ax <= Bx + Bw && !Ay && !Ah && !By && !Bh))
	return 1; return 0;
}
void sair(){sai= 1;}
END_OF_FUNCTION(sair);

