#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5\allegro_acodec.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "COL.h"

using namespace std;

bool redraw = true;
ALLEGRO_DISPLAY *DISPLAY;
int timer_start = time(NULL);
class Game {
public:
	bool music;
	ALLEGRO_BITMAP *image;
	ALLEGRO_BITMAP *loading;
	ALLEGRO_SAMPLE *sample;
};

class Sprite
{
public:
	int x;
	int y;
	int score;
	bool dead;
	float speed;

	int t[8][2];

	mask_t *mask;
	ALLEGRO_BITMAP *image;
};

int FPS = 60;
int SCREEN_W = 400;
int SCREEN_H = 640;

void initPlayer(Sprite &player)
{
	player.x = 160;
	player.y = 540;
	player.speed = 50;
	player.score = 0;
	player.dead = false;
}

void initEnemy(Sprite &enemy)
{
	enemy.y = 0;
	enemy.speed = 5;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 2; j++)
			(j == 1) ? enemy.t[i][j] = -425 : enemy.t[i][j] = 100 * (i % 4);
}

void initBullet(Sprite &bullet)
{
	bullet.speed = 10;

	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 2; j++)
			bullet.t[i][j] = 0;
}

void runBullet(Sprite &bullet, Sprite &enemy, Sprite &player)
{
	bool collision;
	for (int ID = 0; ID < 5; ID++)
	{
		if (bullet.t[ID][1] == -20 && time(NULL) - timer_start >= 1)
		{
			bullet.t[ID][0] = player.x + 37;
			bullet.t[ID][1] = player.y - 5;
			timer_start = time(NULL);
			al_draw_bitmap(bullet.image, bullet.t[ID][0], bullet.t[ID][1], 0);
		}
		if (bullet.t[ID][1] > -20)
		{
			bullet.t[ID][1] -= 5;
			al_draw_bitmap(bullet.image, bullet.t[ID][0], bullet.t[ID][1], 0);
		}
		for (int IDB = 0; IDB < 8; IDB++) {
			if (Mask_Collide(bullet.mask, enemy.mask, enemy.t[IDB][0] - bullet.t[ID][0], enemy.t[IDB][1] - bullet.t[ID][1])) {
				collision = true;
			}
			else {
				collision = false;
			}

			if (collision)
			{
				//printf("strzal %d %d\n", ID, IDB);
				enemy.t[IDB][0] = 9999;
				bullet.t[ID][0] = 999;
				player.score++;
			}
		}
	}
}
void runEnemy(Sprite &bullet, Sprite &player, Sprite &enemy)
{
	bool collision;
	for (int ID = 0; ID < 8; ID++)
	{
		if (ID < 4 && enemy.t[ID][1] == -425)
		{
			enemy.t[ID][0] = 100 * (ID % 4);
			enemy.t[ID][1] = -100;
			al_draw_bitmap(enemy.image, enemy.t[ID][0], enemy.t[ID][1], 0);
		}
		else if (ID > 3 && ID < 8 && 325 < enemy.t[ID - 4][1] && enemy.t[ID][1] == -425)
		{
			enemy.t[ID][0] = 100 * (ID % 4);
			enemy.t[ID][1] = -100;
			al_draw_bitmap(enemy.image, enemy.t[ID][0], enemy.t[ID][1], 0);
		}
		else if (enemy.t[ID][1] >= 650)
		{
			enemy.t[ID][0] = 100 * (ID % 4);
			enemy.t[ID][1] = -100;
		}

		if (enemy.t[ID][1] > -95 || enemy.t[ID][1] < 500)
		{
			enemy.t[ID][1] += enemy.speed;
			al_draw_bitmap(enemy.image, enemy.t[ID][0], enemy.t[ID][1], 0);


			if (Mask_Collide(player.mask, enemy.mask, player.x - enemy.t[ID][0], player.y - enemy.t[ID][1]))
				collision = true;
			else
				collision = false;

			if (collision)
			{
				//printf("kolizja %d\n",ID);
				enemy.t[ID][0] = 9999;
				player.dead = true;
			}
		}
		else
		{
			enemy.t[ID][1] = -100;
			enemy.t[ID][0] = 100 * ID % 4;
		}
	}
}


void foptions(bool opt, ALLEGRO_EVENT_QUEUE *fqoptions, Game &game, bool *leave_menu, ALLEGRO_SAMPLE *sample, ALLEGRO_SAMPLE_ID &sampleID)
{

	ALLEGRO_FONT *small_font = al_load_ttf_font("retro_font.TTF", 30, 0),
		*title_font = al_load_ttf_font("retro_font.TTF", 50, 0);

	enum { MENU_MUSIC, MENU_OPTIONS_EXIT };
	int menu_active = 0;
	const char *menu_opt_str[] = { "Muzyka", "Powrot do menu" };
	bool redraw = true;
	game.music = false;
	while (opt)
	{
		ALLEGRO_EVENT optionsEv;
		al_wait_for_event(fqoptions, &optionsEv);
		al_draw_bitmap(game.image, 0, 0, 0);
		al_draw_text(title_font, al_map_rgb(200, 50, 66), SCREEN_W / 2, 40, ALLEGRO_ALIGN_CENTRE, "Opcje");
		if (optionsEv.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		if (optionsEv.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (optionsEv.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				switch (menu_active)
				{
				case MENU_MUSIC:
					if (game.music == false)
					{
						al_draw_text(small_font, al_map_rgb(0, 255, 0), 300, 150, 0, "ON");
						al_play_sample(sample, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_LOOP, &sampleID);
						game.music = true;
						redraw = true;
					}
					else
					{
						al_draw_text(small_font, al_map_rgb(255, 0, 0), 300, 150, 0, "OFF");
						al_stop_sample(&sampleID);
						game.music = false;
						redraw = true;
					}
					break;
				case MENU_OPTIONS_EXIT:
					al_flush_event_queue(fqoptions);
					opt = false;
					break;
				}
				redraw = true;
			}
			else if (optionsEv.keyboard.keycode == ALLEGRO_KEY_DOWN)
			{
				menu_active = (menu_active + 1) % (MENU_OPTIONS_EXIT + 1);
				redraw = true;
			}
			else if (optionsEv.keyboard.keycode == ALLEGRO_KEY_UP)
			{
				menu_active = (menu_active + MENU_OPTIONS_EXIT) % (MENU_OPTIONS_EXIT + 1);
				redraw = true;
			}
		}

		for (int i = 0; i <= MENU_OPTIONS_EXIT; i++)
		{
			if (i == menu_active)
			{
				al_draw_text(small_font, al_map_rgb(255, 0, 0), SCREEN_W / 2, 50 + 50 * (i + 2), ALLEGRO_ALIGN_CENTRE, menu_opt_str[menu_active]);
			}
			else
			{
				al_draw_text(small_font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 50 + 50 * (i + 2), ALLEGRO_ALIGN_CENTRE, menu_opt_str[i]);
			}
		}
		if (redraw) al_flip_display();
		redraw = false;
	}
}

void menu(ALLEGRO_EVENT_QUEUE *event_queue, Game &game, bool *exit, bool *display_menu)
{
	ALLEGRO_FONT *small_font = al_load_ttf_font("retro_font.TTF", 30, 0),
		*title_font = al_load_ttf_font("retro_font.TTF", 50, 0);
	ALLEGRO_SAMPLE *sample = al_load_sample("sample.wav");
	ALLEGRO_SAMPLE_ID sampleID;

	al_reserve_samples(1);

	enum { MENU_START, MENU_OPTIONS, MENU_EXIT };
	int menu_active = 0;
	const char *menu_options_str[] = { "GRAJ", "OPCJE", "ZAMKNIJ" };
	bool redraw = true;
	bool leave_menu = false;
	bool options = false;
	bool opt = false;
	bool lol = true;
	while (!leave_menu)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				switch (menu_active)
				{
				case MENU_START:
					leave_menu = true;
					*display_menu = false;
					break;
				case MENU_OPTIONS:
					al_flush_event_queue(event_queue);
					opt = true;
					foptions(opt, event_queue, game, &leave_menu, sample, sampleID);
					break;
				case MENU_EXIT:
					*exit = true;
					leave_menu = true;
					break;
				}
				redraw = true;
			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
			{
				menu_active = (menu_active + 1) % (MENU_EXIT + 1);
				redraw = true;
			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_UP)
			{
				menu_active = (menu_active + MENU_EXIT) % (MENU_EXIT + 1);
				redraw = true;
			}
		}
		al_draw_bitmap(game.image, 0, 0, 0);
		al_draw_text(title_font, al_map_rgb(200, 50, 66), SCREEN_W / 2, 40, ALLEGRO_ALIGN_CENTRE, "Fallen Dream");
		for (int i = 0; i <= MENU_EXIT; i++)
		{
			if (i == menu_active)
			{
				al_draw_text(small_font, al_map_rgb(255, 0, 0), SCREEN_W / 2, 50 + 50 * (i + 2), ALLEGRO_ALIGN_CENTRE, menu_options_str[menu_active]);
			}
			else
			{
				al_draw_text(small_font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 50 + 50 * (i + 2), ALLEGRO_ALIGN_CENTRE, menu_options_str[i]);
			}
		}
		if (redraw) al_flip_display();
		redraw = false;
	}
}



int main(int argc, char **argv)
{
	al_init();
	al_init_image_addon();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();

	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_FONT *font = al_load_ttf_font("m04.TTF", 72, 0),
		*small_font = al_load_ttf_font("m04.TTF", 15, 0),
		*score_font = al_load_ttf_font("retro_font.TTF", 20, 0),
		*gameOver_font = al_load_ttf_font("retro_font.TTF", 50, 0),
		*tryAgain_font = al_load_ttf_font("retro_font.TTF", 30, 0);
	// =======================
	bool display_menu = true;
	bool exit = false;
	bool bound = false;
	bool collision = false;
	bool gameInProgress = true;
	// =======================

	timer = al_create_timer(1.0 / FPS);

	DISPLAY = al_create_display(SCREEN_W, SCREEN_H);
	al_set_target_bitmap(al_get_backbuffer(DISPLAY));

	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(DISPLAY));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	Game game;
	Sprite bullet;
	Sprite player;
	Sprite enemy;

	initPlayer(player);
	initBullet(bullet);
	initEnemy(enemy);


	game.image = al_load_bitmap("background.png");
	game.loading = al_load_bitmap("loading.png");

	player.image = al_load_bitmap("hero.png");
	enemy.image = al_load_bitmap("enemy.png");
	bullet.image = al_load_bitmap("shot.png");

	player.mask = Mask_New(player.image);
	enemy.mask = Mask_New(enemy.image);
	bullet.mask = Mask_New(bullet.image);

	al_draw_bitmap(game.image, 0, 0, 0);

	al_flip_display();

	al_start_timer(timer);

	while (gameInProgress) {
		ALLEGRO_EVENT ev;

		if (display_menu) {
			menu(event_queue, game, &exit, &display_menu);
			display_menu = false;
			redraw = true;
		}
		al_wait_for_event(event_queue, &ev);
		if (exit || ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER)
			redraw = true;

		if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT)
		{
			if (player.x > 20)
			{
				player.x -= player.speed;
				redraw = true;
			}
		}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT)
		{
			if (player.x < 300)
			{
				player.x += player.speed;
				redraw = true;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue))
		{
			if (!player.dead)
			{
				redraw = false;
				al_draw_bitmap(game.image, 0, 0, 0);

				al_draw_bitmap(player.image, player.x, player.y, 0);
				runBullet(bullet, enemy, player);
				runEnemy(bullet, player, enemy);

				al_draw_textf(score_font, al_map_rgb(200, 50, 66), 10, 10, 0, "%d", player.score);

				al_flip_display();
			}
			else if (player.dead)
			{
				al_stop_timer(timer);
				al_draw_bitmap(game.image, 0, 0, 0);
				al_draw_text(gameOver_font, al_map_rgb(100, 50, 66), SCREEN_W / 2, 100, ALLEGRO_ALIGN_CENTRE, "Game Over");
				al_draw_textf(gameOver_font, al_map_rgb(200, 50, 66), SCREEN_W / 2, 200, ALLEGRO_ALIGN_CENTRE, "Wynik: %d", player.score);
				al_draw_text(tryAgain_font, al_map_rgb(100, 50, 66), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTRE, "Sprobuj ponownie?");
				al_draw_text(tryAgain_font, al_map_rgb(100, 50, 66), SCREEN_W / 2, 350, ALLEGRO_ALIGN_CENTRE, "y/n");

				if (ev.keyboard.keycode == ALLEGRO_KEY_Y) {
					initPlayer(player);
					initBullet(bullet);
					initEnemy(enemy);
					player.dead = false;
					al_start_timer(timer);
				}
				if (ev.keyboard.keycode == ALLEGRO_KEY_N)
					//menu(event_queue, game, &exit, &display_menu);
					gameInProgress = false;
				al_flip_display();
			}
		}
	}

	al_destroy_timer(timer);
	al_destroy_bitmap(player.image);
	al_destroy_bitmap(game.image);
	al_destroy_bitmap(enemy.image);
	al_destroy_bitmap(bullet.image);
	al_destroy_event_queue(event_queue);
	al_destroy_display(DISPLAY);

	return 0;
}