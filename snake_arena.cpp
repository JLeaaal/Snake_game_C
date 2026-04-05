/*
 * ============================================================
 * SNAKE - Julia Leal Ferreira de Paula Nunes - RA: 250130592
 * ============================================================
 *
 * MELHORIAS FEITAS:
 * 1) Menu de pausa melhorado com opcoes navegaveis
 * 2) Mudanças visuais como menu, ranking, arena, painel lateral e cores
 * 3) Pequenos ajustes visuais para ficar mais apresentavel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

/* ============================================================
 * BLOCO DE DEFINICOES GERAIS DE TAMANHO DA TELA
 * ============================================================
 * Aqui ficam as medidas principais do console, arena e painel.
 */

#define WIDTH       40
#define HEIGHT      26
#define TOP          3

#define ARENA_LEFT   1
#define ARENA_RIGHT (ARENA_LEFT + WIDTH*2 + 1)

#define PANEL_X     (ARENA_RIGHT + 3)
#define PANEL_W     24

#define CON_COLS    (PANEL_X + PANEL_W + 2)
#define CON_ROWS    35

/* ============================================================
 * BLOCO DE DEFINICOES DO JOGO
 * ============================================================
 * Quantidade de frutas, tamanho maximo da cobra, ranking etc.
 */

#define FRUIT_COUNT     6
#define MAX_SNAKE     800
#define MAX_RANKING     5
#define RANKING_FILE  "ranking.txt"
#define OBSTACLE_COUNT  20

#define INITIAL_SPEED  140
#define MIN_SPEED       50
#define LEVEL_STEP      80

/* ============================================================
 * BLOCO DE DEFINICOES DO BFS
 * ============================================================
 * BFS_MAX guarda o total de celulas possiveis no tabuleiro.
 */

#define BFS_MAX     (WIDTH * HEIGHT)

/* Estrutura simples para guardar coordenadas da BFS */
typedef struct { int x, y; } BfsNode;

/* Fila usada pelo algoritmo BFS */
BfsNode bfs_queue[BFS_MAX];
int     bfs_front = 0;
int     bfs_rear  = 0;

/* Matrizes auxiliares da BFS */
int     bfs_visited[WIDTH + 1][HEIGHT + 1];
BfsNode bfs_parent[WIDTH + 1][HEIGHT + 1];

/* Vetor que guarda o caminho encontrado pela BFS */
BfsNode bfs_path[BFS_MAX];
int     bfs_path_len = 0;

/* Vetor que guarda as celulas visitadas pela BFS */
BfsNode bfs_visited_cells[BFS_MAX];
int     bfs_visited_count = 0;

/* Vetores de deslocamento nas 4 direcoes */
int dx[4] = { 1, -1, 0,  0 };
int dy[4] = { 0,  0, 1, -1 };

/* ============================================================
 * BLOCO DE SIMBOLOS UNICODE / EMOJIS
 * ============================================================
 * Usados para deixar o visual do jogo mais bonito no console.
 */

/* Cobra */
#define SYM_HEAD        "\xF0\x9F\x9F\xA2"
#define SYM_HEAD_AUTO   "\xF0\x9F\xA4\x96"
#define SYM_BODY        "\xF0\x9F\x9F\xA9"
#define SYM_TAIL        "\xF0\x9F\x9F\xA8"
#define SYM_BODY_FAST   "\xF0\x9F\x94\xB5"

/* Frutas */
#define SYM_APPLE       "\xF0\x9F\x8D\x8E"
#define SYM_GRAPE       "\xF0\x9F\x8D\x87"
#define SYM_CHERRY      "\xF0\x9F\x8D\x92"
#define SYM_STAR        "\xE2\xAD\x90"
#define SYM_GEM         "\xF0\x9F\x92\x8E"

/* Elementos do HUD e efeitos */
#define SYM_PAUSE       "\xE2\x8F\xB8"
#define SYM_SKULL       "\xF0\x9F\x92\x80"
#define SYM_TROPHY      "\xF0\x9F\x8F\x86"
#define SYM_SPEED_SLOW  "\xF0\x9F\x90\xA2"
#define SYM_SPEED_MED   "\xF0\x9F\x9A\xB6"
#define SYM_SPEED_FAST  "\xF0\x9F\x9A\x80"
#define SYM_SPEED_INS   "\xE2\x9A\xA1"
#define SYM_SNAKE_TITLE "\xF0\x9F\x90\x8D"
#define SYM_CROWN       "\xF0\x9F\x91\x91"
#define SYM_CLOCK       "\xE2\x8F\xB1"
#define SYM_CTRL        "\xF0\x9F\x8E\xAE"
#define SYM_FIRE        "\xF0\x9F\x94\xA5"
#define SYM_SNAKE_EAT   "\xF0\x9F\x8D\xBD"
#define SYM_HEART       "\xE2\x9D\xA4"
#define SYM_ROBOT       "\xF0\x9F\xA4\x96"
#define SYM_BRAIN       "\xF0\x9F\xA7\xA0"
#define SYM_PATH        "\xC2\xB7"
#define SYM_OBSTACLE    "\xE2\x96\xA0"

/* Bordas */
#define BOX_TL   "\xE2\x94\x8C"
#define BOX_TR   "\xE2\x94\x90"
#define BOX_BL   "\xE2\x94\x94"
#define BOX_BR   "\xE2\x94\x98"
#define BOX_H    "\xE2\x94\x80"
#define BOX_V    "\xE2\x94\x82"
#define BOX_TLH  "\xE2\x95\x94"
#define BOX_TRH  "\xE2\x95\x97"
#define BOX_BLH  "\xE2\x95\x9A"
#define BOX_BRH  "\xE2\x95\x9D"
#define BOX_DH   "\xE2\x95\x90"
#define BOX_DV   "\xE2\x95\x91"
#define BOX_LM   "\xE2\x95\xA0"
#define BOX_RM   "\xE2\x95\xA3"
#define BLOCK    "\xE2\x96\x93"
#define DOT_BG   "\xC2\xB7"

/* Barra de progresso */
#define BAR_FULL  "\xE2\x96\x88"
#define BAR_HALF  "\xE2\x96\x91"

/* ============================================================
 * BLOCO DE CORES
 * ============================================================
 * Cores da interface, tabuleiro, modo manual, auto etc.
 */

#define C_RESET      7
#define C_BORDER     14
#define C_PANEL      13
#define C_PANEL_HDR  11
#define C_PANEL_VAL  15
#define C_SCORE      14
#define C_LEVEL      10
#define C_TIME       11
#define C_SPEED      14
#define C_RECORD     10
#define C_RUNNING    10
#define C_PAUSED     14
#define C_BG         1
#define C_TITLE      14
#define C_SELECT     10
#define C_NORMAL     7
#define C_FLASH      15
#define C_FLASH2     14
#define C_GAMEOVER   12
#define C_BEST       10
#define C_WALL       8
#define C_COMBO      14
#define C_BAR_FILL   11
#define C_BAR_EMPTY  1
#define C_BORDER_L1  11
#define C_BORDER_L2  10
#define C_BORDER_L3  14
#define C_BORDER_L4  12
#define C_BORDER_L5  13

/* Cores ligadas ao BFS */
#define C_BFS_PATH   11
#define C_BFS_VISIT  3
#define C_OBSTACLE   4
#define C_MODE_MAN   10
#define C_MODE_AUTO  9
#define C_BFS_INFO   3

/* ============================================================
 * BLOCO DE TIPOS DO JOGO
 * ============================================================
 * Estruturas e enums usados para organizar melhor o sistema.
 */

/* Segmento simples com x e y */
typedef struct { int x, y; } Segment;

/* Estrutura de fruta */
typedef struct {
    int   x, y, value;
    const char *symbol;
    int   color, rarity;
} Fruit;

/* Estrutura de ranking */
typedef struct {
    char name[32];
    int  score;
} RankEntry;

/* Estados de tela do jogo */
typedef enum {
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_RANKING,
    STATE_GAMEOVER,
    STATE_EXIT
} GameState;

/* Direcoes da cobra */
typedef enum {
    DIR_UP = 0, DIR_RIGHT, DIR_DOWN, DIR_LEFT
} Direction;

/* ============================================================
 * BLOCO DE GLOBAIS
 * ============================================================
 * Variaveis globais do sistema.
 */

HANDLE hConsole;

Segment   snake[MAX_SNAKE];
Fruit     fruits[FRUIT_COUNT];
RankEntry ranking[MAX_RANKING];

/* Vetor e contador de obstaculos */
Segment obstacles[OBSTACLE_COUNT];
int     obstacleCount = 0;

/* Variaveis gerais da cobra e do jogo */
int snakeLen  = 0;
Direction dir     = DIR_RIGHT;
Direction nextDir = DIR_RIGHT;

int score         = 0;
int level         = 1;
int speedMs       = INITIAL_SPEED;
int paused        = 0;
int gameOver      = 0;
int fruitsEaten   = 0;
int comboCount    = 0;
int lastComboShow = -1;

/* Controle das telas */
int menuIndex      = 0;
int gameOverIndex  = 0;

/* Flags de redesenho */
static int menuNeedsRedraw     = 1;
static int menuLastIndex       = -1;
static int gameoverNeedsRedraw = 1;
static int gameoverLastIndex   = -1;
static int rankingNeedsRedraw  = 1;

/* Controle de tempo */
time_t startTime;
time_t pauseStart;
int    pausedSeconds = 0;

/* Ultimos valores desenhados no painel */
static int lastElapsed  = -1;
static int lastLevel    = -1;
static int lastScore    = -1;
static int lastFruits   = -1;
static int lastCombo    = -1;

/* Variaveis do modo manual/automatico e visualizacao do BFS */
int autoMode      = 0;
int showBfsPath   = 1;
int showBfsVisit  = 0;

/* ============================================================
 * MELHORIA ADICIONADA: MENU DE PAUSA
 * ============================================================
 * Aqui ficam as opcoes do menu de pausa.
 */

int pauseMenuIndex = 0;

/* ============================================================
 * BLOCO DE FUNCOES DE CONSOLE
 * ============================================================
 * Funcoes basicas para posicionar cursor, mudar cor etc.
 */

/* Move o cursor do console para a coordenada x,y */
void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, c);
}

/* Altera a cor atual do texto */
void set_color(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

/* Esconde o cursor para o jogo ficar visualmente melhor */
void hide_cursor(void) {
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hConsole, &ci);
}

/* Mostra o cursor novamente quando for necessario digitar */
void show_cursor(void) {
    CONSOLE_CURSOR_INFO ci = { 20, TRUE };
    SetConsoleCursorInfo(hConsole, &ci);
}

/* Limpa a tela do console */
void clear_screen(void) {
    system("cls");
}

/* Escreve um texto em uma posicao especifica com uma cor */
void write_at(int x, int y, int color, const char *text) {
    gotoxy(x, y);
    set_color(color);
    printf("%s", text);
    set_color(C_RESET);
    fflush(stdout);
}

/* Preenche uma quantidade de espacos em branco numa linha */
void fill_spaces(int x, int y, int n) {
    gotoxy(x, y);
    int i;
	for (i = 0; i < n; i++) {
    putchar(' ');

    }
    fflush(stdout);
}

/* Remove quebra de linha do texto digitado pelo usuario */
void trim_newline(char *s) {
    size_t n = strlen(s);

    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

/* ============================================================
 * BLOCO DE DESENHO DE CAIXAS / BORDAS
 * ============================================================
 */

/* Desenha uma caixa simples com borda unicode */
void draw_box_unicode(int left, int top, int right, int bottom, int color) {
    set_color(color);

    gotoxy(left, top);
    printf("%s", BOX_TL);
    for (int x = left + 1; x < right; x++) {
        printf("%s", BOX_H);
    }
    printf("%s", BOX_TR);

    for (int y = top + 1; y < bottom; y++) {
        gotoxy(left, y);
        printf("%s", BOX_V);

        gotoxy(right, y);
        printf("%s", BOX_V);
    }

    gotoxy(left, bottom);
    printf("%s", BOX_BL);
    for (int x = left + 1; x < right; x++) {
        printf("%s", BOX_H);
    }
    printf("%s", BOX_BR);

    set_color(C_RESET);
    fflush(stdout);
}

/* Desenha uma caixa dupla para telas e paines mais destacados */
void draw_box_double(int left, int top, int right, int bottom, int color) {
    set_color(color);

    gotoxy(left, top);
    printf("%s", BOX_TLH);
    for (int x = left + 1; x < right; x++) {
        printf("%s", BOX_DH);
    }
    printf("%s", BOX_TRH);

    for (int y = top + 1; y < bottom; y++) {
        gotoxy(left, y);
        printf("%s", BOX_DV);

        gotoxy(right, y);
        printf("%s", BOX_DV);
    }

    gotoxy(left, bottom);
    printf("%s", BOX_BLH);
    for (int x = left + 1; x < right; x++) {
        printf("%s", BOX_DH);
    }
    printf("%s", BOX_BRH);

    set_color(C_RESET);
    fflush(stdout);
}

/* Desenha uma linha separadora dupla */
void draw_separator_double(int left, int y, int right, int color) {
    set_color(color);

    gotoxy(left, y);
    printf("%s", BOX_LM);
    for (int x = left + 1; x < right; x++) {
        printf("%s", BOX_DH);
    }
    printf("%s", BOX_RM);

    set_color(C_RESET);
    fflush(stdout);
}

/* Limpa o interior de uma caixa */
void clear_inside_box(int left, int top, int right, int bottom) {
    for (int y = top + 1; y < bottom; y++) {
        gotoxy(left + 1, y);
        for (int x = left + 1; x < right; x++) {
            putchar(' ');
        }
    }
    fflush(stdout);
}

/* Escreve texto centralizado dentro de um intervalo horizontal */
void print_centered_range(int left, int right, int y, int color, const char *text) {
    int width   = right - left - 1;
    int textlen = (int)strlen(text);
    int x = left + 1 + (width - textlen) / 2;

    if (x < left + 1) {
        x = left + 1;
    }

    write_at(x, y, color, text);
}

/* ============================================================
 * COR DA BORDA POR NIVEL
 * ============================================================
 * Troca de cor da arena conforme o nivel sobe.
 */

int border_color_for_level(void) {
    switch ((level - 1) % 5) {
        case 0: return C_BORDER_L1;
        case 1: return C_BORDER_L2;
        case 2: return C_BORDER_L3;
        case 3: return C_BORDER_L4;
        case 4: return C_BORDER_L5;
    }

    return C_BORDER_L1;
}

/* ============================================================
 * BLOCO DE RANKING
 * ============================================================
 */

/* Carrega o ranking do arquivo texto */
void load_ranking(int *count) {
    FILE *f = fopen(RANKING_FILE, "r");

    *count = 0;

    if (!f) {
        return;
    }

    while (*count < MAX_RANKING &&
           fscanf(f, "%31[^;];%d\n", ranking[*count].name, &ranking[*count].score) == 2) {
        (*count)++;
    }

    fclose(f);
}

/* Salva o ranking no arquivo texto */
void save_ranking(int count) {
    FILE *f = fopen(RANKING_FILE, "w");

    if (!f) {
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(f, "%s;%d\n", ranking[i].name, ranking[i].score);
    }

    fclose(f);
}

/* Retorna o melhor score atual do ranking */
int best_score(void) {
    int count = 0;
    load_ranking(&count);

    if (count) {
        return ranking[0].score;
    }

    return 0;
}

/* Insere um novo score no ranking e reordena */
void insert_ranking(const char *name, int sc) {
    int count = 0;

    load_ranking(&count);

    if (count < MAX_RANKING) {
        strncpy(ranking[count].name, name, 31);
        ranking[count].name[31] = '\0';
        ranking[count].score = sc;
        count++;
    } else if (sc > ranking[count - 1].score) {
        strncpy(ranking[count - 1].name, name, 31);
        ranking[count - 1].name[31] = '\0';
        ranking[count - 1].score = sc;
    } else {
        return;
    }

    /* Bubble sort simples, suficiente para ranking pequeno */
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (ranking[j].score > ranking[i].score) {
                RankEntry t = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = t;
            }
        }
    }

    if (count > MAX_RANKING) {
        count = MAX_RANKING;
    }

    save_ranking(count);
}

/* ============================================================
 * FUNCOES UTILITARIAS
 * ============================================================
 */

/* Verifica se uma coordenada pertence ao corpo da cobra */
int is_on_snake(int x, int y) {
    for (int i = 0; i < snakeLen; i++) {
        if (snake[i].x == x && snake[i].y == y) {
            return 1;
        }
    }

    return 0;
}

/* Verifica se existe obstaculo na coordenada */
int is_obstacle(int x, int y) {
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].x == x && obstacles[i].y == y) {
            return 1;
        }
    }

    return 0;
}

/* Retorna o indice da fruta numa coordenada ou -1 */
int fruit_at(int x, int y) {
    for (int i = 0; i < FRUIT_COUNT; i++) {
        if (fruits[i].x == x && fruits[i].y == y) {
            return i;
        }
    }

    return -1;
}

/* Converte coluna logica da arena para coluna real do console */
static int arena_col(int logical_x) {
    return ARENA_LEFT + 1 + (logical_x - 1) * 2;
}

/* Verifica se a celula pode ser usada pela BFS */
int is_free_for_bfs(int x, int y) {
    if (x < 1 || x > WIDTH || y <= TOP || y >= HEIGHT) {
        return 0;
    }

    if (is_obstacle(x, y)) {
        return 0;
    }

    /* O corpo da cobra bloqueia, exceto a cabeca */
    for (int i = 1; i < snakeLen; i++) {
        if (snake[i].x == x && snake[i].y == y) {
            return 0;
        }
    }

    return 1;
}

/* ============================================================
 * BLOCO BFS
 * ============================================================
 * Mantida a logica do codigo base.
 */

/* Coloca um no na fila do BFS */
void bfs_enqueue(int x, int y) {
    bfs_queue[bfs_rear].x = x;
    bfs_queue[bfs_rear].y = y;
    bfs_rear++;
}

/* Retira e retorna o primeiro no da fila */
BfsNode bfs_dequeue(void) {
    return bfs_queue[bfs_front++];
}

/* Executa a BFS da posicao inicial ate a fruta */
int run_bfs(int sx, int sy, int fx, int fy) {
    bfs_front = 0;
    bfs_rear  = 0;
    bfs_path_len = 0;
    bfs_visited_count = 0;

    memset(bfs_visited, 0, sizeof(bfs_visited));

    bfs_enqueue(sx, sy);
    bfs_visited[sx][sy] = 1;

    bfs_visited_cells[bfs_visited_count].x = sx;
    bfs_visited_cells[bfs_visited_count].y = sy;
    bfs_visited_count++;

    while (bfs_front != bfs_rear) {
        BfsNode cur = bfs_dequeue();

        /* Se chegou na fruta, reconstrui o caminho */
        if (cur.x == fx && cur.y == fy) {
            BfsNode temp_path[BFS_MAX];
            int     temp_len = 0;
            BfsNode node = { fx, fy };

            while (!(node.x == sx && node.y == sy)) {
                temp_path[temp_len++] = node;
                node = bfs_parent[node.x][node.y];
            }

            bfs_path_len = 0;

            for (int i = temp_len - 1; i >= 0; i--) {
                bfs_path[bfs_path_len++] = temp_path[i];
            }

            return 1;
        }

        /* Explora os 4 vizinhos */
        for (int i = 0; i < 4; i++) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];

            if (nx >= 1 && ny > TOP && nx <= WIDTH && ny < HEIGHT &&
                !bfs_visited[nx][ny] &&
                is_free_for_bfs(nx, ny)) {

                bfs_enqueue(nx, ny);
                bfs_visited[nx][ny] = 1;
                bfs_parent[nx][ny] = cur;

                if (bfs_visited_count < BFS_MAX) {
                    bfs_visited_cells[bfs_visited_count].x = nx;
                    bfs_visited_cells[bfs_visited_count].y = ny;
                    bfs_visited_count++;
                }
            }
        }
    }

    return 0;
}

/* Procura a fruta mais proxima com base no menor caminho BFS */
int find_nearest_fruit_bfs(void) {
    int best_len = 999999;
    int best_idx = -1;
    BfsNode best_path[BFS_MAX];
    int     best_path_len = 0;
    BfsNode best_visited[BFS_MAX];
    int     best_visited_count = 0;

    for (int i = 0; i < FRUIT_COUNT; i++) {
        if (run_bfs(snake[0].x, snake[0].y, fruits[i].x, fruits[i].y)) {
            if (bfs_path_len < best_len) {
                best_len = bfs_path_len;
                best_idx = i;
                best_path_len = bfs_path_len;
                memcpy(best_path, bfs_path, sizeof(BfsNode) * bfs_path_len);
                best_visited_count = bfs_visited_count;
                memcpy(best_visited, bfs_visited_cells, sizeof(BfsNode) * bfs_visited_count);
            }
        }
    }

    if (best_idx >= 0) {
        bfs_path_len = best_path_len;
        memcpy(bfs_path, best_path, sizeof(BfsNode) * best_path_len);

        bfs_visited_count = best_visited_count;
        memcpy(bfs_visited_cells, best_visited, sizeof(BfsNode) * best_visited_count);
        return 1;
    }

    bfs_path_len = 0;
    bfs_visited_count = 0;
    return 0;
}

/* Verifica se uma coordenada esta no caminho atual do BFS */
int is_on_bfs_path(int x, int y) {
    for (int i = 0; i < bfs_path_len; i++) {
        if (bfs_path[i].x == x && bfs_path[i].y == y) {
            return 1;
        }
    }

    return 0;
}

/* Verifica se uma coordenada foi visitada pela BFS */
int is_bfs_visited(int x, int y) {
    for (int i = 0; i < bfs_visited_count; i++) {
        if (bfs_visited_cells[i].x == x && bfs_visited_cells[i].y == y) {
            return 1;
        }
    }

    return 0;
}

/* ============================================================
 * BARRA DE PROGRESSO
 * ============================================================
 */

/* Desenha uma barra horizontal representando o progresso do nivel */
void draw_progress_bar(int x, int y, int bar_w, int value, int maxval) {
    if (maxval <= 0) {
        maxval = 1;
    }

    int filled = (value * bar_w) / maxval;

    if (filled > bar_w) {
        filled = bar_w;
    }

    gotoxy(x, y);
    set_color(C_BAR_FILL);

    for (int i = 0; i < filled; i++) {
        printf("%s", BAR_FULL);
    }

    set_color(C_BAR_EMPTY);

    for (int i = filled; i < bar_w; i++) {
        printf("%s", BAR_HALF);
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* ============================================================
 * OBSTACULOS
 * ============================================================
 */

/* Gera obstaculos aleatorios sem colocar perto do inicio */
void spawn_obstacles(void) {
    obstacleCount = 0;

    int mx = WIDTH / 2;
    int my = (TOP + HEIGHT) / 2;

    for (int i = 0; i < OBSTACLE_COUNT && obstacleCount < OBSTACLE_COUNT; i++) {
        int x, y, ok;
        int attempts = 0;

        do {
            ok = 1;
            x = rand() % WIDTH + 1;
            y = rand() % (HEIGHT - TOP - 1) + TOP + 1;

            /* Evita nascer muito perto do centro */
            if (abs(x - mx) <= 5 && abs(y - my) <= 2) {
                ok = 0;
                continue;
            }

            if (is_on_snake(x, y)) {
                ok = 0;
                continue;
            }

            if (is_obstacle(x, y)) {
                ok = 0;
                continue;
            }

            attempts++;

            if (attempts > 200) {
                break;
            }
        } while (!ok);

        if (attempts <= 200) {
            obstacles[obstacleCount].x = x;
            obstacles[obstacleCount].y = y;
            obstacleCount++;
        }
    }
}

/* Desenha um unico obstaculo */
void draw_obstacle(int x, int y) {
    int col = arena_col(x);

    gotoxy(col, y);
    set_color(C_OBSTACLE);
    printf("%s ", SYM_OBSTACLE);
    set_color(C_RESET);
    fflush(stdout);
}

/* Desenha todos os obstaculos */
void draw_all_obstacles(void) {
    for (int i = 0; i < obstacleCount; i++) {
        draw_obstacle(obstacles[i].x, obstacles[i].y);
    }
}

/* ============================================================
 * TABULEIRO
 * ============================================================
 */

/* Desenha a borda principal da arena */
void draw_board_border(void) {
    int bc = border_color_for_level();
    set_color(bc);

    /* Linha superior da arena */
    gotoxy(ARENA_LEFT, TOP);
    printf("%s", BOX_TL);
    for (int x = 0; x < WIDTH * 2; x++) {
        printf("%s", BOX_H);
    }
    printf("%s", BOX_TR);

    /* Laterais da arena */
    for (int y = TOP + 1; y < HEIGHT; y++) {
        gotoxy(ARENA_LEFT, y);
        printf("%s", BLOCK);

        gotoxy(ARENA_RIGHT, y);
        printf("%s", BLOCK);
    }

    /* Linha inferior da arena */
    gotoxy(ARENA_LEFT, HEIGHT);
    printf("%s", BOX_BL);
    for (int x = 0; x < WIDTH * 2; x++) {
        printf("%s", BOX_H);
    }
    printf("%s", BOX_BR);

    /* Pequenos detalhes visuais nas bordas superiores */
    write_at(ARENA_LEFT + 3, TOP, bc, "SNAKE");
    write_at(ARENA_RIGHT - 8, TOP, bc, "ARENA");

    set_color(C_RESET);
    fflush(stdout);
}

/* Desenha o fundo interno da arena */
void draw_board_background(void) {
    /* Fundo da arena com padrao visual mais forte, sem alterar a logica do jogo */
    for (int y = TOP + 1; y < HEIGHT; y++) {
        gotoxy(ARENA_LEFT + 1, y);

        for (int x = 0; x < WIDTH; x++) {
            int logicalX = x + 1;

            if ((logicalX + y) % 6 == 0) {
                set_color(9);
                printf("%s ", DOT_BG);
            } else if ((logicalX + y) % 3 == 0) {
                set_color(C_BG);
                printf("%s ", DOT_BG);
            } else {
                set_color(0);
                printf("  ");
            }
        }
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* ============================================================
 * VISUALIZACAO DO BFS
 * ============================================================
 */

/* Desenha as celulas visitadas pela BFS */
void draw_bfs_visited_overlay(void) {
    if (!showBfsVisit) {
        return;
    }

    for (int i = 0; i < bfs_visited_count; i++) {
        int x = bfs_visited_cells[i].x;
        int y = bfs_visited_cells[i].y;

        if (is_on_snake(x, y)) {
            continue;
        }
        if (fruit_at(x, y) >= 0) {
            continue;
        }
        if (is_obstacle(x, y)) {
            continue;
        }
        if (is_on_bfs_path(x, y)) {
            continue;
        }

        int col = arena_col(x);
        gotoxy(col, y);
        set_color(C_BFS_VISIT);
        printf(". ");
        set_color(C_RESET);
    }

    fflush(stdout);
}

/* Desenha o caminho encontrado pelo BFS */
void draw_bfs_path_overlay(void) {
    if (!showBfsPath) {
        return;
    }

    for (int i = 0; i < bfs_path_len; i++) {
        int x = bfs_path[i].x;
        int y = bfs_path[i].y;

        if (is_on_snake(x, y)) {
            continue;
        }
        if (fruit_at(x, y) >= 0) {
            continue;
        }
        if (is_obstacle(x, y)) {
            continue;
        }

        int col = arena_col(x);
        gotoxy(col, y);
        set_color(C_BFS_PATH);
        printf("%s ", SYM_PATH);
        set_color(C_RESET);
    }

    fflush(stdout);
}

/* Limpa o overlay BFS para redesenhar atualizado */
void clear_bfs_overlay(void) {
    for (int i = 0; i < bfs_visited_count; i++) {
        int x = bfs_visited_cells[i].x;
        int y = bfs_visited_cells[i].y;

        if (is_on_snake(x, y)) {
            continue;
        }
        if (fruit_at(x, y) >= 0) {
            continue;
        }
        if (is_obstacle(x, y)) {
            continue;
        }

        int col = arena_col(x);
        gotoxy(col, y);
        set_color(C_BG);

        if ((x + y) % 4 == 0) {
            printf("%s ", DOT_BG);
        } else {
            printf("  ");
        }
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* ============================================================
 * PAINEL LATERAL
 * ============================================================
 */

/* Desenha o quadro do painel lateral */
void draw_side_panel_frame(void) {
    int L = PANEL_X;
    int R = PANEL_X + PANEL_W;

    /* Moldura principal do painel lateral */
    draw_box_double(L, TOP, R, HEIGHT, C_PANEL);

    /* Cabecalho decorativo */
    char title[48];
    snprintf(title, sizeof(title), "%s SNAKE BFS", SYM_BRAIN);
    print_centered_range(L, R, TOP + 1, C_TITLE, title);

    /* Linha superior com pequenos detalhes visuais */
    write_at(L + 2, TOP + 1, C_PANEL_HDR, SYM_TROPHY);
    write_at(L + 4, TOP + 1, C_PANEL_HDR, "====");
    write_at(R - 7, TOP + 1, C_PANEL_HDR, "====");
    write_at(R - 2, TOP + 1, C_PANEL_HDR, SYM_TROPHY);

    /* Separadores de secoes */
    draw_separator_double(L, TOP + 2, R, C_PANEL);
    draw_separator_double(L, TOP + 13, R, C_PANEL);
    draw_separator_double(L, TOP + 16, R, C_PANEL);
    draw_separator_double(L, TOP + 21, R, C_PANEL);

    /* Rotulos principais */
    write_at(L + 2, TOP + 3,  C_PANEL_HDR, "SCORE");
    write_at(L + 2, TOP + 5,  C_PANEL_HDR, "NIVEL");
    write_at(L + 2, TOP + 7,  C_PANEL_HDR, "SPEED");
    write_at(L + 2, TOP + 9,  C_PANEL_HDR, "TEMPO");
    write_at(L + 2, TOP + 11, C_PANEL_HDR, "RECORDE");

    /* Rotulos secundarios */
    write_at(L + 2, TOP + 14, C_PANEL_HDR, "MODO");
    write_at(L + 2, TOP + 17, C_PANEL_HDR, "BFS INFO");
    write_at(L + 2, TOP + 22, C_PANEL_HDR, "CONTROLES");

    /* Tags visuais */
    write_at(R - 8, TOP + 3,  C_SCORE,    "[PTS]");
    write_at(R - 8, TOP + 5,  C_LEVEL,    "[LVL]");
    write_at(R - 8, TOP + 7,  C_SPEED,    "[SPD]");
    write_at(R - 8, TOP + 9,  C_TIME,     "[TIM]");
    write_at(R - 8, TOP + 11, C_BEST,     "[TOP]");
    write_at(R - 8, TOP + 14, C_MODE_MAN, "[MOD]");
    write_at(R - 7, TOP + 17, C_BFS_INFO, "[AI]");
    write_at(R - 8, TOP + 22, C_PANEL_HDR,"[KEY]");
}

/* Escolhe o icone da velocidade atual */
const char *speed_icon(void) {
    if (speedMs > 110) return SYM_SPEED_SLOW;
    if (speedMs >  85) return SYM_SPEED_MED;
    if (speedMs >  65) return SYM_SPEED_FAST;
    return SYM_SPEED_INS;
}

/* Escolhe o texto da velocidade atual */
const char *speed_label(void) {
    if (speedMs > 110) return "SLOW  ";
    if (speedMs >  85) return "NORMAL";
    if (speedMs >  65) return "FAST  ";
    return "INSANE";
}

/* Atualiza os valores do painel lateral */
void update_side_panel(void) {
    int L = PANEL_X;
    char buf[64];

    if (lastScore != score) {
        snprintf(buf, sizeof(buf), "%06d", score);
        write_at(L + 2, TOP + 4, C_SCORE, buf);
        lastScore = score;
    }

    if (lastLevel != level) {
        snprintf(buf, sizeof(buf), "%d  ", level);
        write_at(L + 2, TOP + 6, C_LEVEL, buf);

        int levelBase  = (level - 1) * LEVEL_STEP;
        int levelNext  = level * LEVEL_STEP;
        int progress   = score - levelBase;
        int needed     = levelNext - levelBase;

        draw_progress_bar(L + 2, TOP + 7, PANEL_W - 3, progress, needed);
        lastLevel = level;
    } else {
        int levelBase = (level - 1) * LEVEL_STEP;
        int levelNext = level * LEVEL_STEP;
        int progress  = score - levelBase;
        int needed    = levelNext - levelBase;

        if (needed > 0) {
            draw_progress_bar(L + 2, TOP + 7, PANEL_W - 3, progress, needed);
        }
    }

    snprintf(buf, sizeof(buf), "%s %s", speed_icon(), speed_label());
    write_at(L + 2, TOP + 8, C_SPEED, buf);

    int elapsed = (int)(time(NULL) - startTime) - pausedSeconds;

    if (paused) {
        elapsed = (int)(pauseStart - startTime) - pausedSeconds;
    }

    if (elapsed < 0) {
        elapsed = 0;
    }

    if (elapsed != lastElapsed) {
        snprintf(buf, sizeof(buf), "%02d:%02d", elapsed / 60, elapsed % 60);
        write_at(L + 2, TOP + 10, C_TIME, buf);
        lastElapsed = elapsed;
    }

    int rec = best_score();
    snprintf(buf, sizeof(buf), "%06d", rec);
    write_at(L + 2, TOP + 12, (score > 0 && score >= rec) ? C_BEST : C_PANEL_VAL, buf);

    if (score > 0 && score >= rec) {
        write_at(L + 10, TOP + 12, C_BEST, SYM_CROWN);
    } else {
        write_at(L + 10, TOP + 12, C_RESET, "  ");
    }

    if (autoMode) {
        snprintf(buf, sizeof(buf), "%s AUTOMATICO  ", SYM_ROBOT);
        write_at(L + 2, TOP + 15, C_MODE_AUTO, buf);
    } else {
        snprintf(buf, sizeof(buf), "%s MANUAL      ", SYM_CTRL);
        write_at(L + 2, TOP + 15, C_MODE_MAN, buf);
    }

    snprintf(buf, sizeof(buf), "Caminho: %3d    ", bfs_path_len);
    write_at(L + 2, TOP + 18, C_BFS_INFO, buf);

    snprintf(buf, sizeof(buf), "Visitados: %3d  ", bfs_visited_count);
    write_at(L + 2, TOP + 19, C_BFS_INFO, buf);

    snprintf(buf, sizeof(buf), "Frutas: %d  ", fruitsEaten);
    write_at(L + 2, TOP + 20, C_PANEL_VAL, buf);

    if (autoMode) {
        write_at(L + 2, TOP + 23, C_NORMAL, "T  Modo Manual  ");
    } else {
        write_at(L + 2, TOP + 23, C_NORMAL, "WASD  Mover     ");
    }

    write_at(L + 2, TOP + 24, C_NORMAL, "V  Caminho BFS  ");
    write_at(L + 2, TOP + 25, C_NORMAL, "G  Visitados BFS");
    write_at(L + 2, TOP + 26, C_NORMAL, "T  Trocar Modo  ");
    write_at(L + 2, TOP + 27, C_NORMAL, "P  Pausar       ");

    if (lastCombo != comboCount) {
        if (comboCount >= 3) {
            snprintf(buf, sizeof(buf), "%s x%d COMBO!  ", SYM_FIRE, comboCount);
            write_at(L + 2, TOP + 28, C_COMBO, buf);
        } else {
            fill_spaces(L + 2, TOP + 28, PANEL_W - 3);
        }
        lastCombo = comboCount;
    }

    if (paused) {
        snprintf(buf, sizeof(buf), "%s PAUSADO ", SYM_PAUSE);
        write_at(L + 2, TOP + 29, C_PAUSED, buf);
    } else {
        fill_spaces(L + 2, TOP + 29, 12);
    }
}

/* ============================================================
 * COBRA
 * ============================================================
 */

/* Desenha a cabeca da cobra */
void draw_head(int x, int y) {
    gotoxy(arena_col(x), y);
    set_color(C_RESET);
    printf("%s", autoMode ? SYM_HEAD_AUTO : SYM_HEAD);
    fflush(stdout);
}

/* Desenha um segmento do corpo */
void draw_body(int x, int y) {
    gotoxy(arena_col(x), y);
    set_color(C_RESET);
    printf("%s", speedMs <= 65 ? SYM_BODY_FAST : SYM_BODY);
    fflush(stdout);
}

/* Desenha a cauda */
void draw_tail_seg(int x, int y) {
    gotoxy(arena_col(x), y);
    set_color(C_RESET);
    printf("%s", SYM_TAIL);
    fflush(stdout);
}

/* Apaga uma celula restaurando o fundo */
void erase_cell(int x, int y) {
    int col = arena_col(x);

    gotoxy(col, y);
    set_color(C_BG);

    if ((x + y) % 4 == 0) {
        printf("%s ", DOT_BG);
    } else {
        printf("  ");
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* ============================================================
 * FRUTAS
 * ============================================================
 */

/* Desenha uma fruta na tela */
void draw_fruit(int i) {
    int col = arena_col(fruits[i].x);

    gotoxy(col, fruits[i].y);
    set_color(fruits[i].color);
    printf("%s", fruits[i].symbol);
    set_color(C_RESET);
    fflush(stdout);
}

/* Tabela de tipos de fruta */
static const struct {
    int         value;
    const char *symbol;
    int         color;
    int         rarity;
} FRUIT_TABLE[] = {
    { 10, SYM_APPLE,  13, 0 },
    { 20, SYM_GRAPE,  13, 0 },
    { 30, SYM_CHERRY, 12, 0 },
    { 50, SYM_STAR,   14, 1 },
    { 80, SYM_GEM,    11, 2 },
};

#define FRUIT_TABLE_LEN 5

/* Faz uma pequena animacao quando a fruta nasce */
void spawn_fruit_anim(int i) {
    int col = arena_col(fruits[i].x);
    int y   = fruits[i].y;

    for (int f = 0; f < 2; f++) {
        gotoxy(col, y);
        set_color(C_FLASH);
        printf("* ");
        fflush(stdout);
        Sleep(40);

        erase_cell(fruits[i].x, y);
        Sleep(30);
    }

    draw_fruit(i);
}

/* Gera uma fruta em posicao valida */
void spawn_fruit(int i) {
    int x, y, ok;

    do {
        ok = 1;
        x = rand() % WIDTH + 1;
        y = rand() % (HEIGHT - TOP - 1) + TOP + 1;

        if (is_on_snake(x, y)) {
            ok = 0;
            continue;
        }

        if (is_obstacle(x, y)) {
            ok = 0;
            continue;
        }

        for (int j = 0; j < FRUIT_COUNT; j++) {
            if (j != i && fruits[j].x == x && fruits[j].y == y) {
                ok = 0;
                break;
            }
        }
    } while (!ok);

    int roll = rand() % 100;
    int fi;

    if (roll < 10) {
        fi = 4;
    } else if (roll < 40) {
        fi = 3;
    } else {
        fi = rand() % 3;
    }

    fruits[i].x      = x;
    fruits[i].y      = y;
    fruits[i].value  = FRUIT_TABLE[fi].value;
    fruits[i].symbol = FRUIT_TABLE[fi].symbol;
    fruits[i].color  = FRUIT_TABLE[fi].color;
    fruits[i].rarity = FRUIT_TABLE[fi].rarity;

    spawn_fruit_anim(i);
}
/* ============================================================
 * FUNCOES AUXILIARES DE DESENHO
 * ============================================================
 * Estas funcoes foram criadas para melhorar a organizacao visual
 * do codigo sem mudar a logica principal do jogo.
 */

/* Desenha novamente todas as frutas ja existentes no tabuleiro */
void draw_all_fruits(void) {
    for (int i = 0; i < FRUIT_COUNT; i++) {
        draw_fruit(i);
    }
}

/* Desenha a cobra inteira usando cabeca, corpo e cauda */
void draw_full_snake(void) {
    for (int i = snakeLen - 1; i >= 0; i--) {
        if (i == 0) {
            /* A primeira posicao do vetor sempre representa a cabeca */
            draw_head(snake[i].x, snake[i].y);
        } else if (i == snakeLen - 1) {
            /* A ultima posicao do vetor representa a cauda */
            draw_tail_seg(snake[i].x, snake[i].y);
        } else {
            /* As posicoes intermediarias representam o corpo */
            draw_body(snake[i].x, snake[i].y);
        }
    }
}


/* ============================================================
 * EFEITOS
 * ============================================================
 */

/* Faz flash na borda quando a cobra come uma fruta */
void flash_eat(int rarity) {
    int flashes    = rarity + 1;
    int flashColor = (rarity == 2) ? 11 : (rarity == 1 ? 14 : 10);
    int bc         = border_color_for_level();

    for (int f = 0; f < flashes; f++) {
        draw_box_unicode(ARENA_LEFT, TOP, ARENA_RIGHT, HEIGHT, flashColor);
        Sleep(25);

        draw_box_unicode(ARENA_LEFT, TOP, ARENA_RIGHT, HEIGHT, bc);
        Sleep(25);
    }

    draw_board_border();
}

/* Animacao simples de game over */
void anim_death(void) {
    int cx  = ARENA_LEFT + WIDTH;
    int cy  = (TOP + HEIGHT) / 2;

    char msg[32];
    snprintf(msg, sizeof(msg), " %s GAME OVER %s ", SYM_SKULL, SYM_SKULL);

    for (int i = 0; i < 4; i++) {
        draw_box_unicode(ARENA_LEFT, TOP, ARENA_RIGHT, HEIGHT, C_GAMEOVER);

        if (i % 2 == 0) {
            write_at(cx - (int)strlen(msg) / 2, cy, C_GAMEOVER, msg);
        } else {
            fill_spaces(cx - (int)strlen(msg) / 2, cy, (int)strlen(msg));
        }

        Sleep(150);
    }

    draw_board_border();
}

/* Animacao quando sobe de nivel */
void anim_level_up(void) {
    char msg[32];
    int cx = ARENA_LEFT + WIDTH;
    int cy = (TOP + HEIGHT) / 2;

    snprintf(msg, sizeof(msg), "  LEVEL %d!  ", level);

    for (int i = 0; i < 3; i++) {
        write_at(cx - (int)strlen(msg) / 2, cy, C_FLASH, msg);
        Sleep(120);

        fill_spaces(cx - (int)strlen(msg) / 2, cy, (int)strlen(msg));
        Sleep(80);
    }

    for (int x = 1; x <= WIDTH; x++) {
        erase_cell(x, cy);
    }

    for (int i = 0; i < FRUIT_COUNT; i++) {
        if (fruits[i].y == cy) {
            draw_fruit(i);
        }
    }

    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].y == cy) {
            draw_obstacle(obstacles[i].x, obstacles[i].y);
        }
    }

    for (int i = snakeLen - 1; i >= 0; i--) {
        if (snake[i].y != cy) {
            continue;
        }

        if (i == 0) {
            draw_head(snake[i].x, snake[i].y);
        } else {
            draw_body(snake[i].x, snake[i].y);
        }
    }

    draw_board_border();
}

/* ============================================================
 * MENU DE PAUSA - MELHORIA ADICIONADA
 * ============================================================
 * Agora a pausa nao mostra apenas "P para retomar".
 * Existe um pequeno menu navegavel com varias opcoes.
 */

/* Desenha o menu de pausa */
void draw_pause_overlay(void) {
    int arenaW = WIDTH * 2;
    int boxW   = 34;
    int L = ARENA_LEFT + 1 + (arenaW - boxW) / 2;
    int R = L + boxW;
    int T = (TOP + HEIGHT) / 2 - 5;
    int B = T + 10;

    draw_box_double(L, T, R, B, C_PAUSED);
    clear_inside_box(L, T, R, B);

    print_centered_range(L, R, T + 1, C_PAUSED, "PAUSE MENU");
    draw_separator_double(L, T + 2, R, C_PAUSED);

    /* As opcoes sao desenhadas abaixo */
    const char *options[5] = {
        "CONTINUAR",
        "TROCAR MODO",
        "MOSTRAR/OCULTAR CAMINHO",
        "MOSTRAR/OCULTAR VISITADOS",
        "VOLTAR AO MENU"
    };

    for (int i = 0; i < 5; i++) {
        int y = T + 3 + i;

        fill_spaces(L + 2, y, boxW - 1);

        if (i == pauseMenuIndex) {
            write_at(L + 3, y, C_SELECT, ">>");
            write_at(L + 6, y, C_SELECT, options[i]);
        } else {
            write_at(L + 6, y, C_NORMAL, options[i]);
        }
    }

    draw_separator_double(L, B - 1, R, C_PAUSED);
}

/* Limpa o menu de pausa e redesenha tudo o que estava por baixo */
void clear_pause_overlay(void) {
    int arenaW = WIDTH * 2;
    int boxW   = 34;
    int L = ARENA_LEFT + 1 + (arenaW - boxW) / 2;
    int R = L + boxW;
    int T = (TOP + HEIGHT) / 2 - 5;
    int B = T + 10;

    /* Limpa a area onde o menu de pausa estava desenhado */
    for (int y = T; y <= B; y++) {
        gotoxy(L, y);
        for (int x = L; x <= R + 1; x++) {
            putchar(' ');
        }
    }
    fflush(stdout);

    /* Redesenha os elementos do jogo que ficam por baixo da pausa */
    draw_board_border();

    for (int y = T; y <= B; y++) {
        for (int lx = 1; lx <= WIDTH; lx++) {
            int col = arena_col(lx);

            if (col >= L && col <= R + 1) {
                erase_cell(lx, y);
            }
        }
    }

    draw_all_obstacles();
    draw_all_fruits();
    draw_full_snake();

    /* Por ultimo, redesenha os overlays do BFS */
    draw_bfs_visited_overlay();
    draw_bfs_path_overlay();
}

/* ============================================================
 * ESTADO INICIAL DO JOGO
 * ============================================================
 */

/* Reinicia todas as variaveis e redesenha a partida do zero */
void init_game(void) {
    /* ========================================================
     * ETAPA 1 - RESET DAS VARIAVEIS GERAIS DA PARTIDA
     * ========================================================
     * Aqui o jogo volta ao estado inicial para comecar uma nova
     * rodada sem carregar dados da partida anterior.
     */
    snakeLen       = 5;
    score          = 0;
    level          = 1;
    speedMs        = INITIAL_SPEED;
    paused         = 0;
    gameOver       = 0;
    fruitsEaten    = 0;
    comboCount     = 0;
    pauseMenuIndex = 0;

    /* ========================================================
     * ETAPA 2 - RESET DE CONTROLE DE TEMPO E DO PAINEL
     * ========================================================
     * Essas variaveis sao reiniciadas para que o painel lateral
     * seja redesenhado corretamente desde o inicio da rodada.
     */
    pausedSeconds = 0;
    lastElapsed   = -1;
    lastLevel     = -1;
    lastScore     = -1;
    lastFruits    = -1;
    lastCombo     = -1;

    /* ========================================================
     * ETAPA 3 - RESET DAS ESTRUTURAS DO BFS
     * ========================================================
     * Como o BFS depende do estado atual do tabuleiro, o caminho
     * e as celulas visitadas precisam ser zerados antes da rodada.
     */
    bfs_path_len      = 0;
    bfs_visited_count = 0;

    /* ========================================================
     * ETAPA 4 - POSICIONAMENTO INICIAL DA COBRA
     * ========================================================
     * A cobra nasce no centro da arena com tamanho inicial 5.
     */
    int mx = WIDTH / 2;
    int my = (TOP + HEIGHT) / 2;

    for (int i = 0; i < snakeLen; i++) {
        snake[i].x = mx - i;
        snake[i].y = my;
    }

    /* A direcao inicial permanece para a direita */
    dir     = DIR_RIGHT;
    nextDir = DIR_RIGHT;

    /* ========================================================
     * ETAPA 5 - LIMPEZA DA TELA E MONTAGEM DO TABULEIRO
     * ========================================================
     * Primeiro limpa a tela, depois monta fundo, bordas, painel
     * e obstaculos. Isso deixa a inicializacao mais organizada.
     */
    clear_screen();
    spawn_obstacles();
    draw_board_border();
    draw_board_background();
    draw_all_obstacles();
    draw_side_panel_frame();

    /* ========================================================
     * ETAPA 6 - DESENHO DOS ELEMENTOS INICIAIS
     * ========================================================
     * Agora o jogo desenha a cobra e gera as frutas iniciais.
     */
    draw_full_snake();

    for (int i = 0; i < FRUIT_COUNT; i++) {
        spawn_fruit(i);
    }

    /* ========================================================
     * ETAPA 7 - CALCULO INICIAL DO BFS E DO PAINEL
     * ========================================================
     * O BFS ja e calculado no comeco para mostrar o primeiro
     * caminho sugerido ou automatico desde o primeiro frame.
     */
    find_nearest_fruit_bfs();
    draw_bfs_visited_overlay();
    draw_bfs_path_overlay();

    /* Marca o horario inicial e desenha os dados do painel */
    startTime = time(NULL);
    update_side_panel();
}

/* ============================================================
 * DIRECAO DA BFS
 * ============================================================
 */

/* Transforma o primeiro passo do caminho em uma direcao */
Direction bfs_next_direction(void) {
    if (bfs_path_len == 0) {
        return dir;
    }

    int nx = bfs_path[0].x;
    int ny = bfs_path[0].y;
    int hx = snake[0].x;
    int hy = snake[0].y;

    int ddx = nx - hx;
    int ddy = ny - hy;

    if (ddx ==  1 && ddy ==  0) return DIR_DOWN;
    if (ddx == -1 && ddy ==  0) return DIR_UP;
    if (ddx ==  0 && ddy ==  1) return DIR_RIGHT;
    if (ddx ==  0 && ddy == -1) return DIR_LEFT;

    ddx = nx - hx;
    ddy = ny - hy;

    if (ddx ==  0 && ddy == -1) return DIR_UP;
    if (ddx ==  1 && ddy ==  0) return DIR_RIGHT;
    if (ddx ==  0 && ddy ==  1) return DIR_DOWN;
    if (ddx == -1 && ddy ==  0) return DIR_LEFT;

    return dir;
}

/* ============================================================
 * ATUALIZACAO DO JOGO
 * ============================================================
 */

/* Atualiza um frame da partida */
void update_game(void) {
    if (paused || gameOver) {
        return;
    }

    clear_bfs_overlay();

    /* No modo automatico, a direcao vem do BFS */
    if (autoMode) {
        find_nearest_fruit_bfs();

        if (bfs_path_len > 0) {
            int nx = bfs_path[0].x;
            int ny = bfs_path[0].y;
            int hx = snake[0].x;
            int hy = snake[0].y;

            int ddx = nx - hx;
            int ddy = ny - hy;

            if      (ddy == -1) nextDir = DIR_UP;
            else if (ddx ==  1) nextDir = DIR_RIGHT;
            else if (ddy ==  1) nextDir = DIR_DOWN;
            else if (ddx == -1) nextDir = DIR_LEFT;
        }
    }

    dir = nextDir;

    int nx = snake[0].x;
    int ny = snake[0].y;

    if (dir == DIR_UP) {
        ny--;
    } else if (dir == DIR_RIGHT) {
        nx++;
    } else if (dir == DIR_DOWN) {
        ny++;
    } else if (dir == DIR_LEFT) {
        nx--;
    }

    /* Colisao com parede */
    if (nx < 1 || nx > WIDTH || ny <= TOP || ny >= HEIGHT) {
        gameOver = 1;
        return;
    }

    /* Colisao com obstaculo */
    if (is_obstacle(nx, ny)) {
        gameOver = 1;
        return;
    }

    int fruitIndex = fruit_at(nx, ny);

    /* Verifica colisao com o corpo */
    if (fruitIndex == -1) {
        for (int i = 0; i < snakeLen - 1; i++) {
            if (snake[i].x == nx && snake[i].y == ny) {
                gameOver = 1;
                return;
            }
        }
    } else {
        for (int i = 0; i < snakeLen; i++) {
            if (snake[i].x == nx && snake[i].y == ny) {
                gameOver = 1;
                return;
            }
        }
    }

    draw_body(snake[0].x, snake[0].y);

    Segment oldTail = snake[snakeLen - 1];

    for (int i = snakeLen; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    snake[0].x = nx;
    snake[0].y = ny;

    /* Se comeu fruta */
    if (fruitIndex != -1) {
        int rarity = fruits[fruitIndex].rarity;

        score += fruits[fruitIndex].value;
        snakeLen++;

        if (snakeLen >= MAX_SNAKE) {
            snakeLen = MAX_SNAKE - 1;
        }

        fruitsEaten++;
        comboCount++;

        int newLevel = score / LEVEL_STEP + 1;
        int didLevel = (newLevel > level);

        if (didLevel) {
            level = newLevel;
            speedMs -= 8;

            if (speedMs < MIN_SPEED) {
                speedMs = MIN_SPEED;
            }
        }

        spawn_fruit(fruitIndex);

        draw_tail_seg(snake[snakeLen - 1].x, snake[snakeLen - 1].y);
        draw_head(nx, ny);

        find_nearest_fruit_bfs();
        draw_bfs_visited_overlay();
        draw_bfs_path_overlay();

        update_side_panel();

        flash_eat(rarity);

        if (didLevel) {
            anim_level_up();
            draw_board_border();
        }

    } else {
        erase_cell(oldTail.x, oldTail.y);
        draw_tail_seg(snake[snakeLen - 1].x, snake[snakeLen - 1].y);
        draw_head(nx, ny);

        find_nearest_fruit_bfs();
        draw_bfs_visited_overlay();
        draw_bfs_path_overlay();

        int elapsed = (int)(time(NULL) - startTime) - pausedSeconds;

        if (elapsed != lastElapsed || lastScore != score) {
            update_side_panel();
        }
    }
}

/* ============================================================
 * TELAS GERAIS
 * ============================================================
 */

/* Arte em ASCII do titulo */
static const char *TITLE_ART[6] = {
    "  _____ _   _          _  _______",
    " / ____| \\ | |   /\\   | |/ /  ___|",
    "| (___ |  \\| |  /  \\  | ' /| |__  ",
    " \\___ \\| . ` | / /\\ \\ |  < |  __| ",
    " ____) | |\\  |/ ____ \\| . \\| |___ ",
    "|_____/|_| \\_/_/    \\_\\_|\\_\\_____|",
};

/* Desenha o titulo centralizado */
static void draw_title(int cx, int ty) {
    int cols[6] = { 10, 10, 11, 11, 10, 10 };

    for (int r = 0; r < 6; r++) {
        int len = (int)strlen(TITLE_ART[r]);

        gotoxy(cx - len / 2, ty + r);
        set_color(cols[r]);
        printf("%s", TITLE_ART[r]);
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* Desenha uma caixa de tela */
static void screen_box(int L, int T, int R, int B, int frame_color, int accent_color) {
    int my = (T + B) / 2;

    set_color(frame_color);

    gotoxy(L, T);
    printf("%s", BOX_TLH);
    for (int x = L + 1; x < R; x++) {
        printf("%s", BOX_DH);
    }
    printf("%s", BOX_TRH);

    gotoxy(L, B);
    printf("%s", BOX_BLH);
    for (int x = L + 1; x < R; x++) {
        printf("%s", BOX_DH);
    }
    printf("%s", BOX_BRH);

    for (int y = T + 1; y < B; y++) {
        if (y == my) {
            set_color(accent_color);
            gotoxy(L, y);
            printf("%s", BOX_LM);
            for (int x = L + 1; x < R; x++) {
                printf("%s", BOX_DH);
            }
            gotoxy(R, y);
            printf("%s", BOX_RM);
        } else {
            set_color(frame_color);
            gotoxy(L, y);
            printf("%s", BOX_DV);
            gotoxy(R, y);
            printf("%s", BOX_DV);
        }
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* Desenha uma separacao horizontal */
static void screen_sep(int L, int y, int R, int color) {
    set_color(color);

    gotoxy(L + 1, y);
    printf("%s", BOX_LM);
    for (int x = L + 2; x < R; x++) {
        printf("%s", BOX_DH);
    }
    printf("%s", BOX_RM);

    set_color(C_RESET);
    fflush(stdout);
}

/* Limpa o interior de uma tela */
static void screen_clear(int L, int T, int R, int B) {
    for (int y = T + 1; y < B; y++) {
        gotoxy(L + 1, y);
        for (int x = L + 1; x < R; x++) {
            putchar(' ');
        }
    }
    fflush(stdout);
}

/* Centraliza um texto em uma tela */
static void screen_center(int L, int R, int y, int color, const char *text) {
    int w = R - L - 1;
    int len = (int)strlen(text);
    int x = L + 1 + (w - len) / 2;

    if (x < L + 1) {
        x = L + 1;
    }

    gotoxy(x, y);
    set_color(color);
    printf("%s", text);
    set_color(C_RESET);
    fflush(stdout);
}

/* Cursor piscando nas telas de menu */
static void screen_cursor(int x, int y, int selected) {
    static DWORD last = 0;
    static int   on   = 1;
    DWORD now = GetTickCount();

    if (now - last > 450) {
        on = !on;
        last = now;
    }

    gotoxy(x, y);

    if (selected && on) {
        set_color(14);
        printf(">");
    } else {
        set_color(0);
        printf(" ");
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* Desenha uma linha decorativa com pontos */
static void screen_dots(int L, int y, int R, int color) {
    set_color(color);
    gotoxy(L + 2, y);

    for (int x = L + 2; x < R - 1; x++) {
        printf((x % 2 == 0) ? "." : " ");
    }

    set_color(C_RESET);
    fflush(stdout);
}

/* ============================================================
 * MENU PRINCIPAL
 * ============================================================
 */

/* Renderiza a tela inicial do jogo */
void render_menu_screen(void) {
    if (menuNeedsRedraw) {
        clear_screen();

        int L = 10, R = 69, T = 1, B = 31;
        int cx = (L + R) / 2;

        /* Moldura principal do menu */
        screen_box(L, T, R, B, 11, 14);
        screen_clear(L, T, R, B);

        /* Titulo em ASCII */
        draw_title(cx, T + 2);

        /* Linhas decorativas e subtitulos */
        screen_dots(L, T + 9, R, 3);
        screen_center(L, R, T + 10, 11, "B F S   E D I T I O N");
        screen_center(L, R, T + 11, 14, "SNAKE ARENA");
        screen_center(L, R, T + 12, 8,  "Projeto de Julia Leal");

        /* Separacao da area de opcoes */
        screen_sep(L, T + 13, R, 11);

        /* Linha decorativa acima do rodape */
        screen_sep(L, B - 3, R, 3);
        screen_center(L, R, B - 2, 8, "W / S   MOVER        ENTER   CONFIRMAR");
        screen_center(L, R, B - 1, 7, "PONTOS, BFS, RANKING E MODO AUTOMATICO");

        menuNeedsRedraw = 0;
        menuLastIndex   = -1;
    }

    {
        int L = 10, R = 69, T = 1;
        int cx = (L + R) / 2;

        for (int i = 0; i < 4; i++) {
            screen_cursor(cx - 18, T + 16 + i * 3, i == menuIndex);
        }
    }

    if (menuLastIndex != menuIndex) {
        int L = 10, R = 69, T = 1;

        const char *labels[] = {
            "JOGAR MANUAL",
            "JOGAR AUTO (BFS)",
            "RANKING",
            "SAIR"
        };

        int hiCol[] = { 10, 11, 14, 12 };

        for (int i = 0; i < 4; i++) {
            int oy = T + 16 + i * 3;

            gotoxy(L + 2, oy);
            for (int x = L + 2; x < R - 1; x++) {
                putchar(' ');
            }

            if (i == menuIndex) {
                char line[48];
                snprintf(line, sizeof(line), "[ %s ]", labels[i]);

                gotoxy(L + 4, oy);
                set_color(hiCol[i]);
                printf("<<<");

                gotoxy(R - 6, oy);
                set_color(hiCol[i]);
                printf(">>>");

                screen_center(L, R, oy, hiCol[i], line);
            } else {
                screen_center(L, R, oy, 8, labels[i]);
            }
        }

        {
            int rs = best_score();
            char rec[48];

            snprintf(rec, sizeof(rec), "HIGH SCORE  %06d", rs);

            gotoxy(L + 2, T + 28);
            for (int x = L + 2; x < R - 1; x++) {
                putchar(' ');
            }

            screen_center(L, R, T + 28, rs > 0 ? 14 : 8, rec);
        }

        menuLastIndex = menuIndex;
    }
}

/* ============================================================
 * RANKING
 * ============================================================
 */

/* Renderiza a tela do ranking */
void render_ranking_screen(void) {
    if (!rankingNeedsRedraw) {
        return;
    }

    clear_screen();

    /* Define os limites da tela de ranking */
    int L = 10, R = 69, T = 2, B = 30;

    /* Desenha a moldura principal e limpa o miolo da tela */
    screen_box(L, T, R, B, 14, 11);
    screen_clear(L, T, R, B);

    /* Cabecalho principal da tela */
    screen_center(L, R, T + 1, 14, "====  H A L L   O F   F A M E  ====");
    screen_center(L, R, T + 2, 8, "MELHORES PONTUACOES DA PARTIDA");
    screen_sep(L, T + 3, R, 14);

    /* Cabecalho das colunas */
    screen_center(L, R, T + 4, 11, "POSICAO        JOGADOR                     PONTOS");
    screen_sep(L, T + 5, R, 8);

    int count = 0;
    load_ranking(&count);

    if (count == 0) {
        /* Mensagem central caso ainda nao exista nenhum score salvo */
        screen_center(L, R, T + 13, 8, "- - -  NENHUM REGISTRO SALVO AINDA  - - -");
        screen_center(L, R, T + 15, 7, "JOGUE UMA PARTIDA PARA CRIAR O PRIMEIRO RANKING");
    } else {
        const char *pos[] = { "1 LUGAR", "2 LUGAR", "3 LUGAR", "4 LUGAR", "5 LUGAR" };
        int posCol[]      = {    14,        11,        13,         7,         7 };

        for (int i = 0; i < count; i++) {
            char line[96];
            int y = T + 7 + i * 3;

            /* Destaca visualmente o primeiro lugar com linhas extras */
            if (i == 0) {
                set_color(14);

                gotoxy(L + 2, y - 1);
                for (int x = L + 2; x < R - 1; x++) {
                    printf("%s", BOX_DH);
                }

                gotoxy(L + 2, y + 1);
                for (int x = L + 2; x < R - 1; x++) {
                    printf("%s", BOX_DH);
                }

                set_color(C_RESET);
                fflush(stdout);
            }

            /* Monta a linha completa com posicao, nome e score */
            snprintf(line, sizeof(line), "%-10s   %-24s   %06d", pos[i], ranking[i].name, ranking[i].score);
            screen_center(L, R, y, posCol[i], line);
        }

        /* Exibe um pequeno resumo no rodape para valorizar o ranking */
        screen_sep(L, B - 4, R, 14);

        if (count > 0) {
            char summary[80];
            snprintf(summary, sizeof(summary), "LIDER ATUAL: %s  |  SCORE: %06d", ranking[0].name, ranking[0].score);
            screen_center(L, R, B - 3, 10, summary);
        }
    }

    /* Rodape com instrucoes */
    screen_sep(L, B - 2, R, 14);
    screen_center(L, R, B - 1, 8, "ENTER / ESC     VOLTAR AO MENU");

    rankingNeedsRedraw = 0;
}

/* ============================================================
 * GAME OVER
 * ============================================================
 */

/* Renderiza a tela de game over */
void render_gameover_screen(void) {
    if (gameoverNeedsRedraw) {
        clear_screen();

        /* Define a area da tela de fim de jogo */
        int L = 10, R = 69, T = 2, B = 31;

        screen_box(L, T, R, B, 12, 14);
        screen_clear(L, T, R, B);

        /* Titulo principal da tela */
        screen_center(L, R, T + 1, 12, "====  G A M E    O V E R  ====");
        screen_center(L, R, T + 2, 8, "RESUMO FINAL DA SUA PARTIDA");
        screen_sep(L, T + 3, R, 12);

        /* Calcula o tempo total jogado descontando o periodo de pausa */
        int elapsed = (int)(pauseStart > startTime ? pauseStart - startTime : time(NULL) - startTime) - pausedSeconds;

        if (elapsed < 0) {
            elapsed = 0;
        }

        char buf[80];

        /* Mostra as estatisticas principais da partida */
        snprintf(buf, sizeof(buf), "SCORE FINAL        >>   %06d", score);
        screen_center(L, R, T + 5, 14, buf);

        snprintf(buf, sizeof(buf), "NIVEL ALCANCADO    >>   %d", level);
        screen_center(L, R, T + 7, 10, buf);

        snprintf(buf, sizeof(buf), "TEMPO SOBREVIVIDO  >>   %02d : %02d", elapsed / 60, elapsed % 60);
        screen_center(L, R, T + 9, 13, buf);

        snprintf(buf, sizeof(buf), "FRUTAS COMIDAS     >>   %d", fruitsEaten);
        screen_center(L, R, T + 11, 11, buf);

        snprintf(buf, sizeof(buf), "MAIOR COMBO        >>   %d x", comboCount);
        screen_center(L, R, T + 13, 12, buf);

        snprintf(buf, sizeof(buf), "TAMANHO DA COBRA   >>   %d", snakeLen);
        screen_center(L, R, T + 15, 10, buf);

        snprintf(buf, sizeof(buf), "MODO DA PARTIDA    >>   %s", autoMode ? "BFS AUTO" : "MANUAL");
        screen_center(L, R, T + 17, autoMode ? 9 : 10, buf);

        /* Informa se a pontuacao bateu o recorde atual */
        if (score > 0 && score >= best_score()) {
            screen_sep(L, T + 19, R, 14);
            screen_center(L, R, T + 20, 14, "* *  N O V O   R E C O R D E  * *");
        } else {
            screen_sep(L, T + 19, R, 8);
            screen_center(L, R, T + 20, 8, "TENTE NOVAMENTE PARA SUPERAR O TOPO DO RANKING");
        }

        /* Rodape da tela */
        screen_sep(L, B - 3, R, 12);
        screen_center(L, R, B - 2, 8, "W / S   MOVER        ENTER   CONFIRMAR");

        gameoverNeedsRedraw = 0;
        gameoverLastIndex   = -1;
    }

    /* Redesenha as opcoes apenas quando a selecao muda */
    if (gameoverLastIndex != gameOverIndex) {
        int L = 10, R = 69, T = 2, B = 31;

        const char *opts[] = {
            "JOGAR  NOVAMENTE",
            "VOLTAR  AO  MENU"
        };

        int hiCol[] = { 10, 8 };

        for (int i = 0; i < 2; i++) {
            int oy = B - 6 + i * 2;

            gotoxy(L + 2, oy);
            for (int x = L + 2; x < R - 1; x++) {
                putchar(' ');
            }

            if (i == gameOverIndex) {
                char line[40];
                snprintf(line, sizeof(line), "[ %s ]", opts[i]);

                gotoxy(L + 3, oy);
                set_color(hiCol[i]);
                printf("<<");

                gotoxy(R - 4, oy);
                set_color(hiCol[i]);
                printf(">>");

                screen_center(L, R, oy, hiCol[i], line);
            } else {
                screen_center(L, R, oy, 8, opts[i]);
            }
        }

        gameoverLastIndex = gameOverIndex;
    }
}

/* ============================================================
 * ENTRADA DO MENU PRINCIPAL
 * ============================================================
 */

/* Processa as teclas na tela inicial */
void input_menu(GameState *state) {
    if (!_kbhit()) {
        return;
    }

    int k = _getch();

    if (k == 0 || k == 224) {
        k = _getch();

        if (k == 72) {
            menuIndex--;
            if (menuIndex < 0) menuIndex = 3;
        } else if (k == 80) {
            menuIndex++;
            if (menuIndex > 3) menuIndex = 0;
        }

        return;
    }

    if (k == 'w' || k == 'W') {
        menuIndex--;
        if (menuIndex < 0) menuIndex = 3;
    } else if (k == 's' || k == 'S') {
        menuIndex++;
        if (menuIndex > 3) menuIndex = 0;
    } else if (k == '1') {
        autoMode = 0;
        init_game();
        menuNeedsRedraw = 1;
        *state = STATE_PLAYING;
    } else if (k == '2') {
        autoMode = 1;
        init_game();
        menuNeedsRedraw = 1;
        *state = STATE_PLAYING;
    } else if (k == '3') {
        rankingNeedsRedraw = 1;
        *state = STATE_RANKING;
    } else if (k == '4' || k == 27) {
        *state = STATE_EXIT;
    } else if (k == 13) {
        if (menuIndex == 0) {
            autoMode = 0;
            init_game();
            menuNeedsRedraw = 1;
            *state = STATE_PLAYING;
        } else if (menuIndex == 1) {
            autoMode = 1;
            init_game();
            menuNeedsRedraw = 1;
            *state = STATE_PLAYING;
        } else if (menuIndex == 2) {
            rankingNeedsRedraw = 1;
            *state = STATE_RANKING;
        } else {
            *state = STATE_EXIT;
        }
    }
}

/* ============================================================
 * ENTRADA DA TELA DE RANKING
 * ============================================================
 */

/* Processa teclas na tela de ranking */
void input_ranking(GameState *state) {
    if (!_kbhit()) {
        return;
    }

    int k = _getch();

    if (k == 13 || k == 27 || k == 'm' || k == 'M') {
        menuNeedsRedraw = 1;
        *state = STATE_MENU;
    }
}

/* ============================================================
 * ENTRADA DA TELA DE PAUSA - MELHORIA ADICIONADA
 * ============================================================
 */

/* Processa teclas quando o jogo esta pausado */
void input_pause_menu(GameState *state) {
    if (!_kbhit()) {
        return;
    }

    int k = _getch();

    if (k == 0 || k == 224) {
        k = _getch();

        if (k == 72) {
            pauseMenuIndex--;
            if (pauseMenuIndex < 0) pauseMenuIndex = 4;
            draw_pause_overlay();
        } else if (k == 80) {
            pauseMenuIndex++;
            if (pauseMenuIndex > 4) pauseMenuIndex = 0;
            draw_pause_overlay();
        }

        return;
    }

    if (k == 'w' || k == 'W') {
        pauseMenuIndex--;
        if (pauseMenuIndex < 0) pauseMenuIndex = 4;
        draw_pause_overlay();
    } else if (k == 's' || k == 'S') {
        pauseMenuIndex++;
        if (pauseMenuIndex > 4) pauseMenuIndex = 0;
        draw_pause_overlay();
    } else if (k == 'p' || k == 'P' || k == 27) {
        paused = 0;
        pausedSeconds += (int)(time(NULL) - pauseStart);
        clear_pause_overlay();
        update_side_panel();
    } else if (k == 13) {
        if (pauseMenuIndex == 0) {
            paused = 0;
            pausedSeconds += (int)(time(NULL) - pauseStart);
            clear_pause_overlay();
            update_side_panel();
        } else if (pauseMenuIndex == 1) {
            autoMode = !autoMode;
            draw_pause_overlay();
            update_side_panel();
        } else if (pauseMenuIndex == 2) {
            showBfsPath = !showBfsPath;
            clear_pause_overlay();
            draw_pause_overlay();
            draw_bfs_visited_overlay();
            draw_bfs_path_overlay();
        } else if (pauseMenuIndex == 3) {
            showBfsVisit = !showBfsVisit;
            clear_pause_overlay();
            draw_pause_overlay();
            draw_bfs_visited_overlay();
            draw_bfs_path_overlay();
        } else if (pauseMenuIndex == 4) {
            paused = 0;
            pausedSeconds += (int)(time(NULL) - pauseStart);
            clear_pause_overlay();
            menuNeedsRedraw = 1;
            *state = STATE_MENU;
        }
    }
}

/* ============================================================
 * ENTRADA DO JOGO
 * ============================================================
 */

/* Processa as teclas da partida */
void input_game(GameState *state) {
    if (paused) {
        input_pause_menu(state);
        return;
    }

    if (!_kbhit()) {
        return;
    }

    int k = _getch();

    if (k == 0 || k == 224) {
        k = _getch();

        if (!autoMode) {
            if (k == 72 && dir != DIR_DOWN) {
                nextDir = DIR_UP;
            } else if (k == 77 && dir != DIR_LEFT) {
                nextDir = DIR_RIGHT;
            } else if (k == 80 && dir != DIR_UP) {
                nextDir = DIR_DOWN;
            } else if (k == 75 && dir != DIR_RIGHT) {
                nextDir = DIR_LEFT;
            }
        }

        return;
    }

    if (!autoMode) {
        if ((k == 'w' || k == 'W') && dir != DIR_DOWN) {
            nextDir = DIR_UP;
        } else if ((k == 'd' || k == 'D') && dir != DIR_LEFT) {
            nextDir = DIR_RIGHT;
        } else if ((k == 's' || k == 'S') && dir != DIR_UP) {
            nextDir = DIR_DOWN;
        } else if ((k == 'a' || k == 'A') && dir != DIR_RIGHT) {
            nextDir = DIR_LEFT;
        }
    }

    if (k == 'p' || k == 'P') {
        paused = 1;
        pauseStart = time(NULL);
        pauseMenuIndex = 0;
        update_side_panel();
        draw_pause_overlay();
    } else if (k == 'q' || k == 'Q') {
        menuNeedsRedraw = 1;
        *state = STATE_MENU;
    } else if (k == 'v' || k == 'V') {
        showBfsPath = !showBfsPath;

        if (!showBfsPath) {
            for (int i = 0; i < bfs_path_len; i++) {
                int px = bfs_path[i].x;
                int py = bfs_path[i].y;

                if (!is_on_snake(px, py) && fruit_at(px, py) < 0 && !is_obstacle(px, py)) {
                    erase_cell(px, py);
                }
            }
        } else {
            draw_bfs_path_overlay();
        }
    } else if (k == 'g' || k == 'G') {
        showBfsVisit = !showBfsVisit;

        if (!showBfsVisit) {
            clear_bfs_overlay();
            draw_all_obstacles();

            for (int i = 0; i < FRUIT_COUNT; i++) {
                draw_fruit(i);
            }

            for (int i = snakeLen - 1; i >= 0; i--) {
                if (i == 0) {
                    draw_head(snake[i].x, snake[i].y);
                } else {
                    draw_body(snake[i].x, snake[i].y);
                }
            }

            if (showBfsPath) {
                draw_bfs_path_overlay();
            }
        } else {
            draw_bfs_visited_overlay();
        }
    } else if (k == 't' || k == 'T') {
        autoMode = !autoMode;
        update_side_panel();
    }
}

/* ============================================================
 * ENTRADA DO GAME OVER
 * ============================================================
 */

/* Processa teclas na tela final */
void input_gameover(GameState *state) {
    if (!_kbhit()) {
        return;
    }

    int k = _getch();

    if (k == 0 || k == 224) {
        k = _getch();

        if (k == 72) {
            gameOverIndex = 0;
        } else if (k == 80) {
            gameOverIndex = 1;
        }

        return;
    }

    if (k == 'w' || k == 'W') {
        gameOverIndex = 0;
    } else if (k == 's' || k == 'S') {
        gameOverIndex = 1;
    } else if (k == '1') {
        init_game();
        gameoverNeedsRedraw = 1;
        menuNeedsRedraw = 1;
        *state = STATE_PLAYING;
    } else if (k == '2') {
        menuNeedsRedraw = 1;
        gameoverNeedsRedraw = 1;
        *state = STATE_MENU;
    } else if (k == 13) {
        if (gameOverIndex == 0) {
            init_game();
            gameoverNeedsRedraw = 1;
            menuNeedsRedraw = 1;
            *state = STATE_PLAYING;
        } else {
            menuNeedsRedraw = 1;
            gameoverNeedsRedraw = 1;
            *state = STATE_MENU;
        }
    }
}

/* ============================================================
 * FUNCAO PRINCIPAL
 * ============================================================
 * Controla a troca entre as telas e o loop principal.
 */

int main(void) {
    /* Ativa UTF-8 no console do Windows */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    /* Pega o handle do console */
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) {
        printf("Erro ao obter o console.\n");
        return 1;
    }

    /* Ajusta tamanho do buffer */
    COORD bufSize = { (SHORT)CON_COLS, (SHORT)CON_ROWS };
    SetConsoleScreenBufferSize(hConsole, bufSize);

    /* Ajusta tamanho da janela */
    SMALL_RECT winRect = { 0, 0, (SHORT)(CON_COLS - 1), (SHORT)(CON_ROWS - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &winRect);

    /* Define o titulo da janela */
    SetConsoleTitleA("SNAKE BFS - Cobra Inteligente");

    /* Inicializa aleatoriedade e cursor */
    srand((unsigned int)time(NULL));
    hide_cursor();

    GameState state = STATE_MENU;

    /* Loop principal do programa */
    while (state != STATE_EXIT) {

        if (state == STATE_MENU) {
            render_menu_screen();
            input_menu(&state);
            Sleep(30);
        }

        else if (state == STATE_RANKING) {
            render_ranking_screen();
            input_ranking(&state);
            Sleep(50);
        }

        else if (state == STATE_PLAYING) {
            input_game(&state);

            if (state != STATE_PLAYING) {
                continue;
            }

            if (!paused) {
                update_game();

                if (gameOver) {
                    anim_death();

                    char name[32];
                    show_cursor();

                    gotoxy(0, HEIGHT + 2);
                    set_color(C_TITLE);
                    printf("  Digite seu nome: ");
                    fflush(stdout);
                    set_color(C_PANEL_VAL);

                    if (fgets(name, sizeof(name), stdin) != NULL) {
                        trim_newline(name);
                    } else {
                        strcpy(name, "Jogador");
                    }

                    if (strlen(name) == 0) {
                        strcpy(name, "Jogador");
                    }

                    insert_ranking(name, score);
                    hide_cursor();

                    gameOverIndex       = 0;
                    gameoverNeedsRedraw = 1;
                    state = STATE_GAMEOVER;

                } else {
                    int sp = autoMode ? (speedMs > 80 ? 80 : speedMs) : speedMs;
                    Sleep(sp);
                }
            } else {
                Sleep(50);
            }
        }

        else if (state == STATE_GAMEOVER) {
            render_gameover_screen();
            input_gameover(&state);
            Sleep(50);
        }
    }

    show_cursor();
    clear_screen();
    set_color(C_RESET);
    return 0;
}

