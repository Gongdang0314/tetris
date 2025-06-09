#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Platform-specific includes and definitions
#ifdef _WIN32
#include <conio.h>  // For _kbhit and _getch
#include <windows.h> // For Sleep and console functions
#else
#include <unistd.h>     // For usleep
#include <sys/time.h>   // For gettimeofday
#include <termios.h>    // For terminal control
#include <sys/ioctl.h>  // For ioctl
#endif

// ... (Your existing constant definitions and block arrays) ...

/* 타이머  */
#define CCHAR 0
#ifdef CTIME
#undef CTIME
#endif
#define CTIME 1

/* 왼쪽, 오른쪽, 아래, 회전  */
#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define ROTATE 3


/* 블록 모양 */
#define I_BLOCK 0
#define T_BLOCK 1
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

char t_block[4][4][4] =
    {
            {{1, 0, 0, 0},   {1, 1, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
            {{1, 1, 1, 0},   {0, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{0, 0, 1, 0},   {0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
            {{0, 0, 0, 0},   {0, 1, 0, 0},   {1, 1, 1, 0},   {0, 0, 0, 0}}
    };


char s_block[4][4][4] =
    {
            {{1, 0, 0, 0},   {1, 1, 0, 0},   {0, 1, 0, 0},   {0, 0, 0, 0}},
            {{0, 1, 1, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{0, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
            {{0, 0, 0, 0},   {0, 1, 1, 0},   {1, 1, 0, 0},   {0, 0, 0, 0}}

    };

char z_block[4][4][4] =
    {
            {{0, 1, 0, 0},   {1, 1, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
            {{1, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{0, 0, 1, 0},   {0, 1, 1, 0},   {0, 1, 0, 0},   {0, 0, 0, 0}},
            {{0, 0, 0, 0},   {1, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 0, 0}}

    };

char l_block[4][4][4] =
    {
            {{1, 0, 0, 0},   {1, 0, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0}},
            {{1, 1, 1, 0},   {1, 0, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
            {{0, 0, 0, 0},   {0, 0, 1, 0},   {1, 1, 1, 0},   {0, 0, 0, 0}}

    };

char j_block[4][4][4] =
    {
            {{0, 1, 0, 0},   {0, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0}},
            {{1, 0, 0, 0},   {1, 1, 1, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{1, 1, 0, 0},   {1, 0, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
            {{0, 0, 0, 0},   {0, 0, 1, 0},   {1, 1, 1, 0},   {0, 0, 0, 0}}
    };


char o_block[4][4][4] =
    {
            {{1, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{1, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{1, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
            {{1, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}}

    };


char tetris_table[21][10];

static struct result
{
    char name[30];
    long point;
    int year;
    int month;
    int day;
    int hour;
    int min;
    int rank;
}temp_result;

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

// Function to clear the console screen
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to set terminal to raw mode (for Linux/macOS)
#ifndef _WIN32
struct termios original_termios;

void set_raw_mode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

// Function to restore terminal mode (for Linux/macOS)
void restore_termios() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}
#endif

/* 메뉴 표시*/
int display_menu(void)
{
    int menu = 0;

    while(1)
    {
        clear_screen();
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
        scanf("%d",&menu);
        if(menu < 1 || menu > 4)
        {
            continue;
        }
        else
        {
            return menu;
        }
    }
    return 0;
}


int can_move(int dx, int dy)
{
    char (*block)[4][4] = get_block_array(block_number);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(block[block_state][i][j]){
                int ny = y + i + dy;
                int nx = x + j + dx;
                if(ny >= 19 || nx < 1 || nx >= 9) return 0;
                if(tetris_table[ny][nx]) return 0;
            }
        }
    }
    return 1;
}

// 게임판 초기화
void reset_game()
{
    memset(tetris_table, 0, sizeof(tetris_table));
    point = 0;
    block_number = rand() % 7;
    next_block_number = rand() % 7;
    x = 3; y = 0;
    block_state = 0;
}

char get_key()
{
#ifdef _WIN32
    if (_kbhit()) {
        return _getch();
    }
    return 0;
#else
    struct timeval tv;
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0; // Check immediately

    if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &tv) > 0) {
        return getchar(); // Read the character
    }
    return 0;
#endif
}

// 블럭 번호에 따라 해당 블럭 배열을 반환하는 함수
char (*get_block_array(int n))[4][4] {
    switch (n) {
        case I_BLOCK: return i_block;
        case T_BLOCK: return t_block;
        case S_BLOCK: return s_block;
        case Z_BLOCK: return z_block;
        case L_BLOCK: return l_block;
        case J_BLOCK: return j_block;
        case O_BLOCK: return o_block;
        default: return i_block; // 기본값
    }
}

void display_game()
{
    // Use ANSI escape codes for clearing and cursor positioning
    printf("\033[H\033[J"); // Clear screen and move cursor to home

    printf("점수: %ld\n\n", point);

    // 상단 테두리
    printf("🔳");
    for (int i = 0; i < 8; i++) printf("🔳");
    printf("🔳\n");

    // 맵 출력 (0~18행, x=1~8열)
    for (int y_pos = 0; y_pos < 19; y_pos++) {
        printf("🔳"); // 왼쪽 벽

        for (int x_pos = 1; x_pos <= 8; x_pos++) {
            int cell = tetris_table[y_pos][x_pos];
            int is_block = 0;

            // 현재 떨어지는 블럭 위치인지 확인
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (y + i == y_pos && x + j == x_pos) {
                        char (*block)[4][4] = get_block_array(block_number);
                        if (block[block_state][i][j]) {
                            is_block = 1;
                        }
                    }
                }
            }

            if (is_block)
                printf("🟨");  // 현재 블럭
            else if (cell)
                printf("🟥");  // 고정된 블럭
            else
                printf("  ");  // 빈 공간
        }

        printf("🔳\n"); // 오른쪽 벽
    }

    // 하단 테두리
    printf("🔳");
    for (int i = 0; i < 8; i++) printf("🔳");
    printf("🔳\n");

    // 다음 블럭 출력
    printf("\n다음 블럭\n");
    char(*next_block)[4][4] = get_block_array(next_block_number);

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(next_block[0][i][j]){
                printf("🟨");
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

void search_result()
{
    char name[30];
    int found = 0;

    // Clear input buffer before reading new input
    while(getchar() != '\n'); 

    printf("기록 검색을 원하는 사람의 이름을 입력하세요: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    
    for(int i = 0; i < result_count; i++){
        if(strcmp(result_list[i].name, name) == 0){
            printf("%s님의 기록: %ld점        %d-%d-%d %d:%d\n", name, result_list[i].point, result_list[i].year, result_list[i].month, result_list[i].day, result_list[i].hour, result_list[i].min);
            found = 1;
        }
    }
    if(!found){
        printf("해당 사람의 기록이 없습니다.\n");
    }
    printf("\n아무 키나 누르면 메뉴로 돌아갑니다.\n");
    // Wait for a key press
#ifdef _WIN32
    _getch();
#else
    getchar();
#endif
}

void print_result()
{
    printf("\n===== 기록 확인 =====\n");

    for(int i = 0 ; i < result_count; i++){
        printf("%d등: %s -> %ld점        %d-%d-%d %d:%d\n", i+1, result_list[i].name, result_list[i].point, result_list[i].year, result_list[i].month, result_list[i].day, result_list[i].hour, result_list[i].min);
    }
    printf("\n아무 키나 누르면 메뉴로 돌아갑니다.\n");
    // Need to consume the newline character from previous scanf, then wait for another key.
    // This is a common issue with mixing scanf and fgets/getchar.
    while(getchar() != '\n'); 
#ifdef _WIN32
    _getch();
#else
    getchar();
#endif
}

void process_key(char key)
{
    switch (key)
    {
        case 'j': // 왼쪽 이동
            if(can_move(-1, 0)) x--;
            break;
        case 'l': // 오른쪽 이동
            if(can_move(1, 0)) x++;
            break;
        case 'k': // 아래로 이동
            if(can_move(0, 1)){
                y++; // 내려 갈 수 있으면 내려가고 아니면 고정
            } else{
                fix_block();
                delete_blocks();
                spawn_block();
            }
            break;
        case 'i': {// 회전
            int next_state = (block_state + 1) % 4;
            char (*block)[4][4] = get_block_array(block_number);
            int rotate = 1;

            for(int i = 0; i < 4 && rotate; i++){
                for(int j = 0; j < 4; j++){
                    if(block[next_state][i][j]){
                        int ny = y + i;
                        int nx = x + j;
                        // Check boundaries and collisions for the rotated block
                        if(ny >= 20 || nx < 1 || nx >= 9 || tetris_table[ny][nx]){
                            rotate = 0;
                            break;
                        }
                    }
                }
            }
            if(rotate) block_state = (block_state + 1) % 4;
            break;
        } // End of case 'i' block
        case 'a': // 떨어지기
            while(can_move(0, 1)){
                y++;
            }
            fix_block();
            delete_blocks();
            spawn_block();
            break;
        case 'p': // 종료
            game = GAME_END;
            break;
        default:
            break;
    }
}

int check_reach()
{
    return !can_move(0, 1);
}

void fix_block()
{
    char (*block)[4][4] = get_block_array(block_number);

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(block[block_state][i][j]){
                int ny = y + i;
                int nx = x + j;

                if(ny >= 0 && ny < 20 && nx >= 0 && nx < 10){
                    tetris_table[ny][nx] = 1;
                }
            }
        }
    }
}

void delete_blocks()
{
    for(int i = 18; i >= 0; i--){
        int full = 1;
        for(int j = 1; j <= 8; j++){
            if(tetris_table[i][j] == 0){
                full = 0;
                break;
            }
        }
        if(full){
            for(int k = i; k > 0; k--){
                for(int j = 1; j <= 8; j++){
                    tetris_table[k][j] = tetris_table[k-1][j];
                }
            }

            for(int j = 1; j <=8; j++){
                tetris_table[0][j] = 0;
            }

            i++;
            point += 100;
        }
    }
}

void spawn_block()
{
    block_number = next_block_number;
    next_block_number = rand() % 7;
    x = 3; y = 0;
    block_state = 0;

    if(game_over()){
            game = GAME_END;
    }
}

int game_over()
{
    char (*block)[4][4] = get_block_array(block_number);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(block[block_state][i][j]){
                int ny = y + i;
                int nx = x + j;
                if(tetris_table[ny][nx]){
                    return 1; // 충돌
                }
            }
        }
    }
    return 0;
}

void save_score()
{
    if(result_count >= MAX_RESULT){
        printf("사용할 수 있는 용량 초과");
        return;
    }

    printf("이름을 입력하세요: ");
    // Clear input buffer before reading new input
    while(getchar() != '\n'); 
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

    // 기록 별로 내림차순 정렬
    for(int i = 0; i < result_count - 1; i++){
        for(int j = i + 1; j < result_count; j++){
            if(result_list[i].point < result_list[j].point){
                struct result tmp = result_list[i];
                result_list[i] = result_list[j];
                result_list[j] = tmp;
            }
        }
    }
    
    // 랭킹 설정
    for(int i = 0; i < result_count; i++){
        result_list[i].rank = i + 1;
    }
}

int game_start()
{
    reset_game();
    char key;

#ifdef _WIN32
    DWORD last_drop_time = GetTickCount(); // Milliseconds since system start
#else
    struct timeval last_drop, current;
    gettimeofday(&last_drop, NULL);
    set_raw_mode(); // Set terminal to raw mode for non-blocking input
#endif

    while(game == GAME_START)
    {
        display_game(); // Clear screen and display game state

#ifdef _WIN32
        DWORD current_time = GetTickCount();
        long elapsed = current_time - last_drop_time;
#else
        gettimeofday(&current, NULL);
        long elapsed = (current.tv_sec - last_drop.tv_sec) * 1000000L + (current.tv_usec - last_drop.tv_usec);
#endif

        // 0.6초 마다 내려감 (600 milliseconds or 600000 microseconds)
        if (elapsed >= 600) { // For Windows GetTickCount, this is milliseconds
#ifndef _WIN32
             if (elapsed >= 600000) { // For Linux/macOS gettimeofday, this is microseconds
#endif
                if(can_move(0, 1)){
                    y++; // 내려 갈 수 있으면 내려가고 아니면 고정
                } else{
                    fix_block();
                    delete_blocks();
                    spawn_block();
                }
#ifdef _WIN32
                last_drop_time = current_time;
#else
                gettimeofday(&last_drop, NULL);
#endif
#ifndef _WIN32
            }
#endif
        }


        key = get_key();
        if(key) {
            process_key(key);
        }

        if(game == GAME_END){
            printf("게임 오버! 1초뒤에 메뉴로 돌아갑니다!\n");
#ifdef _WIN32
            Sleep(1000); // 1 second delay (milliseconds)
            FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
#else
            sleep(1); // 1 second delay (seconds)
            tcflush(STDIN_FILENO, TCIFLUSH); // Clear input buffer
            restore_termios(); // Restore terminal mode
#endif
            break;
        }

#ifdef _WIN32
        Sleep(60); // Small delay to prevent CPU hogging and reduce flicker
#else
        usleep(60000); // Small delay (microseconds)
#endif
    }
    
    save_score();
    return 1;
}

/// 테트리스 게임 메인 함수
/// 메뉴를 표시하고 사용자의 선택에 따라 게임을 시작하거나 결과를 검색하거나 종료합니다.
/// @param  
/// @return 
int main(void)
{
    srand(time(NULL) + getpid()); // 난수 생성 시드 설정 (getpid() is POSIX, but common on Win with some libs)
                                  // For pure Windows, you might just use time(NULL) or a more robust seed.

    int menu = 1;

    while(menu)
    {
        menu = display_menu();

        if(menu == 1)
        {
            game = GAME_START;
            menu = game_start();
        }
        else if(menu == 2)
        {
            search_result();
        }
        else if(menu == 3)
        {
            print_result();
        }
        else if(menu == 4)
        {   
            exit(0);
        }
    }

    return 0;
}