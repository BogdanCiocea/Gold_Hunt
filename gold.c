#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>

#define LENGTH_OF_MAP 28

const int length = LENGTH_OF_MAP;
int points, lives, fruitx, fruity, snake_x, snake_y;
int gameover, flag, difficulty, highscore;
int *barriers;

void setup()
{
    gameover = 0;
	lives = 3;
    barriers = (int*)malloc(length * length * 2 * sizeof(int));

    for (int i = 0; i < length * length * 2; i++)
        barriers[i] = rand();

    for (int i = 0; i < length * length * 2; i++) {
        if (barriers[i] % difficulty == 0)
            barriers[i] = 1;
        else
            barriers[i] = 0;
    }

    snake_x = length * 2;
    snake_y = length / 2;
    flag = 1;

    // Generate fruit coordinates
    fruitx = rand() % ((length - 1) * 4) + 1;
    fruity = rand() % length;

	
    // Check if the fruit collides with barriers
    while (barriers[fruity * (length * 2) + fruitx]) {
        fruitx = rand() % ((length - 1) * 4) + 1;
        fruity = rand() % length;
    }

    barriers[fruity * (length * 2) + fruitx] = 0; // Clear barrier at fruit position

    // Generate barriers and avoid fruit position
    for (int i = 0; i < length * length * 2; i++) {
        if (barriers[i] == 1) {
            barriers[i] = rand() % difficulty == 0 ? 1 : 0;
        }
    }

    FILE *fp = fopen("highscore.txt", "rw");
    fscanf(fp, "%d", &highscore);
    fclose(fp);
    points = 0;
}

void draw() {
	system("clear");
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < length * 4; j++) {

			int isBarrier = barriers[i * (length * 2) + j];

			if (i == 0 || i == length - 1 || j == 0 || j == length * 4 - 1) {
				if (i == length / 2 || i == length / 2 + 1 
					|| i == length / 2 - 1 || j == length * 2
					|| j == length * 2 + 1 || j == length * 2 - 1
					|| j == length * 2 + 2 || j == length * 2 - 2
					|| j == length * 2 + 3 || j == length * 2 - 3)
					// Portals woooooooooooooooooooo
					printf("\033[1;35m#\033[0m"); 
				else
					printf("#");
			} else if (i == fruity && j == fruitx) {
				printf("\033[1;33m$\033[0m");
			} else if (i == snake_y && j == snake_x) {
				printf("\033[1;32m@\033[0m");
			} else if (isBarrier) {
				printf("\033[1;31m^\033[0m");
			} else
				printf(" ");
		}
		printf("\n");
	}

	if (difficulty > 0 || difficulty < -10)
		printf("\033[1;31mSecret level\033[0m\n");	  
	printf("Take the gold and watch out for the mines!\n");
	printf("Points: %d\nLives: %d\n", points, lives);
	printf("Highscore: %d\n", highscore);
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
    //fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

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
			case 'a':
				flag = 1;
				break;
			case 's':
				flag = 2;
				break;
			case 'd':
				flag = 3;
				break;
			case 'w':
				flag = 4;
				break;
			case 'x':
				gameover = 1;
				break;
		}
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

void logic()
{
	sleep(0.01);
	if (flag != 0) {
		switch (flag) {
			case 1:
				snake_x--;
				break;
			case 2:
				snake_y++;
				break;
			case 3:
				snake_x++;
				break;
			case 4:
				snake_y--;
				break;
			default:
				break;
		}
		playSound("sounds/walking2.mp3", 70);
		fflush(stdout);
		flag = 0;
	}

	if (snake_x <= 0 || snake_x >= length * 4 - 1
		|| snake_y <= 0 || snake_y >= length - 1
		|| barriers[snake_y * (length * 2) + snake_x]) {

		if (snake_x == 0 && (snake_y == length / 2
		|| snake_y == length / 2 + 1 || snake_y == length / 2 - 1)) {
			snake_x = length * 4 - 2;
			system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
		}
		else if (snake_x == length * 4 - 1
				&& (snake_y == length / 2 || snake_y == length / 2 + 1
					|| snake_y == length / 2 - 1)) {
						system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
						snake_x = 1;
					}
		else if (snake_y == 0
					&& (snake_x == length * 2 || snake_x == length * 2 + 1
						|| snake_x == length * 2 - 1
						|| snake_x == length * 2 + 2
						|| snake_x == length * 2 - 2
						|| snake_x == length * 2 + 3
						|| snake_x == length * 2 - 3)) {
							system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
							snake_y = length - 2;
						}
		else if (snake_y == length - 1
					&& (snake_x == length * 2 || snake_x == length * 2 + 1
					|| snake_x == length * 2 - 1 || snake_x == length * 2 + 2
					|| snake_x == length * 2 - 2 || snake_x == length * 2 + 3
					|| snake_x == length * 2 - 3)) {
						system("mplayer sounds/portal.mp3 > /dev/null 2>&1 &");
						snake_y = 1;
					}
		else {
			system("mplayer sounds/moody-blip-43107.mp3 > /dev/null 2>&1 &");
			lives--;
			snake_x = length * 2;
			snake_y = length / 2;
		}
	}

	if (lives <= 0) {
		gameover = 1;
		draw();
		system("mplayer sounds/8-bit-video-game-fail-version-2-145478.mp3 > /dev/null 2>&1 &");
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

		if (points > highscore) {
			printf("\033[1;31Woah hey! New highscore. Nice job!\033[0m\n");
			FILE *fp = fopen("highscore.txt", "w");
			highscore = points;
			fprintf(fp, "%d", highscore);
			fclose(fp);
		}

		system("mplayer sounds/8-bit-powerup-6768.mp3 > /dev/null 2>&1 &");
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
                //fflush(stdout);
                usleep(50000);
            }
        }
    }
    printf("\n");
}

void choose_level()
{
	if (difficulty + 10 == 115) {
		system("pkill mplayer");
		system("mplayer -loop 9999 sounds/115.mp3 > /dev/null 2>&1 &");
		goto skip;
	}

	if (difficulty > 0 || difficulty < -10) {
		system("pkill mplayer");
		system("mplayer -loop 9999 sounds/song-of-storms.mp3 > /dev/null 2>&1 &");
	}

	skip:
}

void title_screen_display()
{
	system("clear");
	system("mplayer -loop 9999 sounds/8bit-music-for-game-68698.mp3 > /dev/null 2>&1 &");
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
        "                                                                               \n"
        "                                                                               \n\033[0m";

    for (const char *ch = title; *ch != '\0'; ch++) {
        putchar(*ch);
        fflush(stdout); // Flush the output buffer to ensure the character is displayed immediately
        usleep(1500); // Delay in microseconds
    }

	printf("Choose the");
	printf("\033[1;32m level \033[0m");
	printf("(1 -> 10): ");
	scanf("%d", &difficulty);
	system("mplayer sounds/success.mp3 > /dev/null 2>&1");

	difficulty -= 10;
	if (difficulty == 0 || difficulty == -1)
		difficulty = -2;
}

int main(void)
{

restart:
	
	title_screen_display();

	print_loading_animation(1);

    fflush(stdout);

	//sleep(4);

	setup();
	system("pkill mplayer");
	system("mplayer -loop 9999 sounds/2020-03-22_-_8_Bit_Surf_-_FesliyanStudios.com_-_David_Renda.mp3 > /dev/null 2>&1 &");

	choose_level();
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
