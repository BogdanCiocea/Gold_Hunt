#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define LENGTH_OF_MAP 28
#define MAX_STRING 256
#define MAX_LEVEL_POINTS 1000

//const int LENGTH_OF_MAP = LENGTH_OF_MAP;
int points = 0, lives, fruitx, fruity, snake_x, snake_y;
int gameover, flag, difficulty, highscore;
int *barriers;
int level = 0;

bool secret_level = false;
bool ninth_level = false;
bool tenth_level = false;
bool eleventh_level = false;

int nightcall = 0;
int change_back_song = 3;
int song_number;
char *default_song;

int bullet_left_x = -1, bullet_left_y = -1;
int bullet_right_x = -1, bullet_right_y = -1;
int bullet_up_x = -1, bullet_up_y = -1;
int bullet_down_x = -1, bullet_down_y = -1;

void playSong(const char *songPath) {
	system("pkill mplayer");
    char command[256];
    snprintf(command, sizeof(command), "mplayer -loop 9999 sounds/%s.mp3 > /dev/null 2>&1 &", songPath);
    system(command);
}

void playlist()
{
	const char *playlist[] = {
		"resonance",
		"just_ken",
		"nightcall",
		"retro-wave",
		"8bittown",
		"swamp",
		"frontier",
		"zelda",
		"memory_reboot"
    };

	int numSongs = sizeof(playlist) / sizeof(playlist[0]);

	if (song_number < 0)
		song_number = numSongs - 1;

	if (song_number >= numSongs)
		song_number = song_number % numSongs;

	playSong(playlist[song_number]);
}

void setup()
{
    gameover = 0;
	lives = 3;
    barriers = (int*)malloc(LENGTH_OF_MAP * LENGTH_OF_MAP * 2 * sizeof(int));

    for (int i = 0; i < LENGTH_OF_MAP * LENGTH_OF_MAP * 2; i++)
        barriers[i] = rand();

    for (int i = 0; i < LENGTH_OF_MAP * LENGTH_OF_MAP * 2; i++) {
        if (barriers[i] % difficulty == 0)
            barriers[i] = 1;
        else
            barriers[i] = 0;
    }

    snake_x = LENGTH_OF_MAP * 2;
    snake_y = LENGTH_OF_MAP / 2;
    flag = 1;

    // Generate fruit coordinates
    fruitx = rand() % LENGTH_OF_MAP * 4;
    fruity = rand() % LENGTH_OF_MAP;

    // Check if the fruit collides with barriers
    while (barriers[fruity * (LENGTH_OF_MAP * 2) + fruitx]) {
        fruitx = rand() % LENGTH_OF_MAP * 4;
        fruity = rand() % LENGTH_OF_MAP;
    }

    barriers[fruity * (LENGTH_OF_MAP * 2) + fruitx] = 0; // Clear barrier at fruit position

    // Generate barriers and avoid fruit position
    for (int i = 0; i < LENGTH_OF_MAP * LENGTH_OF_MAP * 2; i++) {
        if (barriers[i] == 1) {
            barriers[i] = rand() % difficulty == 0 ? 1 : 0;
        }
    }

    FILE *fp = fopen("highscore.txt", "rw");
    fscanf(fp, "%d", &highscore);
    fclose(fp);

	default_song = "2020-03-22_-_8_Bit_Surf_-_FesliyanStudios.com_-_David_Renda";
	song_number = -1;
	change_back_song = 3;
}

void display_pro_tips()
{
	char *pro_tips[MAX_STRING] = {
        "Master the Controls: Get comfortable with the controls early on. Precise movements and quick reactions are essential to surviving in the game.",
        "Use Bullets Wisely: Bullets are powerful tools. Use them to clear barriers efficiently or to target fruit in hard-to-reach spots.",
        "Explore the Map: Take the time to explore the entire map. Discovering portals and safe paths can be a game-changer.",
        "Avoid Recklessness: Rushing into actions can lead to mistakes. Stay patient and focused to avoid unnecessary collisions and loss of lives.",
        "Highscore Challenge: Strive to beat your highscore. Push your limits and continuously improve your point-gathering skills.",
        "Use Sound Cues: Pay attention to sound cues. They can alert you to barriers, bullets, and other events, helping you react faster.",
        "Practice Precision: Precision is key when shooting bullets. Practice aiming accurately to take down barriers efficiently.",
        "Watch the Border: The map's border can be tricky. Use it to your advantage when escaping enemies or navigating through tight spaces.",
        "Pew Pew Banana Blaster: Blaster goes pew pew! Shoot barrels, go pew pew. Big smiles, pew pew fun!",
		"Hot Barrel, No Touch: Barrels are hot. No touch, jump away. No hugs for hot!",
		"googoo gaga",
		"amogus",
		"hola amigos! today i am going to teach you how to make a taco so savory, so...so spicy so....Salamanca",
		"sus",
		"hello there",
		"General Kenobi, you are a bold one...",
		"You don't know the POWER of the dark side",
		"I am vengence. I am the knight. I AM BATMAN!",
		"I'm Homelander and I can do whatever the fuck i want",
		"I am Ryan Gosling"
    };

	int random_number = rand() % 20;

	if (pro_tips[random_number])
		printf("\033[1;31mPro tip:\033[0m\033[1;32m %s\033[0m\n\n", pro_tips[random_number]);
}

void draw() {
	system("clear");
	for (int i = 0; i < LENGTH_OF_MAP; i++) {
		for (int j = 0; j < LENGTH_OF_MAP * 4; j++) {

			int isBarrier = barriers[i * (LENGTH_OF_MAP * 2) + j];

			if (i == 0 || i == LENGTH_OF_MAP - 1 || j == 0 || j == LENGTH_OF_MAP * 4 - 1) {
				if (i == LENGTH_OF_MAP / 2 || i == LENGTH_OF_MAP / 2 + 1 
					|| i == LENGTH_OF_MAP / 2 - 1 || j == LENGTH_OF_MAP * 2
					|| j == LENGTH_OF_MAP * 2 + 1 || j == LENGTH_OF_MAP * 2 - 1
					|| j == LENGTH_OF_MAP * 2 + 2 || j == LENGTH_OF_MAP * 2 - 2
					|| j == LENGTH_OF_MAP * 2 + 3 || j == LENGTH_OF_MAP * 2 - 3)
					// Portals woooooooooooooooooooo
					printf("\033[1;35m.\033[0m"); 
				else
					printf("#");
			} else if (i == fruity && j == fruitx) {
				printf("\033[1;33m$\033[0m");
			} else if (i == snake_y && j == snake_x) {
				printf("\033[1;32m@\033[0m");
			} else if (isBarrier) {
				printf("\033[1;31m^\033[0m");
			} else if ((i == bullet_left_y && j == bullet_left_x) || (i == bullet_right_y && j == bullet_right_x)) {
				// Bullets pew pew
				printf("\033[1;33m-\033[0m");
			} else if ((i == bullet_up_y && j == bullet_up_x) || (i == bullet_down_y && j == bullet_down_x)) {
				// Bullets pew pew
				printf("\033[1;33m|\033[0m");
			} else
				printf(" ");
		}
		printf("\n");
	}

	printf("\n");

	if (secret_level)
		printf("\033[1;31mSecret level\033[0m\n");

	printf("\033[1;33mLevel %d\033[0m\n", level);
	//printf("Take the gold and watch out for the mines!\n");
	printf("Points: %d\nLives: %d\n", points, lives);
	printf("Highscore: %d\n", highscore);

	if (song_number == -1) {
		printf("\033[1;32mPress 9 to change the song\033[0m\n");
		printf("\033[1;32mPress 7 to change back\033[0m\n");
	}

	if (change_back_song != 3)
		printf("\033[1;32mPress 1 to change back to the default song\033[0m\n");

	printf("Press x to quit the game!\n");
}

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void input()
{
	if (kbhit()) {
		switch (getchar()) {
			/* Movement */

			// left
			case 'a':
				flag = 1;
				break;

			// down
			case 's':
				flag = 2;
				break;

			// right
			case 'd':
				flag = 3;
				break;

			// up
			case 'w':
				flag = 4;
				break;
			case 'x':
				gameover = 1;
				break;

			/*now time for guns.
			 lots of guns.*/
			
			// left
			case '4':
				flag = 5;
				break;
			
			// up
			case '8':
				flag = 6;
				break;
			
			// right
			case '6':
				flag = 7;
				break;

			// down
			case '2':
				flag = 8;
				break;

			// literally me
			case '1':
				flag = 9;
				break;
			
			// change back playlist
			case '7':
				flag = 10;
				break;
			
			// change forward playlist
			case '9':
				flag = 11;
				break;
		}
	}
}

void delay(int milliseconds) {
    long pause;
    clock_t now, then;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ((now - then) < pause) {
        now = clock();
    }
}

void playSound(const char *soundFile, int volume) {
    char command[100];
    snprintf(command, sizeof(command), "mplayer -volume %d %s > /dev/null 2>&1 &", volume, soundFile);
    FILE *fp = popen(command, "r");
    if (fp) {
        pclose(fp);
    }
}

void handle_bullets()
{
	/* LEFT */
	if (bullet_left_x >= 0 && bullet_left_y >= 0) {
		while (bullet_left_x >= 0 && bullet_left_y >= 0) {
			// Check if the bullet's position is within the boundaries and if there's a barrier to hit
			if (barriers[bullet_left_y * (LENGTH_OF_MAP * 2) + bullet_left_x] == 1) {
				barriers[bullet_left_y * (LENGTH_OF_MAP * 2) + bullet_left_x] = 0; // Remove the barrier
				bullet_left_x = -1; // Reset the bullet's position
				bullet_left_y = -1;
				break; // Exit the loop when a barrier is hit
			}

			// Move the bullet
			bullet_left_x--;

			// Exit the loop when the bullet goes out of bounds
			if (bullet_left_x < 0) {
				bullet_left_x = -1;
				bullet_left_y = -1;
				break;
			}

			delay(2);
			fflush(stdout);
			draw(); // Redraw the screen after updating bullet position
		}

	/* RIGHT */
	} else if (bullet_right_x >= 0 && bullet_right_y >= 0) {
		while (bullet_right_x >= 0 && bullet_right_y >= 0) {
			// Check if the bullet's position is within the boundaries and if there's a barrier to hit
			if (barriers[bullet_right_y * (LENGTH_OF_MAP * 2) + bullet_right_x] == 1) {
				barriers[bullet_right_y * (LENGTH_OF_MAP * 2) + bullet_right_x] = 0; // Remove the barrier
				bullet_right_y = -1; // Reset the bullet's position
				bullet_right_x = -1;
				break; // Exit the loop when a barrier is hit
			}

			// Move the bullet
			bullet_right_x++;

			// Exit the loop when the bullet goes out of bounds
			if (bullet_right_x > LENGTH_OF_MAP * 4) {
				bullet_right_y = -1; // Reset the bullet's position
				bullet_right_x = -1;
				break;
			}

			delay(2);
			fflush(stdout);
			draw(); // Redraw the screen after updating bullet position
		}

	/* UP */
	} else if (bullet_up_x >= 0 && bullet_up_y >= 0) {
		while (bullet_up_x >= 0 && bullet_up_y >= 0) {
			// Check if the bullet's position is within the boundaries and if there's a barrier to hit
			if (barriers[bullet_up_y * (LENGTH_OF_MAP * 2) + bullet_up_x] == 1) {
				barriers[bullet_up_y * (LENGTH_OF_MAP * 2) + bullet_up_x] = 0; // Remove the barrier
				bullet_up_y = -1; // Reset the bullet's position
				bullet_up_x = -1;
				break; // Exit the loop when a barrier is hit
			}

			// Move the bullet
			bullet_up_y--;

			// Exit the loop when the bullet goes out of bounds
			if (bullet_up_y < 0) {
				bullet_up_y = -1; // Reset the bullet's position
				bullet_up_x = -1;
				break;
			}

			delay(5);
			fflush(stdout);
			draw(); // Redraw the screen after updating bullet position
		}

	/* DOWN */
	} else if (bullet_down_x >= 0 && bullet_down_y >= 0) {
        while (bullet_down_x >= 0 && bullet_down_y >= 0) {
            // Check if the bullet's position is within the boundaries and if there's a barrier to hit
            if (barriers[bullet_down_y * (LENGTH_OF_MAP * 2) + bullet_down_x] == 1) {
                barriers[bullet_down_y * (LENGTH_OF_MAP * 2) + bullet_down_x] = 0; // Remove the barrier
                bullet_down_y = -1; // Reset the bullet's position
                bullet_down_x = -1;
                break; // Exit the loop when a barrier is hit
            }

            // Move the bullet
            bullet_down_y++;  // Move the bullet downward

            // Exit the loop when the bullet goes out of bounds
            if (bullet_down_y >= LENGTH_OF_MAP) { // Adjust the condition for the lower boundary
                bullet_down_y = -1; // Reset the bullet's position
                bullet_down_x = -1;
                break;
            }

            delay(5);
			fflush(stdout);
            draw(); // Redraw the screen after updating bullet position
        }
    }
}

void choose_level()
{
	if (difficulty + 10 == 12)
		difficulty++;

	if (difficulty + 10 >= 1 && difficulty + 10 <= 8) {
		default_song = "2020-03-22_-_8_Bit_Surf_-_FesliyanStudios.com_-_David_Renda";
		secret_level = 0;
	} else if (difficulty + 10 == 115) {
		default_song = "115";
		secret_level = 1;
	} else if (difficulty + 10 == 911) {
		default_song = "i_shoot";
		secret_level = 1;
	} else if ((difficulty + 10 > 9 || difficulty <= -10) && (difficulty + 10 != 911 || difficulty + 10 != 115)) {
		default_song = "song-of-storms";
		secret_level = 1;
	}

	playSong(default_song);
}

void logic()
{
	if (flag != 0) {
		switch (flag) {
			case 1:
				snake_x--;
				playSound("sounds/walking2.mp3", 100);
				break;
			case 2:
				snake_y++;
				playSound("sounds/walking2.mp3", 100);
				break;
			case 3:
				snake_x++;
				playSound("sounds/walking2.mp3", 100);
				break;
			case 4:
				snake_y--;
				playSound("sounds/walking2.mp3", 100);
				break;
			case 5:
				// left
                bullet_left_x = snake_x - 1;
                bullet_left_y = snake_y;
				playSound("sounds/pew.mp3", 100);
                break;
			case 6:
				// up
				bullet_up_x = snake_x;
				bullet_up_y = snake_y - 1;
				playSound("sounds/pew.mp3", 100);
				break;
			case 7:
				// right
				bullet_right_x = snake_x + 1;
				bullet_right_y = snake_y;
				playSound("sounds/pew.mp3", 100);
				break;
			case 8:
				// down
				bullet_down_x = snake_x;
				bullet_down_y = snake_y + 1;
				playSound("sounds/pew.mp3", 100);
				break;
			case 10:
				// change song backward
				song_number--;
				change_back_song = 0;
				playlist();
				break;
			case 9:
				// change back song
				change_back_song = 3;
				song_number = -1;
				playSong(default_song);
				break;
			case 11:
				// change song forward
				song_number++;
				change_back_song = 0;
				playlist();
				break;
			default:
				break;
		}
		
		fflush(stdout);
		flag = 0;
	}

	handle_bullets();

	if (snake_x <= 0 || snake_x >= LENGTH_OF_MAP * 4 - 1
		|| snake_y <= 0 || snake_y >= LENGTH_OF_MAP - 1
		|| barriers[snake_y * (LENGTH_OF_MAP * 2) + snake_x]) {

		if (snake_x == 0 && (snake_y == LENGTH_OF_MAP / 2
		|| snake_y == LENGTH_OF_MAP / 2 + 1 || snake_y == LENGTH_OF_MAP / 2 - 1)) {
			snake_x = LENGTH_OF_MAP * 4 - 2;
			system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
		}
		else if (snake_x == LENGTH_OF_MAP * 4 - 1
				&& (snake_y == LENGTH_OF_MAP / 2 || snake_y == LENGTH_OF_MAP / 2 + 1
					|| snake_y == LENGTH_OF_MAP / 2 - 1)) {
						system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
						snake_x = 1;
					}

		else if (snake_y == 0
					&& (snake_x == LENGTH_OF_MAP * 2 || snake_x == LENGTH_OF_MAP * 2 + 1
						|| snake_x == LENGTH_OF_MAP * 2 - 1
						|| snake_x == LENGTH_OF_MAP * 2 + 2
						|| snake_x == LENGTH_OF_MAP * 2 - 2
						|| snake_x == LENGTH_OF_MAP * 2 + 3
						|| snake_x == LENGTH_OF_MAP * 2 - 3)) {
							system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
							snake_y = LENGTH_OF_MAP - 2;
						}

		else if (snake_y == LENGTH_OF_MAP - 1
					&& (snake_x == LENGTH_OF_MAP * 2 || snake_x == LENGTH_OF_MAP * 2 + 1
					|| snake_x == LENGTH_OF_MAP * 2 - 1 || snake_x == LENGTH_OF_MAP * 2 + 2
					|| snake_x == LENGTH_OF_MAP * 2 - 2 || snake_x == LENGTH_OF_MAP * 2 + 3
					|| snake_x == LENGTH_OF_MAP * 2 - 3)) {
						system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
						snake_y = 1;
					}
		else {
			system("mplayer sounds/moody-blip-43107.mp3 > /dev/null 2>&1 &");
			lives--;
			snake_x = LENGTH_OF_MAP * 2;
			snake_y = LENGTH_OF_MAP / 2;
		}
	}

	if (lives <= 0) {
		gameover = 1;
		draw();
		playSound("sounds/8-bit-video-game-fail-version-2-145478.mp3", 100);
		printf("\033[1;31mGAME OVER!\033[0m\n");
		sleep(2);
		system("pkill mplayer");
	}

	if (snake_x == fruitx && snake_y == fruity) {
	label3:
		fruitx = rand() % 20;
		if (fruitx == 0)
			goto label3;

	label4:
		fruity = rand() % 20;
		if (fruity == 0)
			goto label4;
		points += 10;

		if (points % MAX_LEVEL_POINTS == 0) {
			difficulty++;
			level++;
			printf("\033[1;31mNew level unlocked!\033[0m\n");
			playSound("sounds/8-bit-powerup-6768.mp3", 100);

			sleep(2);
			playSound("sounds/yay.mp3", 100);
			sleep(4);

			if (difficulty == 0 || difficulty == -1 || difficulty == 1)
				difficulty += 5;

			choose_level();

			if (difficulty + 10 > 10 || difficulty + 10 < 1)
				secret_level = 1;
			setup(); // Restart the game
			return;
		}

		if (points > highscore) {
			FILE *fp = fopen("highscore.txt", "w");
			highscore = points;
			fprintf(fp, "%d", highscore);
			fclose(fp);
		}

		playSound("sounds/8-bit-powerup-6768.mp3", 100);
	}
}

void print_loading_animation(int num_cycles) {
    const char* loading = "Loading";
    const char* dots[] = {".", "..", "..."};

    for (int cycle = 0; cycle < num_cycles; cycle++) {
        for (unsigned int dot_index = 0; dot_index < sizeof(dots) / sizeof(dots[0]); dot_index++) {
            printf("%s%s", loading, dots[dot_index]);
            fflush(stdout);
            usleep(500000);

            for (size_t i = 0; i < strlen(loading) + strlen(dots[dot_index]); i++) {
                printf("\b \b");
                usleep(50000);
            }
        }
    }

    printf("\n");
}

void title_screen_display()
{
	system("clear");
	playSong("8bit-music-for-game-68698");

	const char *title =
        "\033[1;33m  ______             __        __        __    __                       __     \n"
        " /      \\           |  \\      |  \\      |  \\  |  \\                     |  \\    \n"
        "|  $$$$$$\\  ______  | $$  ____| $$      | $$  | $$ __    __  _______  _| $$_   \n"
        "| $$ __\\$$ /      \\ | $$ /      $$      | $$__| $$|  \\  |  \\|       \\|   $$ \\  \n"
        "| $$|    \\|  $$$$$$\\| $$|  $$$$$$$      | $$    $$| $$  | $$| $$$$$$$\\$$$$$$  \n"
        "| $$ \\$$$$| $$  | $$| $$| $$  | $$      | $$$$$$$$| $$  | $$| $$  | $$ | $$ __ \n"
        "| $$__| $$| $$__/ $$| $$| $$__| $$      | $$  | $$| $$__/ $$| $$  | $$ | $$|  \\\n"
        " \\$$    $$ \\$$    $$| $$ \\$$    $$      | $$  | $$ \\$$    $$| $$  | $$  \\$$  $$\n"
        "  \\$$$$$$   \\$$$$$$  \\$$  \\$$$$$$$       \\$$   \\$$  \\$$$$$$  \\$$   \\$$   \\$$$$ \n"
        "                                                                               \n"
        "                                                                               \n\033[0m";

    for (const char *ch = title; *ch != '\0'; ch++) {
        putchar(*ch);
        fflush(stdout); // Flush the output buffer to ensure the character is displayed immediately
        usleep(1500); // Delay in microseconds
    }

	printf("Choose the");
	printf("\033[1;32m level \033[0m");
	printf("(1 -> 8): ");
	scanf("%d", &difficulty);
	printf("\n");
	system("mplayer sounds/success.mp3 > /dev/null 2>&1");

	level = difficulty;

	difficulty -= 10;
	if (difficulty == 0 || difficulty == -1 || difficulty == 1)
		difficulty += 5;
}

int main(void)
{
restart:
	
	title_screen_display();

	display_pro_tips();

	print_loading_animation(2);

    fflush(stdout);

	setup();

	points = 0;

	choose_level();

	playSong(default_song);

	if (difficulty > 0 || difficulty <= -10)
		secret_level = 1;
	else
		secret_level = 0;

	while (!gameover) {
		draw();
		input();
		logic();

		if (gameover) {
			printf("Press r to restart or x to quit\n");
			fflush(stdout);

			if (kbhit()) {
				char choice = getchar();
				if (choice == 'r' || choice == 'R') {
					system("pkill mplayer");
					goto restart;
				}
			}
		}
	}

	system("pkill mplayer");
	free(barriers);
	system("clear");
	return 0;
}
