#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#endif

#define CCHAR 0
#ifdef CTIME
#undef CTIME
#endif
#define CTIME 1

#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define ROTATE 3

#define I_BLOCK 0
#define T_BLOCK 1
#define S_BLOCK 2
#define Z_BLOCK 3
#define L_BLOCK 4
#define J_BLOCK 5
#define O_BLOCK 6

#define GAME_START 0
#define GAME_END 1

#define MAX_RESULT 100

char i_block[4][4][4] = {
    {{1,1,1,1},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1}},
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,1,1,1}},
    {{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0}}
};

char t_block[4][4][4] = {
    {{1,0,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}},
    {{1,1,1,0},{0,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,1,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}}
};

char s_block[4][4][4] = {
    {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}},
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}}
};

char z_block[4][4][4] = {
    {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
    {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}}
};

char l_block[4][4][4] = {
    {{1,0,0,0},{1,0,0,0},{1,1,0,0},{0,0,0,0}},
    {{1,1,1,0},{1,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,1,0},{0,0,1,0},{0,0,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}}
};

char j_block[4][4][4] = {
    {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}},
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,0,0,0},{1,0,0,0},{0,0,0,0}},
    {{0,0,0,0},{0,0,0,0},{1,1,1,0},{0,0,1,0}}
};

char o_block[4][4][4] = {
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}
};

char tetris_table[21][10];

static struct result {
    char name[30];
    long point;
    int year, month, day, hour, min, rank;
} temp_result;

int block_number = 0;
int next_block_number = 0;
int block_state = 0;
int x = 3, y = 0;
int game = GAME_END;
int best_point = 0;
long point = 0;
struct result result_list[MAX_RESULT];
int result_count = 0;

int display_menu(void);
int game_start();
void search_result();
void print_result();
void reset_game();
char get_key();
void display_game();
char (*get_block_array(int n))[4][4];
void process_key(char key);
int check_reach();
void fix_block();
int can_move(int dx, int dy);
void delete_blocks();
void spawn_block();
int game_over();
void save_score();
int preview();

char (*get_block_array(int n))[4][4] {
    switch (n) {
        case I_BLOCK: return i_block;
        case T_BLOCK: return t_block;
        case S_BLOCK: return s_block;
        case Z_BLOCK: return z_block;
        case L_BLOCK: return l_block;
        case J_BLOCK: return j_block;
        case O_BLOCK: return o_block;
        default: return i_block;
    }
}

void reset_game() {
    memset(tetris_table, 0, sizeof(tetris_table));
    point = 0;
    block_number = rand() % 7;
    next_block_number = rand() % 7;
    x = 3;
    y = 0;
    block_state = 0;
}

int preview() {
    int preview_y = y;
    while (can_move(0, preview_y - y + 1)) {
        preview_y++;
    }
    return preview_y;
}

int can_move(int dx, int dy) {
    char (*block)[4][4] = get_block_array(block_number);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[block_state][i][j]) {
                int ny = y + i + dy;
                int nx = x + j + dx;
                if (ny >= 19 || nx < 1 || nx >= 9) return 0;
                if (tetris_table[ny][nx]) return 0;
            }
        }
    }
    return 1;
}

char get_key() {
#ifdef _WIN32
    if (_kbhit()) return _getch();
    return 0;
#else
    struct termios oldt, newt;
    struct timeval tv;
    fd_set read_fds;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &tv) > 0) {
        char ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
#endif
}

void display_game() {
    printf("점수: %ld\n\n", point);

    printf("🔳");
    for (int i = 0; i < 8; i++) printf("🔳");
    printf("🔳\n");

    int preview_y = preview();
    char (*block)[4][4] = get_block_array(block_number);

    for (int y_pos = 0; y_pos < 19; y_pos++) {
        printf("🔳");

        for (int x_pos = 1; x_pos <= 8; x_pos++) {
            int is_block = 0;
            int is_preview = 0;

            for (int i = 0; i < 4 && !is_preview; i++) {
                for (int j = 0; j < 4; j++) {
                    if (block[block_state][i][j] && preview_y + i == y_pos && x + j == x_pos) {
                        is_preview = 1;
                    }
                }
            }

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (y + i == y_pos && x + j == x_pos && block[block_state][i][j]) {
                        is_block = 1;
                    }
                }
            }

            if (is_block) printf("🟨");
            else if (is_preview) printf("⬜️");
            else if (tetris_table[y_pos][x_pos]) printf("🟥");
            else printf("  ");
        }
        printf("🔳\n");
    }

    printf("🔳");
    for (int i = 0; i < 8; i++) printf("🔳");
    printf("🔳\n");

    printf("\n다음 블럭\n");
    char (*next_block)[4][4] = get_block_array(next_block_number);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (next_block[0][i][j]) printf("🟨");
            else printf("  ");
        }
        printf("\n");
    }
}

void fix_block() {
    char (*block)[4][4] = get_block_array(block_number);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[block_state][i][j]) {
                int ny = y + i;
                int nx = x + j;
                if (ny >= 0 && ny < 20 && nx >= 0 && nx < 10) {
                    tetris_table[ny][nx] = 1;
                }
            }
        }
    }
}

void delete_blocks() {
    for (int i = 18; i >= 0; i--) {
        int full = 1;
        for (int j = 1; j <= 8; j++) {
            if (tetris_table[i][j] == 0) {
                full = 0;
                break;
            }
        }
        if (full) {
            for (int k = i; k > 0; k--) {
                for (int j = 1; j <= 8; j++) {
                    tetris_table[k][j] = tetris_table[k - 1][j];
                }
            }
            for (int j = 1; j <= 8; j++) tetris_table[0][j] = 0;
            i++;
            point += 100;
        }
    }
}

void spawn_block() {
    block_number = next_block_number;
    next_block_number = rand() % 7;
    x = 3; y = 0;
    block_state = 0;
    if (game_over()) game = GAME_END;
}

int game_over() {
    char (*block)[4][4] = get_block_array(block_number);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[block_state][i][j]) {
                int ny = y + i;
                int nx = x + j;
                if (tetris_table[ny][nx]) return 1;
            }
        }
    }
    return 0;
}

void process_key(char key) {
    switch (key) {
        case 'j': if (can_move(-1, 0)) x--; break;
        case 'l': if (can_move(1, 0)) x++; break;
        case 'k':
            if (can_move(0, 1)) y++;
            else { fix_block(); delete_blocks(); spawn_block(); }
            break;
        case 'i': {
            int next_state = (block_state + 1) % 4;
            char (*block)[4][4] = get_block_array(block_number);
            int can_rotate = 1;
            for (int i = 0; i < 4 && can_rotate; i++) {
                for (int j = 0; j < 4; j++) {
                    if (block[next_state][i][j]) {
                        int ny = y + i;
                        int nx = x + j;
                        if (ny >= 20 || nx < 1 || nx >= 9 || tetris_table[ny][nx]) {
                            can_rotate = 0;
                            break;
                        }
                    }
                }
            }
            if (can_rotate) block_state = next_state;
            break;
        }
        case 'a':
            while (can_move(0, 1)) y++;
            fix_block(); delete_blocks(); spawn_block();
            break;
        case 'p':
            game = GAME_END;
            break;
    }
}

void save_score() {
    if (result_count >= MAX_RESULT) return;
    printf("이름을 입력하세요: ");
    scanf("%s", result_list[result_count].name);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    result_list[result_count].point = point;
    result_list[result_count].year = t->tm_year + 1900;
    result_list[result_count].month = t->tm_mon + 1;
    result_list[result_count].day = t->tm_mday;
    result_list[result_count].hour = t->tm_hour;
    result_list[result_count].min = t->tm_min;
    result_list[result_count].rank = 0;
    result_count++;

    for (int i = 0; i < result_count - 1; i++) {
        for (int j = i + 1; j < result_count; j++) {
            if (result_list[i].point < result_list[j].point) {
                struct result tmp = result_list[i];
                result_list[i] = result_list[j];
                result_list[j] = tmp;
            }
        }
    }
    for (int i = 0; i < result_count; i++) {
        result_list[i].rank = i + 1;
    }
}

void search_result() {
    char name[30];
    int found = 0;
    while (getchar() != '\n');
    printf("기록 검색을 원하는 사람의 이름을 입력하세요: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    for (int i = 0; i < result_count; i++) {
        if (strcmp(result_list[i].name, name) == 0) {
            printf("%s님의 기록: %ld점 %d-%d-%d %d:%d\n", 
                name, result_list[i].point,
                result_list[i].year, result_list[i].month, result_list[i].day,
                result_list[i].hour, result_list[i].min);
            found = 1;
        }
    }
    if (!found) printf("해당 사람의 기록이 없습니다.\n");
    printf("\n아무 키나 누르면 메뉴로 돌아갑니다.\n");
    getchar();
}

void print_result() {
    printf("\n===== 기록 확인 =====\n");
    for (int i = 0; i < result_count; i++) {
        printf("%d등: %s -> %ld점 %d-%d-%d %d:%d\n", 
            i+1, result_list[i].name, result_list[i].point, 
            result_list[i].year, result_list[i].month, result_list[i].day, 
            result_list[i].hour, result_list[i].min);
    }
    printf("\n아무 키나 누르면 메뉴로 돌아갑니다.\n");
    getchar(); getchar();
}

int game_start() {
    reset_game();
    char key;
    struct timeval last_drop, current;
    gettimeofday(&last_drop, NULL);

    while (game == GAME_START) {
#ifdef _WIN32
        system("cls");
#else
        printf("\033[H\033[J");
#endif
        gettimeofday(&current, NULL);
        long elapsed = (current.tv_sec - last_drop.tv_sec) * 1000000L +
                       (current.tv_usec - last_drop.tv_usec);

        if (elapsed > 600000) {
            if (can_move(0, 1)) y++;
            else {
                fix_block();
                delete_blocks();
                spawn_block();
            }
            gettimeofday(&last_drop, NULL);
        }

        key = get_key();
        if (key) process_key(key);

        if (game == GAME_END) {
            printf("게임 오버! 1초 뒤 메뉴로...\n");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
            #ifndef _WIN32
            tcflush(STDIN_FILENO, TCIFLUSH);
            #endif
#endif
            break;
        }

        display_game();
#ifdef _WIN32
        Sleep(60);
#else
        usleep(60000);
#endif
    }

    save_score();
    return 1;
}

int display_menu(void) {
    int menu = 0;
    while (1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("\n\n\t\t\t\tText Tetris");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t\tMAIN MENU\t\n");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t   1) Start Game");
        printf("\n\t\t\t   2) Search Records");
        printf("\n\t\t\t   3) View Leaderboard");
        printf("\n\t\t\t   4) EXIT");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t\t\t SELECT : ");
        scanf("%d", &menu);
        if (menu >= 1 && menu <= 4) return menu;
    }
}

int main(void) {
    srand(time(NULL) + getpid());
    int menu = 1;

    while (menu) {
        menu = display_menu();

        if (menu == 1) {
            game = GAME_START;
            menu = game_start();
        }
        else if (menu == 2) search_result();
        else if (menu == 3) print_result();
        else if (menu == 4) exit(0);
    }
    return 0;
}
