#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

const float FPS = 100;

int VIDA = 3;

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int NAVE_W = 100;
const int NAVE_Y = 50;

const int ENEMY_W = 60;
const int ENEMY_H = 50;

const int DMG_H = 60;;



void draw_scenario() {

	//deixa o background preto
	al_clear_to_color(al_map_rgb(0, 0, 0));

	//desenha o retangulo de colisao / se um inimigo encostar perde o jogo
	al_draw_filled_rectangle(0, SCREEN_H - DMG_H, SCREEN_W, SCREEN_H,
		al_map_rgb(0, 255, 0));

}

//struct da nave
typedef struct Nave {
	float x;
	float vel;
	int dir, esq;
	ALLEGRO_COLOR cor;

} Nave;
//struct do inimigo/obstaculo
typedef struct Enemy {
	float x, y;
	float x_vel, y_vel;
	ALLEGRO_COLOR cor;

}Enemy;
//inicializacao da nave
void initNave(Nave* nave) {
	nave->x = SCREEN_W / 2;
	nave->cor = al_map_rgb(0, 0, 255);
	nave->vel = 4;
	nave->dir = 0;
	nave->esq = 0;
}
//inicializacao do inimigo
void initEnemy(Enemy* enemy) {
	enemy->x = 10;
	enemy->y = 10;
	enemy->x_vel = 3;
	enemy->y_vel = 1;
	enemy->cor = al_map_rgb(100, 150, 150);
}

//desenho do inimigo (mudar pra um sprite depois)
void draw_enemy(Enemy enemy) {
	al_draw_filled_rounded_rectangle(enemy.x, enemy.y, enemy.x + ENEMY_W, enemy.y + ENEMY_H,
		10, 10, enemy.cor);
}

//desenho da nave (mudar pra um sprite depois)
void draw_nave(Nave nave, ALLEGRO_BITMAP *navezinha) {
	float y_base = SCREEN_H - DMG_H / 2;
	
	al_draw_bitmap(navezinha, nave.x - NAVE_W, y_base - DMG_H, nave.x + NAVE_W, SCREEN_H - DMG_H);
	
	al_draw_filled_triangle(nave.x, y_base - DMG_H,
		nave.x - NAVE_W / 2, y_base,
		nave.x + NAVE_W / 2, y_base,
		nave.cor);
	//hitbox colisao da nave com o inimigo
	al_draw_filled_rectangle(nave.x - NAVE_W / 3, y_base - DMG_H, nave.x + NAVE_W / 3, SCREEN_H - DMG_H,
		al_map_rgb(255, 0, 0));
}

void update_nave(Nave* nave) {
	if (nave->dir && nave->x + nave->vel <= SCREEN_W) {
		nave->x += nave->vel;
	}
	if (nave->esq && nave->x - nave->vel >= 0) {
		nave->x -= nave->vel;
	}
}

void update_enemy(Enemy* enemy) {

	if (enemy->x + ENEMY_W + enemy->x_vel > SCREEN_W || enemy->x + enemy->x_vel < 0) {
		enemy->y += enemy->y_vel;
		enemy->x_vel *= -1;
	}

	enemy->x += enemy->x_vel;
	enemy->y += enemy->y_vel;

	if (enemy->y + ENEMY_H > SCREEN_H - DMG_H) {
		VIDA--;
		enemy->x = 10;
		enemy->y = 10;
	}

}



int perdeu() {
	if (VIDA == 0) {
		return 0;
	}
}



int main() {

	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_BITMAP *navezinha = NULL;


	//inicializa o Allegro
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	//inicializa o módulo de primitivas do Allegro
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "failed to initialize primitives!\n");
		return -1;
	}


	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}



	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	al_init_ttf_addon();


	//inicializa o modulo que carrega as imagens
	al_init_image_addon();

	//carrega a imagem
	navezinha = al_load_bitmap("spaceship.bmp");



	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}



	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_flip_display();


	Nave nave;
	initNave(&nave);

	Enemy enemy;
	initEnemy(&enemy);

	ALLEGRO_FONT* fonte = al_load_font("arial.ttf", 20, NULL);

	int playing = 1;
	al_start_timer(timer);
	while (playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		draw_scenario();


		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if (ev.type == ALLEGRO_EVENT_TIMER) {


			update_nave(&nave);
			update_enemy(&enemy);

			draw_nave(nave, navezinha);

			

			draw_enemy(enemy);
			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W - 40, 40, ALLEGRO_ALIGN_CENTRE, "Vidas: %d", VIDA);
			playing = perdeu();

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();

			if (al_get_timer_count(timer) % (int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer) / FPS));
		}

		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

		//se o tipo de evento for um pressionar de uma tecla
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			switch (ev.keyboard.keycode) {

			case ALLEGRO_KEY_A:
				nave.esq = 1;
				break;
			case ALLEGRO_KEY_D:
				nave.dir = 1;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			switch (ev.keyboard.keycode) {

			case ALLEGRO_KEY_A:
				nave.esq = 0;
				break;
			case ALLEGRO_KEY_D:
				nave.dir = 0;
				break;
			}


		}
	}//fim do while

		//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)


	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}