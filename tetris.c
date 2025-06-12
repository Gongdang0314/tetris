#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 플랫폼별 헤더 파일 포함
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <process.h>
#define SLEEP(x) Sleep(x)
#define CLEAR_SCREEN() system("cls")
#define FLUSH_INPUT() while(_kbhit()) _getch()
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#define SLEEP(x) usleep((x)*1000)
#define CLEAR_SCREEN() system("clear")
#define FLUSH_INPUT() tcflush(STDIN_FILENO, TCIFLUSH)
#endif

/*
윈도우로 실행하실 때는 mingw32-make후 mingw32-make run 하시면 됩니다!!!
*/

/* 타이머 -> 미사용 */
/*
#define CCHAR 0
#ifdef CTIME
#undef CTIME
#endif
#define CTIME 1
*/

/* 왼쪽, 오른쪽, 아래, 회전, 드롭, 종료  */
#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define ROTATE 3
#define DROP 4
#define QUIT 5

/* 블록 모양 */
#define I_BLOCK 0
#define	T_BLOCK 1
#define S_BLOCK 2
#define Z_BLOCK 3
#define L_BLOCK 4
#define J_BLOCK 5
#define O_BLOCK 6

/* 게임 시작, 게임 종료 */
#define GAME_START 0
#define GAME_END 1

/* 점수 저장 배열 */
#define MAX_RESULT 100

/* 블록 정의 */
char i_block[4][4][4] = {
    {  // 첫 번째 회전 상태
        {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
    {  // 두 번째 회전 상태
        {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1} },
    {  // 세 번째 회전 상태
        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1} },
    {  // 네 번째 회전 상태
        {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0} }
};

char t_block[4][4][4] = {
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}
};

char s_block[4][4][4] = {
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}
};

char z_block[4][4][4] = {
    {{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}
};

char l_block[4][4][4] = {
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}
};

char j_block[4][4][4] = {
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}}
};

char o_block[4][4][4] = {
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
};

/* 테트리스 판 */
char tetris_table[21][10];

/* 결과 저장 구조체 */
struct result {
    char name[30];
    long point;
    int year;
    int month;
    int day;
    int hour;
    int min;
    int rank;
}; // temp_result;

/* 전역 변수 */
int block_number = 0;
int next_block_number = 0;
int block_state = 0;
int x = 3, y = 0;
int game = GAME_END;
int best_point = 0;
long point = 0;
struct result result_list[MAX_RESULT];
int result_count = 0;

/* 함수 선언 */
int display_menu(void);
int game_start();
void search_result();
void print_result();
void reset_game();
char get_key();
void display_game();
char (*block_pattern(int n))[4][4];
void handle_key(char key);
int check_reach();
void fix_block();
int is_move(int dx, int dy);
void delete_blocks();
void generate_block();
int game_over();
void save_score();
int preview();
void init_console();
void restore_console();
long get_time_ms();
int map_key(char key);
void move_cursor_top();
void hide_cursor();

#ifndef _WIN32
struct termios orig_termios; // 원래 터미널 설정 저장
int terminal_initialized = 0;  // 터미널 초기화 여부
#endif

/* 시간 반환 함수 */
long get_time_ms() {
#ifdef _WIN32
    return (long)GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L) + (tv.tv_usec / 1000L);
#endif
}

/* 콘솔 초기화 */
void init_console() {
#ifndef _WIN32
    if (!terminal_initialized) {
        tcgetattr(STDIN_FILENO, &orig_termios); // 현재 터미널 설정 저장
        struct termios new_termios = orig_termios;
        new_termios.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        terminal_initialized = 1;
    }
#endif
}

/* 콘솔 되돌리기 */
void restore_console() {
#ifndef _WIN32
    if (terminal_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        terminal_initialized = 0;
    }
#endif
}

/* 커서 이동 */
#ifdef _WIN32
void move_cursor_top() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(h, coord);
}
#endif

/* 커서 숨기기 */
#ifdef _WIN32
void hide_cursor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize = 1;
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(h, &ci);
}
#endif

/* 키 입력 받기 */
char get_key() {
#ifdef _WIN32
    if (_kbhit()) {  // 키 눌렸는지 확인하고 반환값 지정
        return _getch();
    }
    return 0;
#else
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout) > 0) {
        return getchar();
    }
    return 0;
#endif
}

/* 메뉴 표시 */
int display_menu(void) {
    int menu = 0;

    while (1) {
        CLEAR_SCREEN();
        printf("\n\n\t\t\t\tText Tetris");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t\tGAME MENU\t\n");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t   1) Game Start");
        printf("\n\t\t\t   2) Search history");
        printf("\n\t\t\t   3) Record Output");
        printf("\n\t\t\t   4) QUIT");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t\t\t SELECT : ");

        if (scanf("%d", &menu) != 1) {
            while (getchar() != '\n'); // 입력 버퍼 비우기
            continue;
        }

        if (menu < 1 || menu > 4) {
            continue;
        }
        else {
            return menu;
        }
    }
    return 0;
}

/* 움직임 가능 여부 체크 */
int is_move(int move_x, int move_y) {
    char (*block)[4][4] = block_pattern(block_number);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[block_state][i][j]) {
                int new_y = y + i + move_y;  // 새로운 Y좌표
                int new_x = x + j + move_x;  // 새로운 X좌표
                // 경계를 벗어나거나 이미 블록이 있는 위치인지 확인인
                if (new_y >= 19 || new_x < 1 || new_x >= 9) return 0;
                if (tetris_table[new_y][new_x]) return 0;
            }
        }
    }
    return 1;
}

/* 게임 초기화 */
void reset_game() {
    memset(tetris_table, 0, sizeof(tetris_table)); // 게임판 초기화
    point = 0; // 점수 초기화
    block_number = rand() % 7; // 현재 블록 랜덤 생성
    next_block_number = rand() % 7; // 다음 블록 랜덤 생성
    x = 3; // 블록 시작 x위치
    y = 0; // 블록 시작 Y위치
    block_state = 0; // 블록 회전 상태 초기화
}

/* 미리보기 위치 계산 */
int preview() {
    int preview_y = y;
    while (is_move(0, preview_y - y + 1)) {
        preview_y++;
    }
    return preview_y;
}

/* 블록 배열 반환 */
char (*block_pattern(int n))[4][4]{
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

/* 게임 화면 출력 */
void display_game() {
    printf("Score: %ld\n\n", point); // 현재 점수 출력

#ifdef _WIN32
    const char* WALL = "▣ ";
    const char* BLOCK = "▩ ";
    const char* PREVIEW = "▤ ";
    const char* FIXED = "▩ ";
    const char* EMPTY = "  ";
#else
    const char* WALL = "🔳";
    const char* BLOCK = "🟨";
    const char* PREVIEW = "⬜️";
    const char* FIXED = "🟥";
    const char* EMPTY = "  ";
#endif

    // 상단 테두리 출력
    for (int i = 0; i < 10; i++) printf("%s", WALL);
    printf("\n");

    int preview_y = preview();
    char (*block)[4][4] = block_pattern(block_number);

    // 맵 출력
    for (int y_pos = 0; y_pos < 19; y_pos++) {
        printf("%s", WALL); // 왼쪽 벽 출력

        for (int x_pos = 1; x_pos <= 8; x_pos++) {
            int is_block = 0; // 현재 블록 위치인지
            int is_preview = 0; // 미리보기 위치인지

            // 블럭 미리보기 위치 확인
            for (int i = 0; i < 4 && !is_preview; i++) {
                for (int j = 0; j < 4; j++) {
                    if (block[block_state][i][j] &&
                        preview_y + i == y_pos &&
                        x + j == x_pos) {
                        is_preview = 1;
                    }
                }
            }

            // 현재 떨어지는 블럭 위치인지 확인
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (y + i == y_pos && x + j == x_pos) {
                        if (block[block_state][i][j]) {
                            is_block = 1;
                        }
                    }
                }
            }

            if (is_block)
                printf("%s", BLOCK);      // 현재 블럭
            else if (is_preview)
                printf("%s", PREVIEW);    // 미리보기
            else if (tetris_table[y_pos][x_pos])
                printf("%s", FIXED);      // 고정된 블럭
            else
                printf("%s", EMPTY);      // 빈 공간
        }

        printf("%s\n", WALL); // 오른쪽 벽 출력
    }

    // 하단 테두리 출력
    for (int i = 0; i < 10; i++) printf("%s", WALL);
    printf("\n");

    // 다음 블럭 출력
    printf("\nNext Block:\n");
    char(*next_block)[4][4] = block_pattern(next_block_number);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (next_block[0][i][j]) {
                printf("%s", BLOCK);
            } else {
                printf("%s", EMPTY);
            }
        }
        printf("\n");
    }
}


/* 기록 검색 */
void search_result() {
    char name[30];
    int found = 0;

    while (getchar() != '\n'); // 입력 버퍼 비우기

    printf("Enter name to search: ");
    if (fgets(name, sizeof(name), stdin) != NULL) {
        name[strcspn(name, "\n")] = '\0'; // 개행문자 제거

        // 저장된 기록에서 이름으로 검색
        for (int i = 0; i < result_count; i++) {
            if (strcmp(result_list[i].name, name) == 0) {
                printf("%s's record: %ld point - %d-%d-%d %d:%d\n",
                    name, result_list[i].point, result_list[i].year,
                    result_list[i].month, result_list[i].day,
                    result_list[i].hour, result_list[i].min);
                found = 1;
            }
        }
        if (!found) {
            printf("No record found for this person.\n");
        }
    }
    printf("\nPress any key to return to menu.\n");
    (void)getchar();
}

/* 기록 출력 */
void print_result() {
    printf("\n===== Ranked Scoreboard =====\n");

    for (int i = 0; i < result_count; i++) {
        printf("%d. %s -> %ld points - %d-%d-%d %d:%d\n",
            i + 1, result_list[i].name, result_list[i].point,
            result_list[i].year, result_list[i].month, result_list[i].day,
            result_list[i].hour, result_list[i].min);
    }
    printf("\nPress any key to return to menu.\n");
    (void)getchar();
    (void)getchar();
}

/* 키 맵핑 */
int map_key(char key) {
    switch (key) {
        case 'j': return LEFT;
        case 'l': return RIGHT;
        case 'k': return DOWN;
        case 'i': return ROTATE;
        case 'a': return DROP;
        case 'p': return QUIT;
        default: return -1;
    }
}

/* 입력 받은 키 다루기 */
void handle_key(char key) {
    int dir = map_key(key);
    switch (dir) {
        case LEFT:
            if (is_move(-1, 0)) x--;
            break;
        case RIGHT:
            if (is_move(1, 0)) x++;
            break;
        case DOWN:
            if (is_move(0, 1)) {
                y++;
            } else {
                fix_block();
                delete_blocks();
                generate_block();
            }
            break;
        case ROTATE: {
            int next_state = (block_state + 1) % 4;
            char (*block)[4][4] = block_pattern(block_number);
            int can_rotate = 1;

            // 회전 가능한지 확인
            for (int i = 0; i < 4 && can_rotate; i++) {
                for (int j = 0; j < 4; j++) {
                    if (block[next_state][i][j]) {
                        int new_y = y + i;
                        int new_x = x + j;
                        if (new_y >= 20 || new_x < 1 || new_x >= 9 || tetris_table[new_y][new_x]) {
                            can_rotate = 0;
                            break;
                        }
                    }
                }
            }
            if (can_rotate) block_state = next_state;
            break;
        }
        case DROP:
            while(is_move(0, 1)){
                y++;
            }
            fix_block();
            delete_blocks();
            generate_block();
            break;
        case QUIT:
            game = GAME_END;
            break;
        default:
            break;
    }
}

/* 벽이나 블록에 닿았는지 체크 */
int check_reach() {
    return !is_move(0, 1);
}

/* 블록 고정 */
void fix_block() {
    char (*block)[4][4] = block_pattern(block_number);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[block_state][i][j]) {
                int new_y = y + i;
                int new_x = x + j;

                // 유효한 범위 내에서 블록 고정
                if (new_y >= 0 && new_y < 20 && new_x >= 0 && new_x < 10) {
                    tetris_table[new_y][new_x] = 1;
                }
            }
        }
    }
}

/* 블록 삭제 */
void delete_blocks() {
    for (int i = 18; i >= 0; i--) {
        int full = 1;  // 해당 줄이 가득 찼는지 확인
        for (int j = 1; j <= 8; j++) {
            if (tetris_table[i][j] == 0) {
                full = 0;
                break;
            }
        }
        if (full) {
            // 한 줄 삭제 후 위의 줄들을 한 줄씩 아래로 복사사
            for (int k = i; k > 0; k--) {
                for (int j = 1; j <= 8; j++) {
                    tetris_table[k][j] = tetris_table[k - 1][j];
                }
            }
            // 최상단 줄을 비우기
            for (int j = 1; j <= 8; j++) {
                tetris_table[0][j] = 0;
            }

            i++;
            point += 100;
        }
    }
}

/* 블록 생성 */
void generate_block() {
    block_number = next_block_number;
    next_block_number = rand() % 7;
    x = 3;
    y = 0;
    block_state = 0;

    if (game_over()) {
        game = GAME_END;
    }
}

/* 게임 오버 체크 */
int game_over() {
    char (*block)[4][4] = block_pattern(block_number);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[block_state][i][j]) {
                int new_y = y + i;
                int new_x = x + j;
                if (tetris_table[new_y][new_x]) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* 점수 저장 */
void save_score() {
    CLEAR_SCREEN();
    if (result_count >= MAX_RESULT) {
        printf("Maximum capacity exceeded");
        return;
    }

    printf("Enter your name: ");
    if (scanf("%29s", result_list[result_count].name) != 1) {
        strcpy(result_list[result_count].name, "Unknown");
    }

    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    result_list[result_count].point = point;
    result_list[result_count].year = t->tm_year + 1900;
    result_list[result_count].month = t->tm_mon + 1;
    result_list[result_count].day = t->tm_mday;
    result_list[result_count].hour = t->tm_hour;
    result_list[result_count].min = t->tm_min;
    result_list[result_count].rank = 0;

    result_count++;

    // 점수별 내림차순 정렬
    for (int i = 0; i < result_count - 1; i++) {
        for (int j = i + 1; j < result_count; j++) {
            if (result_list[i].point < result_list[j].point) {
                struct result tmp = result_list[i];
                result_list[i] = result_list[j];
                result_list[j] = tmp;
            }
        }
    }

    // 랭킹 설정
    for (int i = 0; i < result_count; i++) {
        result_list[i].rank = i + 1;
    }
}



/* 게임 시작 */
int game_start() {
    reset_game();
    init_console();

    #ifdef _WIN32
        hide_cursor();
    #endif

    char key;
    long last_drop = get_time_ms();

    while (game == GAME_START) {
        

        long current_time = get_time_ms();
        long elapsed = current_time - last_drop;

        // 블록 자동 하강
        if (elapsed > 500) {
            if (is_move(0, 1)) {
                y++;
            }
            else {
                fix_block();
                delete_blocks();
                generate_block();
            }
            last_drop = current_time;
        }

        key = get_key();
        if (key) {
            handle_key(key);
        }

        if (game == GAME_END) {
            restore_console();
            printf("Game Over! Returning to menu in 1 second!\n");
            SLEEP(1000);
            FLUSH_INPUT();
            break;
        }

        #ifdef _WIN32
            move_cursor_top();
        #else
            CLEAR_SCREEN();
        #endif
        display_game();
        SLEEP(30);
    }

    restore_console();
    save_score();
    return 1;
}

/* 메인 함수 */
int main(void) {
#ifdef _WIN32
    srand((unsigned int)(time(NULL) + _getpid()));
    SetConsoleOutputCP(CP_UTF8); // 콘솔 문자 인코딩 설정
    SetConsoleCP(CP_UTF8);
#else
    srand((unsigned int)(time(NULL) + getpid()));
#endif

    int menu = 1;

    while (menu) {
        menu = display_menu();

        if (menu == 1) {
            game = GAME_START;
            CLEAR_SCREEN();
            menu = game_start();
        }
        else if (menu == 2) {
            CLEAR_SCREEN();
            search_result();
        }
        else if (menu == 3) {
            CLEAR_SCREEN();
            print_result();
        }
        else if (menu == 4) {
            exit(0);
        }
    }

    return 0;
}
