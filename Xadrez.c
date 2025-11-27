#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h> // Para fmin/fmax.

//  Cores para o terminal 
#define RESET                 "\x1b[0m"
#define BRIGHT_BLACK          "\x1b[90m" // Usado para bordas ou texto secundário
#define BRIGHT_RED            "\x1b[91m" // Para erros
#define BRIGHT_GREEN          "\x1b[92m" // Para sucesso
#define BRIGHT_YELLOW         "\x1b[93m" // Para avisos/informações importantes
#define BRIGHT_BLUE           "\x1b[94m" // Para prompts de entrada
#define BRIGHT_MAGENTA        "\x1b[95m" // Para peças pretas ou destaque
#define BRIGHT_CYAN           "\x1b[96m" // Para peças brancas ou destaque
#define WHITE                 "\x1b[97m" // Texto branco puro

// Cores de fundo para o tabuleiro
#define BOARD_LIGHT_SQUARE    "\x1b[48;5;22m" 
#define BOARD_DARK_SQUARE     "\x1b[48;5;236m" 

#define NEGRITO               "\x1b[1m"

// === Definições ===
#define MAX_USUARIOS 100

// Estruturas
typedef struct {
    char sigla;
    char cor; // 'B' (Brancas) ou 'P' (Pretas)
    
} Peca;

typedef struct {
    Peca* peca;
} Casa;

// Definição da struct Usuario movida para antes de seu uso global.
typedef struct {
    char nome[32];
    char senha[32];
    int vitorias;
    int derrotas;
    int empates;
} Usuario;

// Variáveis Globais
Casa* tabuleiro[8][8];
char turno = 'B'; 
Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;
char jogadorAtual[32] = ""; // Nome do jogador logado
int usuarioLogado = -1;     // Indice do usuario logado no array de usuarios

// Para pecas capturadas
char pecasCapturadasBrancas[16]; // Siglas das pecas brancas capturadas
int totalPecasCapturadasBrancas = 0;
char pecasCapturadasPretas[16];  // Siglas das pecas pretas capturadas
int totalPecasCapturadasPretas = 0;


// --- Protótipos de funções ---
void inicializarTabuleiro();
const char* simboloPeca(char sigla, char cor); 
void mostrarTabuleiro();
void liberarTabuleiro();
void verificarTabuleiropIntegridade(); 
int ehMovimentoValido(int ox, int oy, int dx, int dy);
int ehMovimentoValidoPeao(int ox, int oy, int dx, int dy, char cor);
int ehMovimentoValidoTorre(int ox, int oy, int dx, int dy);
int ehMovimentoValidoCavalo(int ox, int oy, int dx, int dy);
int ehMovimentoValidoBispo(int ox, int oy, int dx, int dy);
int ehMovimentoValidoDama(int ox, int oy, int dx, int dy);
int ehMovimentoValidoRei(int ox, int oy, int dx, int dy);
int movimentoResultaEmXeque(int ox, int oy, int dx, int dy, char corDoRei);
int casaEstaAtacada(int tx, int ty, char corAtacante);
int estaEmXeque(char corDoRei);
int temMovimentoLegal(char cor);
int estaEmXequeMate(char corDoRei);
int estaAfogado(char corDoRei);
void promoverPeao(int x, int y, char cor);
void jogar();
void jogarContraBot();
int fazerMovimentoBot(char corDoBot);
void menuPrincipal();
void cadastrarUsuario();
void loginUsuario();
void salvarRanking();
void carregarRanking();
void mostrarRanking();
void atualizarRanking(char resultado); // 'V' vitoria, 'D' derrota, 'E' empate
void salvarPartida();
void continuarPartida();
void limparHistoricoCapturas();


// === Implementação das Funções ===

void inicializarTabuleiro() {
    
    liberarTabuleiro(); // Garante que o tabuleiro esta limpo antes de alocar

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tabuleiro[i][j] = (Casa*)malloc(sizeof(Casa));
            if (tabuleiro[i][j] == NULL) {
                fprintf(stderr, BRIGHT_RED "Erro de alocacao de memoria para Casa no tabuleiro. Encerrando.\n" RESET);
                exit(EXIT_FAILURE);
            }
            tabuleiro[i][j]->peca = NULL; // Garante que a casa comeca vazia
        }
    }

    // Peoes
    for (int j = 0; j < 8; j++) {
        tabuleiro[1][j]->peca = (Peca*)malloc(sizeof(Peca));
        if (tabuleiro[1][j]->peca == NULL) exit(EXIT_FAILURE);
        tabuleiro[1][j]->peca->sigla = 'P';
        tabuleiro[1][j]->peca->cor = 'P'; // Pretas

        tabuleiro[6][j]->peca = (Peca*)malloc(sizeof(Peca));
        if (tabuleiro[6][j]->peca == NULL) exit(EXIT_FAILURE);
        tabuleiro[6][j]->peca->sigla = 'P';
        tabuleiro[6][j]->peca->cor = 'B'; // Brancas
    }

    // Torres
    tabuleiro[0][0]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][0]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][0]->peca->sigla = 'T'; tabuleiro[0][0]->peca->cor = 'P';
    tabuleiro[0][7]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][7]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][7]->peca->sigla = 'T'; tabuleiro[0][7]->peca->cor = 'P';

    tabuleiro[7][0]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][0]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][0]->peca->sigla = 'T'; tabuleiro[7][0]->peca->cor = 'B';
    tabuleiro[7][7]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][7]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][7]->peca->sigla = 'T'; tabuleiro[7][7]->peca->cor = 'B';

    // Cavalos
    tabuleiro[0][1]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][1]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][1]->peca->sigla = 'C'; tabuleiro[0][1]->peca->cor = 'P';
    tabuleiro[0][6]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][6]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][6]->peca->sigla = 'C'; tabuleiro[0][6]->peca->cor = 'P';

    tabuleiro[7][1]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][1]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][1]->peca->sigla = 'C'; tabuleiro[7][1]->peca->cor = 'B';
    tabuleiro[7][6]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][6]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][6]->peca->sigla = 'C'; tabuleiro[7][6]->peca->cor = 'B';

    // Bispos
    tabuleiro[0][2]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][2]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][2]->peca->sigla = 'B'; tabuleiro[0][2]->peca->cor = 'P';
    tabuleiro[0][5]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][5]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][5]->peca->sigla = 'B'; tabuleiro[0][5]->peca->cor = 'P';

    tabuleiro[7][2]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][2]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][2]->peca->sigla = 'B'; tabuleiro[7][2]->peca->cor = 'B';
    tabuleiro[7][5]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][5]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][5]->peca->sigla = 'B'; tabuleiro[7][5]->peca->cor = 'B';

    // Damas
    tabuleiro[0][3]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][3]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][3]->peca->sigla = 'D'; tabuleiro[0][3]->peca->cor = 'P';

    tabuleiro[7][3]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][3]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][3]->peca->sigla = 'D'; tabuleiro[7][3]->peca->cor = 'B';

    // Reis
    tabuleiro[0][4]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[0][4]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[0][4]->peca->sigla = 'R'; tabuleiro[0][4]->peca->cor = 'P';

    tabuleiro[7][4]->peca = (Peca*)malloc(sizeof(Peca)); if (tabuleiro[7][4]->peca == NULL) exit(EXIT_FAILURE);
    tabuleiro[7][4]->peca->sigla = 'R'; tabuleiro[7][4]->peca->cor = 'B';

    limparHistoricoCapturas(); // Zera o historico de pecas capturadas
    turno = 'B'; // Reseta o turno
}

void limparHistoricoCapturas() {
    totalPecasCapturadasBrancas = 0;
    totalPecasCapturadasPretas = 0;
}

// Funcao auxiliar para retornar o simbolo ASCII simples da peca
const char* simboloPeca(char sigla, char cor) {
    if (cor == 'B') { // Pecas Brancas
        switch (sigla) {
            case 'P': return "p"; // Peao Branco
            case 'T': return "T"; // Torre Branca
            case 'C': return "C"; // Cavalo Branco
            case 'B': return "B"; // Bispo Branco
            case 'D': return "D"; // Dama Branca
            case 'R': return "R"; // Rei Branco
        }
    } else { // Pecas Pretas
        switch (sigla) {
            case 'P': return "P"; // Peao Preto (maiúscula para diferenciar do peao branco minúsculo)
            case 'T': return "t"; // Torre Preta
            case 'C': return "c"; // Cavalo Preto
            case 'B': return "b"; // Bispo Preto
            case 'D': return "d"; // Dama Preta
            case 'R': return "r"; // Rei Preto
        }
    }
    return " "; // Retorna espaco se a sigla nao for reconhecida
}

// Funcao para exibir o tabuleiro
void mostrarTabuleiro() {
    system("cls || clear");

    printf(NEGRITO "PECAS CAPTURADAS:\n" RESET);

    printf(BRIGHT_CYAN "Brancas (Pecas Brancas): " RESET);
    for (int i = 0; i < totalPecasCapturadasBrancas; i++) {
        // As peças capturadas brancas podem aparecer com o texto branco padrão
        printf("%s%s " RESET, BRIGHT_CYAN, simboloPeca(pecasCapturadasBrancas[i], 'B'));
    }
    printf("\n");

    printf(BRIGHT_MAGENTA "Pretas  (Pecas Pretas):  " RESET);
    for (int i = 0; i < totalPecasCapturadasPretas; i++) {
        // As peças capturadas pretas podem aparecer com o texto azul claro
        printf("%s%s " RESET, BRIGHT_MAGENTA, simboloPeca(pecasCapturadasPretas[i], 'P'));
    }
    printf("\n\n");

    printf(BRIGHT_BLACK "   A  B  C  D  E  F  G  H\n" RESET);
    printf(BRIGHT_BLACK "  +------------------------+\n" RESET);
    for (int i = 0; i < 8; i++) {
        printf(BRIGHT_BLACK "%d " RESET "|", 8 - i);
        for (int j = 0; j < 8; j++) {
            // Determina a cor de fundo com base na posição (i, j)
            if ((i + j) % 2 == 0) {
                printf(BOARD_LIGHT_SQUARE); // Fundo para quadrados "claros"
            } else {
                printf(BOARD_DARK_SQUARE); // Fundo para quadrados "escuros"
            }

            if (tabuleiro[i][j] == NULL || tabuleiro[i][j]->peca == NULL) {
                printf("   "); 
            } else {
                char sigla = tabuleiro[i][j]->peca->sigla;
                char cor = tabuleiro[i][j]->peca->cor;
                
                if (cor == 'B') {
                    printf(BRIGHT_CYAN " %s " RESET, simboloPeca(sigla, cor)); 
                } else {
                    printf(BRIGHT_MAGENTA " %s " RESET, simboloPeca(sigla, cor)); 
                }
            }
            printf(RESET); // Reseta a cor de fundo e texto após cada casa
        }
        printf(BRIGHT_BLACK " | %d\n" RESET, 8 - i); 
    }
    printf(BRIGHT_BLACK "  +------------------------+\n" RESET);


    printf("\n" NEGRITO "Turno: %s\n" RESET, (turno == 'B') ? (BRIGHT_CYAN "Brancas" RESET) : (BRIGHT_MAGENTA "Pretas" RESET));
    printf(NEGRITO "Jogador atual: %s\n" RESET, jogadorAtual);
}


// Libera a memoria alocada para o tabuleiro
void liberarTabuleiro() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tabuleiro[i][j] != NULL) {
                if (tabuleiro[i][j]->peca != NULL) {
                    free(tabuleiro[i][j]->peca);
                    tabuleiro[i][j]->peca = NULL; // Garante que o ponteiro e nulo apos free
                }
                free(tabuleiro[i][j]);
                tabuleiro[i][j] = NULL; // Garante que o ponteiro e nulo apos free
            }
        }
    }
}

// Funcao de depuracao para verificar a integridade do tabuleiro
void verificarTabuleiropIntegridade() {
    printf(NEGRITO "\n--- Verificacao de Integridade do Tabuleiro ---\n" RESET);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tabuleiro[i][j] == NULL) {
                printf(BRIGHT_RED "ERRO CRITICO: tabuleiro[%d][%d] e NULL!\n" RESET, i, j);
                // exit(EXIT_FAILURE); // Pode remover ou manter, dependendo do quao critico vc considera
            } else {
                
            }
        }
    }
    printf(NEGRITO "--- Fim da Verificacao ---\n\n" RESET);
}


// === Validacoes de Movimento ===

// Valida se o movimento esta dentro dos limites do tabuleiro
// e se a casa de destino nao esta ocupada por uma peca da mesma cor
int ehMovimentoValido(int ox, int oy, int dx, int dy) {
    if (ox < 0 || ox > 7 || oy < 0 || oy > 7 ||
        dx < 0 || dx > 7 || dy < 0 || dy > 7) {
        return 0;
    }

    if (tabuleiro[ox][oy] == NULL || tabuleiro[ox][oy]->peca == NULL) { // Se nao tem peca na origem
        return 0;
    }

    Peca* pecaOrigem = tabuleiro[ox][oy]->peca;
    Peca* pecaDestino = tabuleiro[dx][dy]->peca;

    if (pecaDestino != NULL && pecaDestino->cor == pecaOrigem->cor) {
        return 0; // Nao pode capturar peca da mesma cor
    }

    // Chama a funcao de validacao especifica para cada tipo de peca
    switch (pecaOrigem->sigla) {
        case 'P': return ehMovimentoValidoPeao(ox, oy, dx, dy, pecaOrigem->cor);
        case 'T': return ehMovimentoValidoTorre(ox, oy, dx, dy);
        case 'C': return ehMovimentoValidoCavalo(ox, oy, dx, dy);
        case 'B': return ehMovimentoValidoBispo(ox, oy, dx, dy);
        case 'D': return ehMovimentoValidoDama(ox, oy, dx, dy);
        case 'R': return ehMovimentoValidoRei(ox, oy, dx, dy);
        default: return 0;
    }
}

int ehMovimentoValidoPeao(int ox, int oy, int dx, int dy, char cor) {
    int diffX = dx - ox;
    int diffY = dy - oy;

    if (cor == 'B') { // Peoes brancos movem de baixo para cima (linhas 6 para 0)
        // Movimento para frente
        if (diffY == 0) { // Mesma coluna
            if (diffX == -1 && tabuleiro[dx][dy]->peca == NULL) { // Um passo para frente
                return 1;
            }
            if (ox == 6 && diffX == -2 && tabuleiro[dx][dy]->peca == NULL && tabuleiro[ox - 1][oy]->peca == NULL) { // Dois passos na primeira jogada
                return 1;
            }
        }
        // Captura
        if (abs(diffY) == 1 && diffX == -1) { // Diagonal
            if (tabuleiro[dx][dy]->peca != NULL && tabuleiro[dx][dy]->peca->cor != cor) {
                return 1;
            }
            // TODO: Implementar En Passant (Captura "em passagem")
        }
    } else { // Peoes pretos movem de cima para baixo (linhas 1 para 7)
        // Movimento para frente
        if (diffY == 0) { // Mesma coluna
            if (diffX == 1 && tabuleiro[dx][dy]->peca == NULL) { // Um passo para frente
                return 1;
            }
            if (ox == 1 && diffX == 2 && tabuleiro[dx][dy]->peca == NULL && tabuleiro[ox + 1][oy]->peca == NULL) { // Dois passos na primeira jogada
                return 1;
            }
        }
        // Captura
        if (abs(diffY) == 1 && diffX == 1) { // Diagonal
            if (tabuleiro[dx][dy]->peca != NULL && tabuleiro[dx][dy]->peca->cor != cor) {
                return 1;
            }
            // TODO: Implementar En Passant (Captura "em passagem")
        }
    }
    return 0;
}

int ehMovimentoValidoTorre(int ox, int oy, int dx, int dy) {
    // Movimento horizontal ou vertical
    if (ox != dx && oy != dy) {
        return 0; // Nao e movimento de torre (nem horizontal, nem vertical)
    }

    // Verifica se ha pecas no caminho
    if (ox == dx) { // Movimento vertical (muda apenas a coluna Y)
        int min_y = fmin(oy, dy);
        int max_y = fmax(oy, dy);
        for (int i = min_y + 1; i < max_y; i++) {
            if (tabuleiro[ox][i]->peca != NULL) return 0; // Caminho bloqueado
        }
    } else { // Movimento horizontal (muda apenas a linha X)
        int min_x = fmin(ox, dx);
        int max_x = fmax(ox, dx);
        for (int i = min_x + 1; i < max_x; i++) {
            if (tabuleiro[i][oy]->peca != NULL) return 0; // Caminho bloqueado
        }
    }
    return 1;
}

int ehMovimentoValidoCavalo(int ox, int oy, int dx, int dy) {
    int diffX = abs(dx - ox);
    int diffY = abs(dy - oy);
    // Movimento em "L" (2 casas em uma direcao, 1 em outra, ou vice-versa)
    return (diffX == 1 && diffY == 2) || (diffX == 2 && diffY == 1);
}

int ehMovimentoValidoBispo(int ox, int oy, int dx, int dy) {
    // Movimento diagonal (abs(diffX) == abs(diffY))
    int diffX = abs(dx - ox);
    int diffY = abs(dy - oy);

    if (diffX != diffY || diffX == 0) {
        return 0; // Nao e movimento diagonal ou nao se moveu
    }

    // Verifica se ha pecas no caminho
    int stepX = (dx > ox) ? 1 : -1;
    int stepY = (dy > oy) ? 1 : -1;

    for (int i = 1; i < diffX; i++) {
        if (tabuleiro[ox + i * stepX][oy + i * stepY]->peca != NULL) {
            return 0; // Caminho bloqueado
        }
    }
    return 1;
}

int ehMovimentoValidoDama(int ox, int oy, int dx, int dy) {
    // A dama move-se como uma torre OU como um bispo
    return ehMovimentoValidoTorre(ox, oy, dx, dy) || ehMovimentoValidoBispo(ox, oy, dx, dy);
}

int ehMovimentoValidoRei(int ox, int oy, int dx, int dy) {
    int diffX = abs(dx - ox);
    int diffY = abs(dy - oy);
    // Move uma casa em qualquer direcao
    // TODO: Implementar Roque (Castling)
    return (diffX <= 1 && diffY <= 1) && (diffX != 0 || diffY != 0);
}

// Verifica se, ao realizar um movimento, o rei da cor especificada entra em xeque
int movimentoResultaEmXeque(int ox, int oy, int dx, int dy, char corDoRei) {
    // Salva o estado atual das pecas para simulacao
    Peca* pecaOrigem = tabuleiro[ox][oy]->peca;
    Peca* pecaDestinoTemp = tabuleiro[dx][dy]->peca; // Peca no destino, se houver

    // Simula o movimento
    tabuleiro[dx][dy]->peca = pecaOrigem;
    tabuleiro[ox][oy]->peca = NULL;

    // Encontra a posicao do rei apos o movimento simulado
    int reiX = -1, reiY = -1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tabuleiro[i][j]->peca != NULL &&
                tabuleiro[i][j]->peca->sigla == 'R' &&
                tabuleiro[i][j]->peca->cor == corDoRei) {
                reiX = i;
                reiY = j;
                break;
            }
        }
        if (reiX != -1) break;
    }

    // Verifica se o rei esta em xeque apos o movimento simulado
    int emXeque = casaEstaAtacada(reiX, reiY, (corDoRei == 'B' ? 'P' : 'B')); // Verifica se a casa do rei e atacada pela cor oposta

    // Desfaz o movimento simulado
    tabuleiro[ox][oy]->peca = pecaOrigem;
    tabuleiro[dx][dy]->peca = pecaDestinoTemp;

    return emXeque;
}

// Verifica se uma determinada casa (tx, ty) esta sendo atacada por pecas da corAtacante
int casaEstaAtacada(int tx, int ty, char corAtacante) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tabuleiro[i][j]->peca != NULL && tabuleiro[i][j]->peca->cor == corAtacante) {
                // Simula temporariamente uma peca no destino para testar ehMovimentoValido
                // Isso e importante porque ehMovimentoValido() verifica se o destino esta ocupado
                // por uma peca da MESMA cor, o que nao se aplica ao teste de ataque.
                Peca* pecaTemporariaNoDestino = tabuleiro[tx][ty]->peca; // Salva a peca que esta la
                tabuleiro[tx][ty]->peca = NULL; // Temporariamente esvazia a casa para simular o ataque

                // Se a peca atacante puder fazer um movimento valido para a casa do rei (tx,ty)
                // Usamos as coordenadas (i,j) da peca atacante e (tx,ty) do rei.
                // A unica excecao e o peao, que ataca de forma diferente de como move para casas vazias.
                if (tabuleiro[i][j]->peca->sigla == 'P') {
                    int diffX = tx - i;
                    int diffY = ty - j;
                    if (tabuleiro[i][j]->peca->cor == 'B') { // Peao branco ataca para cima
                        if (diffX == -1 && abs(diffY) == 1) {
                            tabuleiro[tx][ty]->peca = pecaTemporariaNoDestino; // Restaura
                            return 1;
                        }
                    } else { // Peao preto ataca para baixo
                        if (diffX == 1 && abs(diffY) == 1) {
                            tabuleiro[tx][ty]->peca = pecaTemporariaNoDestino; // Restaura
                            return 1;
                        }
                    }
                } else if (ehMovimentoValido(i, j, tx, ty)) {
                    tabuleiro[tx][ty]->peca = pecaTemporariaNoDestino; // Restaura
                    return 1;
                }
                tabuleiro[tx][ty]->peca = pecaTemporariaNoDestino; // Restaura sempre
            }
        }
    }
    return 0;
}


// Verifica se o rei da cor especificada esta em xeque
int estaEmXeque(char corDoRei) {
    int reiX = -1, reiY = -1;

    // Encontra a posicao do rei
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tabuleiro[i][j]->peca != NULL &&
                tabuleiro[i][j]->peca->sigla == 'R' &&
                tabuleiro[i][j]->peca->cor == corDoRei) {
                reiX = i;
                reiY = j;
                break;
            }
        }
        if (reiX != -1) break;
    }

    if (reiX == -1) { // Rei nao encontrado (situacao anomala)
        return 0;
    }

    char corOponente = (corDoRei == 'B') ? 'P' : 'B';
    return casaEstaAtacada(reiX, reiY, corOponente);
}

// Verifica se ha algum movimento legal disponivel para a cor especificada
int temMovimentoLegal(char cor) {
    for (int ox = 0; ox < 8; ox++) {
        for (int oy = 0; oy < 8; oy++) {
            if (tabuleiro[ox][oy] != NULL && tabuleiro[ox][oy]->peca != NULL && tabuleiro[ox][oy]->peca->cor == cor) {
                for (int dx = 0; dx < 8; dx++) {
                    for (int dy = 0; dy < 8; dy++) {
                        if (ehMovimentoValido(ox, oy, dx, dy)) {
                            // Se o movimento nao resulta em xeque para o proprio rei, e um movimento legal
                            if (!movimentoResultaEmXeque(ox, oy, dx, dy, cor)) {
                                return 1; // Encontrou um movimento legal
                            }
                        }
                    }
                }
            }
        }
    }
    return 0; // Nenhum movimento legal encontrado
}

// Verifica se a cor do rei esta em xeque-mate
int estaEmXequeMate(char corDoRei) {
    // Deve estar em xeque E nao ter movimentos legais
    return estaEmXeque(corDoRei) && !temMovimentoLegal(corDoRei);
}

// Verifica se a cor do rei esta afogada (pat)
int estaAfogado(char corDoRei) {
    // Nao deve estar em xeque E nao ter movimentos legais
    return !estaEmXeque(corDoRei) && !temMovimentoLegal(corDoRei);
}


// Funcao para promover um peao
void promoverPeao(int x, int y, char cor) {
    char escolha;
    if (strcmp(jogadorAtual, "Bot") == 0) { // Se for o bot, ele sempre promove para Dama
        escolha = 'D';
        printf(BRIGHT_YELLOW "Bot promoveu um peao para Dama!\n" RESET);
    } else {
        do {
            printf(BRIGHT_YELLOW "Seu peao (%s) chegou a ultima fileira! Escolha a promocao (D/T/C/B): " RESET, simboloPeca(tabuleiro[x][y]->peca->sigla, cor));
            // Consome o newline pendente e le a escolha
            int c; while ((c = getchar()) != '\n' && c != EOF);
            scanf(" %c", &escolha); // Espaco antes de %c para consumir qualquer whitespace restante
            escolha = toupper(escolha);
        } while (escolha != 'D' && escolha != 'T' && escolha != 'C' && escolha != 'B');
        printf(BRIGHT_YELLOW "Peao promovido para %c!\n" RESET, escolha);
    }
    tabuleiro[x][y]->peca->sigla = escolha;
}


// === Funcoes de Jogo ===

void jogar() {
    int ox, oy, dx, dy;
    char colunaOrigem, colunaDestino;
    int linhaOrigem, linhaDestino;
    char resultadoPartida = 'N'; // 'N' - Nao terminou, 'V' - Vitoria, 'D' - Derrota, 'E' - Empate

    inicializarTabuleiro(); // Reinicia o tabuleiro para uma nova partida
    verificarTabuleiropIntegridade(); // Adicionado para depuração
    mostrarTabuleiro();

    while (resultadoPartida == 'N') {
        printf(BRIGHT_BLUE "Digite o movimento (ex: e2 e4, ou 'salvar' para salvar e sair, 'sair' para sair): " RESET);
        char input[10];
        // Usa scanf com %s para ler a primeira parte, mas com cuidado para o buffer
        if (scanf("%9s", input) != 1) { // Limita a 9 caracteres para evitar overflow
            printf(BRIGHT_RED "Entrada inválida. Tente novamente.\n" RESET);
            int c; while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer do teclado
            continue;
        }

        if (strcmp(input, "salvar") == 0) {
            salvarPartida();
            printf(BRIGHT_GREEN "Partida salva com sucesso. Saindo do jogo.\n" RESET);
            resultadoPartida = 'S'; // Marcador para sair do loop
            break;
        }
        if (strcmp(input, "sair") == 0) {
            printf(BRIGHT_YELLOW "Saindo do jogo.\n" RESET);
            resultadoPartida = 'X'; // Marcador para sair do loop
            break;
        }

        // Tenta ler a segunda parte do movimento (destino)
        char input2[10];
        if (scanf("%9s", input2) != 1) { // Limita a 9 caracteres
            printf(BRIGHT_RED "Entrada inválida. Tente novamente.\n" RESET);
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        // Converte as coordenadas
        colunaOrigem = tolower(input[0]);
        linhaOrigem = atoi(&input[1]);
        colunaDestino = tolower(input2[0]);
        linhaDestino = atoi(&input2[1]);

        ox = 8 - linhaOrigem;
        oy = colunaOrigem - 'a';
        dx = 8 - linhaDestino;
        dy = colunaDestino - 'a';

        // --- Adicionado para depuração: Mostra as coordenadas internas ---
        printf(BRIGHT_BLUE "DEBUG: Origem (%c%d) -> (%d, %d); Destino (%c%d) -> (%d, %d)\n" RESET,
               colunaOrigem, linhaOrigem, ox, oy, colunaDestino, linhaDestino, dx, dy);
        // --- Fim da depuração ---


        // Validar coordenadas e turno
        if (ox < 0 || ox > 7 || oy < 0 || oy > 7 || dx < 0 || dx > 7 || dy < 0 || dy > 7) {
            printf(BRIGHT_RED "Coordenadas inválidas. Por favor, insira coordenadas dentro do tabuleiro (e.g., a1 a2).\n" RESET);
            continue;
        }
        // Adicionado: verifica se a casa de origem é NULL antes de acessar peca
        if (tabuleiro[ox][oy] == NULL || tabuleiro[ox][oy]->peca == NULL || tabuleiro[ox][oy]->peca->cor != turno) {
            printf(BRIGHT_RED "Jogada inválida: Não há sua peça na origem ou não é seu turno. Tente novamente.\n" RESET);
            continue;
        }

        // Valida o movimento e verifica se resulta em xeque no proprio rei
        if (ehMovimentoValido(ox, oy, dx, dy)) {
            if (!movimentoResultaEmXeque(ox, oy, dx, dy, turno)) {
                // Salva a peca capturada (se houver) antes de move-la
                if (tabuleiro[dx][dy]->peca != NULL) {
                    if (tabuleiro[dx][dy]->peca->cor == 'B') { // Peca branca capturada
                        if (totalPecasCapturadasBrancas < 16) {
                            pecasCapturadasBrancas[totalPecasCapturadasBrancas++] = tabuleiro[dx][dy]->peca->sigla;
                        }
                    } else { // Peca preta capturada
                        if (totalPecasCapturadasPretas < 16) {
                            pecasCapturadasPretas[totalPecasCapturadasPretas++] = tabuleiro[dx][dy]->peca->sigla;
                        }
                    }
                    free(tabuleiro[dx][dy]->peca); // Libera a memoria da peca capturada
                    tabuleiro[dx][dy]->peca = NULL; // Garante que o ponteiro e nulo
                }

                // Move a peca (apenas manipulando ponteiros de Peca*)
                Peca* pecaMovida = tabuleiro[ox][oy]->peca;
                tabuleiro[dx][dy]->peca = pecaMovida;
                // CORREÇÃO AQUI: APENAS ZERA O PONTEIRO DA PECA NA CASA DE ORIGEM, NÃO LIBERA A CASA EM SI
                tabuleiro[ox][oy]->peca = NULL; // A casa de origem agora esta vazia

                // Checa promocao de peao
                if (pecaMovida->sigla == 'P' && ((pecaMovida->cor == 'B' && dx == 0) || (pecaMovida->cor == 'P' && dx == 7))) {
                    promoverPeao(dx, dy, pecaMovida->cor);
                }

                // Troca o turno
                turno = (turno == 'B') ? 'P' : 'B';

                // Verifica o estado do jogo para o proximo turno (apos a troca de turno)
                char corProximoTurno = turno;
                if (estaEmXequeMate(corProximoTurno)) {
                    printf(BRIGHT_RED "XEQUE-MATE! O Rei %s não tem movimentos legais e está sob ataque. Fim de jogo!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
                    resultadoPartida = (corProximoTurno == 'P' ? 'V' : 'P'); // 'V' para vitoria do jogador que deu xeque-mate
                } else if (estaAfogado(corProximoTurno)) {
                    printf(BRIGHT_YELLOW "EMPATE! Jogo Afogado (Pat): O Rei %s não está em xeque, mas não tem movimentos legais. Fim de jogo!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
                    resultadoPartida = 'E';
                } else if (estaEmXeque(corProximoTurno)) {
                    printf(BRIGHT_RED "XEQUE! O Rei %s está sob ataque!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
                }

            } else {
                // Se a jogada inválida é porque o rei está em xeque e o movimento não o tira
                if (estaEmXeque(turno)) {
                    printf(BRIGHT_RED "Jogada inválida: Você continua em XEQUE! O movimento não tira seu rei do ataque.\n" RESET);
                } else {
                    printf(BRIGHT_RED "Jogada inválida: Este movimento resulta em xeque no seu próprio rei. Escolha outra jogada.\n" RESET);
                }
            }
        } else {
            printf(BRIGHT_RED "Jogada inválida para a peça selecionada. Verifique as regras de movimento da peça e tente novamente.\n" RESET);
        }
        mostrarTabuleiro(); // Mostra o tabuleiro apos cada movimento (ou tentativa)
    }

    // Mensagens de fim de jogo so se a partida realmente terminou (nao por salvar/sair)
    if (resultadoPartida == 'V') {
        printf(BRIGHT_GREEN "FIM DE JOGO! As Brancas venceram!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('V');
    } else if (resultadoPartida == 'P') {
        printf(BRIGHT_RED "FIM DE JOGO! As Pretas venceram!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('D');
    } else if (resultadoPartida == 'E') {
        printf(BRIGHT_YELLOW "FIM DE JOGO! Empate!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('E');
    }

    // Libera o tabuleiro apenas se a partida realmente terminou ou foi encerrada
    if (resultadoPartida != 'S') { // Nao libera se apenas salvou e saiu
        liberarTabuleiro();
    }
}


void jogarContraBot() {
    int ox, oy, dx, dy;
    char colunaOrigem, colunaDestino;
    int linhaOrigem, linhaDestino;
    char resultadoPartida = 'N';

    inicializarTabuleiro(); // Reinicia o tabuleiro para uma nova partida
    verificarTabuleiropIntegridade(); // Adicionado para depuração
    mostrarTabuleiro();

    // Defina a cor do jogador e do bot
    char corJogador = 'B'; // Jogador e sempre Brancas
    char corBot = 'P';     // Bot e sempre Pretas

    printf(BRIGHT_CYAN "Voce é as peças Brancas. O Bot é as peças Pretas.\n" RESET);

    while (resultadoPartida == 'N') {
        if (turno == corJogador) {
            printf(BRIGHT_BLUE "Sua vez (Brancas). Digite o movimento (ex: e2 e4, ou 'salvar', 'sair'): " RESET);
            char input[10];
            if (scanf("%9s", input) != 1) {
                printf(BRIGHT_RED "Entrada inválida. Tente novamente.\n" RESET);
                int c; while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }

            if (strcmp(input, "salvar") == 0) {
                salvarPartida();
                printf(BRIGHT_GREEN "Partida salva com sucesso. Saindo do jogo.\n" RESET);
                resultadoPartida = 'S';
                break;
            }
            if (strcmp(input, "sair") == 0) {
                printf(BRIGHT_YELLOW "Saindo do jogo.\n" RESET);
                resultadoPartida = 'X';
                break;
            }

            char input2[10];
            if (scanf("%9s", input2) != 1) {
                printf(BRIGHT_RED "Entrada inválida. Tente novamente.\n" RESET);
                int c; while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }

            colunaOrigem = tolower(input[0]);
            linhaOrigem = atoi(&input[1]);
            colunaDestino = tolower(input2[0]);
            linhaDestino = atoi(&input2[1]);

            ox = 8 - linhaOrigem;
            oy = colunaOrigem - 'a';
            dx = 8 - linhaDestino;
            dy = colunaDestino - 'a';

            // --- Adicionado para depuração: Mostra as coordenadas internas ---
            printf(BRIGHT_BLUE "DEBUG: Origem (%c%d) -> (%d, %d); Destino (%c%d) -> (%d, %d)\n" RESET,
                   colunaOrigem, linhaOrigem, ox, oy, colunaDestino, linhaDestino, dx, dy);
            // --- Fim da depuração ---

            if (ox < 0 || ox > 7 || oy < 0 || oy > 7 || dx < 0 || dx > 7 || dy < 0 || dy > 7) {
                printf(BRIGHT_RED "Coordenadas inválidas. Por favor, insira coordenadas dentro do tabuleiro (e.g., a1 a2).\n" RESET);
                continue;
            }
            // Adicionado: verifica se a casa de origem é NULL antes de acessar peca
            if (tabuleiro[ox][oy] == NULL || tabuleiro[ox][oy]->peca == NULL || tabuleiro[ox][oy]->peca->cor != turno) {
                printf(BRIGHT_RED "Jogada inválida: Não há sua peça na origem ou não é seu turno. Tente novamente.\n" RESET);
                continue;
            }

            if (ehMovimentoValido(ox, oy, dx, dy)) {
                if (!movimentoResultaEmXeque(ox, oy, dx, dy, turno)) {
                    if (tabuleiro[dx][dy]->peca != NULL) {
                        if (tabuleiro[dx][dy]->peca->cor == 'B') {
                            if (totalPecasCapturadasBrancas < 16) {
                                pecasCapturadasBrancas[totalPecasCapturadasBrancas++] = tabuleiro[dx][dy]->peca->sigla;
                            }
                        } else {
                            if (totalPecasCapturadasPretas < 16) {
                                pecasCapturadasPretas[totalPecasCapturadasPretas++] = tabuleiro[dx][dy]->peca->sigla;
                            }
                        }
                        free(tabuleiro[dx][dy]->peca);
                        tabuleiro[dx][dy]->peca = NULL;
                    }
                    Peca* pecaMovida = tabuleiro[ox][oy]->peca;
                    tabuleiro[dx][dy]->peca = pecaMovida;
                    // CORREÇÃO AQUI: APENAS ZERA O PONTEIRO DA PECA NA CASA DE ORIGEM
                    tabuleiro[ox][oy]->peca = NULL;

                    if (pecaMovida->sigla == 'P' && ((pecaMovida->cor == 'B' && dx == 0) || (pecaMovida->cor == 'P' && dx == 7))) {
                        promoverPeao(dx, dy, pecaMovida->cor);
                    }
                    turno = (turno == 'B') ? 'P' : 'B';
                } else {
                    if (estaEmXeque(turno)) {
                        printf(BRIGHT_RED "Jogada inválida: Você continua em XEQUE! O movimento não tira seu rei do ataque.\n" RESET);
                    } else {
                        printf(BRIGHT_RED "Jogada inválida: Este movimento resulta em xeque no seu próprio rei. Escolha outra jogada.\n" RESET);
                    }
                }
            } else {
                printf(BRIGHT_RED "Jogada inválida para a peça selecionada. Verifique as regras de movimento da peça e tente novamente.\n" RESET);
            }
        } else { // Turno do Bot
            printf(BRIGHT_YELLOW "Vez do Bot (Pretas)...\n" RESET);
            // Salvamos o nome do jogador atual para o bot para a funcao promoverPeao
            char jogadorAnterior[32];
            strcpy(jogadorAnterior, jogadorAtual);
            strcpy(jogadorAtual, "Bot"); // Temporariamente define o jogador atual como "Bot"

            if (!fazerMovimentoBot(corBot)) {
                // Se o bot nao conseguiu fazer um movimento, o jogo deve estar em xeque-mate ou afogado
                printf(BRIGHT_RED "O Bot não conseguiu encontrar um movimento legal. O jogo pode ter terminado em xeque-mate ou afogado para o Bot.\n" RESET);
            }
            strcpy(jogadorAtual, jogadorAnterior); // Restaura o nome do jogador anterior

            turno = (turno == 'B') ? 'P' : 'B'; // Troca o turno para o jogador
        }

        mostrarTabuleiro();

        // Verifica o estado do jogo para o proximo turno (apos a troca de turno)
        char corProximoTurno = turno;
        if (estaEmXequeMate(corProximoTurno)) {
            printf(BRIGHT_RED "XEQUE-MATE! O Rei %s não tem movimentos legais e está sob ataque. Fim de jogo!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
            resultadoPartida = (corProximoTurno == 'P' ? 'V' : 'P');
        } else if (estaAfogado(corProximoTurno)) {
            printf(BRIGHT_YELLOW "EMPATE! Jogo Afogado (Pat): O Rei %s não está em xeque, mas não tem movimentos legais. Fim de jogo!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
            resultadoPartida = 'E';
        } else if (estaEmXeque(corProximoTurno)) {
            printf(BRIGHT_RED "XEQUE! O Rei %s está sob ataque!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
        }
    }

    if (resultadoPartida == 'V') {
        printf(BRIGHT_GREEN "FIM DE JOGO! As Brancas venceram!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('V');
    } else if (resultadoPartida == 'P') {
        printf(BRIGHT_RED "FIM DE JOGO! As Pretas venceram!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('D');
    } else if (resultadoPartida == 'E') {
        printf(BRIGHT_YELLOW "FIM DE JOGO! Empate!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('E');
    }

    if (resultadoPartida != 'S') {
        liberarTabuleiro();
    }
}


int fazerMovimentoBot(char corDoBot) {
    srand(time(NULL)); // Inicializa o gerador de numeros aleatorios

    // Estrategia simples do bot: encontrar o primeiro movimento legal aleatorio
    int movimentosTentados = 0;
    while (movimentosTentados < 10000) { // Limita as tentativas para evitar loop infinito
        int ox = rand() % 8;
        int oy = rand() % 8;

        // Adicionada verificacao de NULL para tabuleiro[ox][oy]
        if (tabuleiro[ox][oy] != NULL && tabuleiro[ox][oy]->peca != NULL && tabuleiro[ox][oy]->peca->cor == corDoBot) {
            int dx = rand() % 8;
            int dy = rand() % 8;

            if (ehMovimentoValido(ox, oy, dx, dy)) {
                if (!movimentoResultaEmXeque(ox, oy, dx, dy, corDoBot)) {
                    // Executa o movimento
                    if (tabuleiro[dx][dy]->peca != NULL) {
                        if (tabuleiro[dx][dy]->peca->cor == 'B') {
                            if (totalPecasCapturadasBrancas < 16) {
                                pecasCapturadasBrancas[totalPecasCapturadasBrancas++] = tabuleiro[dx][dy]->peca->sigla;
                            }
                        } else {
                            if (totalPecasCapturadasPretas < 16) {
                                pecasCapturadasPretas[totalPecasCapturadasPretas++] = tabuleiro[dx][dy]->peca->sigla;
                            }
                        }
                        free(tabuleiro[dx][dy]->peca);
                        tabuleiro[dx][dy]->peca = NULL;
                    }
                    Peca* pecaMovida = tabuleiro[ox][oy]->peca;
                    tabuleiro[dx][dy]->peca = pecaMovida;
                    // CORREÇÃO AQUI: APENAS ZERA O PONTEIRO DA PECA NA CASA DE ORIGEM
                    tabuleiro[ox][oy]->peca = NULL; // A casa de origem agora esta vazia

                    // Promocao de peao do bot (ja chamei promoverPeao, mas ela fara a logica de Dama para o bot)
                    if (pecaMovida->sigla == 'P' && ((pecaMovida->cor == 'B' && dx == 0) || (pecaMovida->cor == 'P' && dx == 7))) {
                        promoverPeao(dx, dy, pecaMovida->cor);
                    }
                    printf(BRIGHT_YELLOW "Bot moveu de %c%d para %c%d\n" RESET, oy + 'a', 8 - ox, dy + 'a', 8 - dx);
                    return 1; // Movimento realizado com sucesso
                }
            }
        }
        movimentosTentados++;
    }
    printf(BRIGHT_RED "Bot não encontrou um movimento legal após muitas tentativas. (Pode ser xeque-mate ou afogado para o bot).\n" RESET);
    return 0; // Nao conseguiu fazer um movimento
}

// === Menu e Gerenciamento de Usuarios ===

void menuPrincipal() {
    int escolha;
    carregarRanking(); // Carrega o ranking ao iniciar o programa

    do {
        system("cls || clear"); // Limpa a tela
        printf(NEGRITO BRIGHT_CYAN "==========================================\n" RESET);
        printf(NEGRITO BRIGHT_GREEN "           MENU PRINCIPAL - XADREZ        \n" RESET);
        printf(NEGRITO BRIGHT_CYAN "==========================================\n" RESET);
        printf(BRIGHT_YELLOW " 1. Cadastrar Usuario                     \n" RESET);
        printf(BRIGHT_BLUE " 2. Login                                 \n" RESET);
        printf(BRIGHT_YELLOW " 3. Jogar (2 Jogadores)                   \n" RESET);
        printf(BRIGHT_BLUE " 4. Jogar contra BOT                      \n" RESET);
        printf(BRIGHT_YELLOW " 5. Continuar Partida Salva               \n" RESET);
        printf(BRIGHT_BLUE " 6. Mostrar Ranking                       \n" RESET);
        printf(BRIGHT_RED " 7. Sair                                  \n" RESET);
        printf(NEGRITO BRIGHT_CYAN "==========================================\n" RESET);
        printf(WHITE " Escolha uma opcao: " RESET);
        if (scanf("%d", &escolha) != 1) { // Verifica se a leitura foi bem-sucedida
            printf(BRIGHT_RED "Entrada inválida. Por favor, digite um número.\n" RESET);
            int c; while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer
            continue; // Volta ao inicio do loop
        }

        // Limpa o buffer do teclado apos ler a escolha do menu (essencial apos scanf %d)
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        switch (escolha) {
            case 1:
                cadastrarUsuario();
                break;
            case 2:
                loginUsuario();
                break;
            case 3:
                if (usuarioLogado != -1) {
                    printf(BRIGHT_YELLOW "Iniciando nova partida com 2 jogadores...\n" RESET);
                    jogar();
                } else {
                    printf(BRIGHT_RED "Por favor, faça login primeiro para jogar.\n" RESET);
                }
                break;
            case 4:
                if (usuarioLogado != -1) {
                    printf(BRIGHT_YELLOW "Iniciando nova partida contra o BOT...\n" RESET);
                    jogarContraBot();
                } else {
                    printf(BRIGHT_RED "Por favor, faça login primeiro para jogar contra o BOT.\n" RESET);
                }
                break;
            case 5:
                if (usuarioLogado != -1) {
                    printf(BRIGHT_YELLOW "Tentando continuar partida...\n" RESET);
                    continuarPartida();
                } else {
                    printf(BRIGHT_RED "Por favor, faça login primeiro para continuar uma partida.\n" RESET);
                }
                break;
            case 6:
                mostrarRanking();
                break;
            case 7:
                printf(BRIGHT_CYAN "Saindo do programa. Até mais!\n" RESET);
                // Garante que o tabuleiro esteja liberado ao sair do programa
                liberarTabuleiro();
                break;
            default:
                printf(BRIGHT_RED "Opção inválida. Tente novamente.\n" RESET);
        }
        if (escolha != 7) { // Nao pausa se o usuario escolheu sair
            printf(BRIGHT_BLUE "\nPressione ENTER para continuar..." RESET);
            int c; while ((c = getchar()) != '\n' && c != EOF); // Espera por ENTER
        }

    } while (escolha != 7);

    salvarRanking(); // Salva o ranking ao sair
}

void cadastrarUsuario() {
    if (totalUsuarios >= MAX_USUARIOS) {
        printf(BRIGHT_RED "Limite de usuários atingido.\n" RESET);
        return;
    }

    Usuario novoUsuario;
    printf(BRIGHT_BLUE "Digite o nome de usuário: " RESET);
    fgets(novoUsuario.nome, sizeof(novoUsuario.nome), stdin);
    novoUsuario.nome[strcspn(novoUsuario.nome, "\n")] = 0; // Remove newline

    // Verifica se o nome de usuario ja existe
    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, novoUsuario.nome) == 0) {
            printf(BRIGHT_RED "Nome de usuário já existe. Escolha outro.\n" RESET);
            return;
        }
    }

    printf(BRIGHT_BLUE "Digite a senha: " RESET);
    fgets(novoUsuario.senha, sizeof(novoUsuario.senha), stdin);
    novoUsuario.senha[strcspn(novoUsuario.senha, "\n")] = 0; // Remove newline

    novoUsuario.vitorias = 0;
    novoUsuario.derrotas = 0;
    novoUsuario.empates = 0;

    usuarios[totalUsuarios++] = novoUsuario;
    printf(BRIGHT_GREEN "Usuário cadastrado com sucesso!\n" RESET);
}

void loginUsuario() {
    char nome[32];
    char senha[32];

    printf(BRIGHT_BLUE "Digite seu nome de usuário: " RESET);
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf(BRIGHT_BLUE "Digite sua senha: " RESET);
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0;

    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0 && strcmp(usuarios[i].senha, senha) == 0) {
            usuarioLogado = i;
            strcpy(jogadorAtual, usuarios[i].nome);
            printf(BRIGHT_GREEN "Login bem-sucedido! Bem-vindo, %s!\n" RESET, jogadorAtual);
            return;
        }
    }
    printf(BRIGHT_RED "Nome de usuário ou senha incorretos.\n" RESET);
    usuarioLogado = -1;
    strcpy(jogadorAtual, "");
}

void salvarRanking() {
    FILE *arq = fopen("ranking.txt", "w");
    if (arq == NULL) {
        perror(BRIGHT_RED "Erro ao abrir ranking.txt para escrita" RESET);
        return;
    }

    fprintf(arq, "%d\n", totalUsuarios); // Salva o numero total de usuarios
    for (int i = 0; i < totalUsuarios; i++) {
        fprintf(arq, "%s\n", usuarios[i].nome);
        fprintf(arq, "%s\n", usuarios[i].senha);
        fprintf(arq, "%d %d %d\n", usuarios[i].vitorias, usuarios[i].derrotas, usuarios[i].empates);
    }

    fclose(arq);
    printf(BRIGHT_GREEN "Ranking salvo.\n" RESET);
}

void carregarRanking() {
    FILE *arq = fopen("ranking.txt", "r");
    if (arq == NULL) {
        printf(BRIGHT_YELLOW "Nenhum ranking existente. Um novo será criado.\n" RESET);
        totalUsuarios = 0;
        return;
    }

    if (fscanf(arq, "%d\n", &totalUsuarios) != 1) {
        printf(BRIGHT_RED "Erro ao ler total de usuários do ranking.\n" RESET);
        fclose(arq);
        return;
    }

    // Use a loop guard and check return values carefully for file I/O
    for (int i = 0; i < totalUsuarios && i < MAX_USUARIOS; i++) {
        // Read name
        if (fgets(usuarios[i].nome, sizeof(usuarios[i].nome), arq) == NULL) {
            fprintf(stderr, BRIGHT_RED "Erro ao ler nome do usuário %d\n" RESET, i);
            totalUsuarios = i; // Adjust totalUsers if read fails
            break;
        }
        usuarios[i].nome[strcspn(usuarios[i].nome, "\n")] = 0; // Remove newline

        // Read password
        if (fgets(usuarios[i].senha, sizeof(usuarios[i].senha), arq) == NULL) {
            fprintf(stderr, BRIGHT_RED "Erro ao ler senha do usuário %d\n" RESET, i);
            totalUsuarios = i; // Adjust totalUsers if read fails
            break;
        }
        usuarios[i].senha[strcspn(usuarios[i].senha, "\n")] = 0; // Remove newline

        // Read stats, consume newline after
        if (fscanf(arq, "%d %d %d\n", &usuarios[i].vitorias, &usuarios[i].derrotas, &usuarios[i].empates) != 3) {
            fprintf(stderr, BRIGHT_RED "Erro ao ler estatísticas do usuário %d\n" RESET, i);
            totalUsuarios = i; // Adjust totalUsers if read fails
            break;
        }
    }
    fclose(arq);
    printf(BRIGHT_GREEN "Ranking carregado.\n" RESET);
}

void mostrarRanking() {
    if (totalUsuarios == 0) {
        printf(BRIGHT_YELLOW "Nenhum usuário no ranking ainda.\n" RESET);
        return;
    }

    // Algoritmo de ordenacao simples (Bubble Sort) para o ranking baseado em vitorias
    // MUDANÇA AQUI: Ordenar por Winrate (maior Winrate primeiro)
    for (int i = 0; i < totalUsuarios - 1; i++) {
        for (int j = 0; j < totalUsuarios - i - 1; j++) {
            float winrate1, winrate2;
            int totalJogos1 = usuarios[j].vitorias + usuarios[j].derrotas + usuarios[j].empates;
            int totalJogos2 = usuarios[j+1].vitorias + usuarios[j+1].derrotas + usuarios[j+1].empates;

            winrate1 = (totalJogos1 == 0) ? 0.0 : ((float)usuarios[j].vitorias / totalJogos1) * 100.0;
            winrate2 = (totalJogos2 == 0) ? 0.0 : ((float)usuarios[j+1].vitorias / totalJogos2) * 100.0;

            if (winrate1 < winrate2) {
                Usuario temp = usuarios[j];
                usuarios[j] = usuarios[j + 1];
                usuarios[j + 1] = temp;
            }
        }
    }

    printf(NEGRITO "\n--- RANKING ---\n" RESET);
    // MUDANÇA AQUI: Adicionado Winrate
    printf(BRIGHT_BLUE "%-20s %-8s %-8s %-8s %-10s\n" RESET, "Nome", "Vitorias", "Derrotas", "Empates", "Winrate(%)");
    printf(BRIGHT_BLUE "------------------------------------------------------------\n" RESET);
    for (int i = 0; i < totalUsuarios; i++) {
        int totalJogos = usuarios[i].vitorias + usuarios[i].derrotas + usuarios[i].empates;
        float winrate = (totalJogos == 0) ? 0.0 : ((float)usuarios[i].vitorias / totalJogos) * 100.0;
        printf("%-20s %-8d %-8d %-8d %-10.2f\n", // .2f para 2 casas decimais
               usuarios[i].nome,
               usuarios[i].vitorias,
               usuarios[i].derrotas,
               usuarios[i].empates,
               winrate);
    }
    printf(BRIGHT_BLUE "------------------------------------------------------------\n" RESET);
}

void atualizarRanking(char resultado) {
    if (usuarioLogado != -1) {
        if (resultado == 'V') {
            usuarios[usuarioLogado].vitorias++;
        } else if (resultado == 'D') {
            usuarios[usuarioLogado].derrotas++;
        } else if (resultado == 'E') {
            usuarios[usuarioLogado].empates++;
        }
        printf(BRIGHT_GREEN "Ranking de %s atualizado.\n" RESET, usuarios[usuarioLogado].nome);
    }
}

void salvarPartida() {
    FILE *f = fopen("partida_salva.txt", "w");
    if (f == NULL) {
        perror(BRIGHT_RED "Erro ao salvar partida" RESET);
        return;
    }

    fprintf(f, "%s\n", jogadorAtual);
    fprintf(f, "%c\n", turno);
    fprintf(f, "%d\n", usuarioLogado); // Salva o indice do usuario logado

    // Salva o estado das pecas capturadas
    fprintf(f, "%d\n", totalPecasCapturadasBrancas);
    for (int i = 0; i < totalPecasCapturadasBrancas; i++) {
        fprintf(f, "%c\n", pecasCapturadasBrancas[i]);
    }
    fprintf(f, "%d\n", totalPecasCapturadasPretas);
    for (int i = 0; i < totalPecasCapturadasPretas; i++) {
        fprintf(f, "%c\n", pecasCapturadasPretas[i]);
    }

    // Salva as pecas no tabuleiro
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // Verifica se a Casa ou a Peca dentro dela é NULL antes de tentar acessar
            if (tabuleiro[i][j] != NULL && tabuleiro[i][j]->peca != NULL) {
                fprintf(f, "%d %d %c %c\n", i, j,
                        tabuleiro[i][j]->peca->sigla,
                        tabuleiro[i][j]->peca->cor);
            }
        }
    }
    fprintf(f, "FIM\n"); // Marcador de fim para leitura

    fclose(f);
}

void continuarPartida() {
    FILE *f = fopen("partida_salva.txt", "r");
    if (f == NULL) {
        printf(BRIGHT_YELLOW "Nenhuma partida salva encontrada.\n" RESET);
        return;
    }

    // Limpa o tabuleiro atual e o historico de capturas antes de carregar
    liberarTabuleiro(); // Libera TODAS as Casa* e Peca* existentes
    // Agora precisamos re-inicializar as Casa*
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tabuleiro[i][j] = (Casa*)malloc(sizeof(Casa));
            if (tabuleiro[i][j] == NULL) {
                fprintf(stderr, BRIGHT_RED "Erro de alocacao de memoria para Casa ao carregar partida.\n" RESET);
                exit(EXIT_FAILURE);
            }
            tabuleiro[i][j]->peca = NULL;
        }
    }
    limparHistoricoCapturas();
    verificarTabuleiropIntegridade(); // Adicionado para depuração

    char tempJogadorAtual[32];
    char turnoChar;
    int tempUsuarioLogado;

    // Lendo o cabecalho
    // Use %31s for fixed-size buffer to prevent overflow
    if (fscanf(f, "%31s\n%c\n%d\n", tempJogadorAtual, &turnoChar, &tempUsuarioLogado) != 3) {
        printf(BRIGHT_RED "Erro ao ler dados de cabeçalho da partida salva.\n" RESET);
        fclose(f);
        return;
    }

    // Verifica se o usuario logado e o mesmo da partida salva
    if (usuarioLogado == -1) { // If no user is logged in, cannot continue a saved game
        printf(BRIGHT_RED "Nenhum usuário logado. Faça login para continuar uma partida salva.\n" RESET);
        fclose(f);
        inicializarTabuleiro();
        return;
    }

    if (tempUsuarioLogado != usuarioLogado || strcmp(tempJogadorAtual, usuarios[usuarioLogado].nome) != 0) {
        printf(BRIGHT_RED "A partida salva pertence a outro usuário (%s). Não é possível carregar.\n" RESET, tempJogadorAtual);
        fclose(f);
        inicializarTabuleiro(); // Reinicia um tabuleiro limpo se nao pode carregar a partida.
        return;
    }

    strcpy(jogadorAtual, tempJogadorAtual);
    turno = turnoChar;
    usuarioLogado = tempUsuarioLogado;

    // Carrega pecas capturadas
    if (fscanf(f, "%d\n", &totalPecasCapturadasBrancas) != 1) {
        printf(BRIGHT_RED "Erro ao ler total de peças brancas capturadas.\n" RESET); fclose(f); return;
    }
    for (int i = 0; i < totalPecasCapturadasBrancas; i++) {
        if (fscanf(f, " %c\n", &pecasCapturadasBrancas[i]) != 1) { // Espaco antes de %c para consumir newline
            printf(BRIGHT_RED "Erro ao ler peça branca capturada.\n" RESET); fclose(f); return;
        }
    }
    if (fscanf(f, "%d\n", &totalPecasCapturadasPretas) != 1) {
        printf(BRIGHT_RED "Erro ao ler total de peças pretas capturadas.\n" RESET); fclose(f); return;
        }
    for (int i = 0; i < totalPecasCapturadasPretas; i++) {
        if (fscanf(f, " %c\n", &pecasCapturadasPretas[i]) != 1) { // Espaco antes de %c para consumir newline
            printf(BRIGHT_RED "Erro ao ler peça preta capturada.\n" RESET); fclose(f); return;
        }
    }

    // Carrega as pecas no tabuleiro
    int i, j;
    char sigla, cor;
    // Loop até o "FIM"
    char buffer[10]; // Para ler a string "FIM"
    while (fscanf(f, "%d %d %c %c\n", &i, &j, &sigla, &cor) == 4) {
        // Aloca nova peca
        tabuleiro[i][j]->peca = (Peca*)malloc(sizeof(Peca));
        if (tabuleiro[i][j]->peca == NULL) {
            fprintf(stderr, BRIGHT_RED "Erro de alocacao de memoria ao carregar peça da partida.\n" RESET);
            liberarTabuleiro(); // Tenta liberar o que foi alocado ate agora
            fclose(f);
            return;
        }
        tabuleiro[i][j]->peca->sigla = sigla;
        tabuleiro[i][j]->peca->cor = cor;
    }
    // Uma última leitura para consumir o "FIM\n" se o loop acima parar com feof
    fgets(buffer, sizeof(buffer), f); // Reads the "FIM" line

    fclose(f);
    printf(BRIGHT_GREEN "Partida carregada com sucesso!.\n" RESET);
    mostrarTabuleiro();

    char resultadoPartida = 'N';
    int ox_input, oy_input, dx_input, dy_input;
    char colunaOrigem, colunaDestino;
    int linhaOrigem, linhaDestino;

    while (resultadoPartida == 'N') {
        printf(BRIGHT_BLUE "Digite o movimento (ex: e2 e4, ou 'salvar' para salvar e sair, 'sair' para sair): " RESET);
        char input[10];
        if (scanf("%9s", input) != 1) {
            printf(BRIGHT_RED "Entrada inválida. Tente novamente.\n" RESET);
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (strcmp(input, "salvar") == 0) {
            salvarPartida();
            printf(BRIGHT_GREEN "Partida salva com sucesso. Saindo do jogo.\n" RESET);
            resultadoPartida = 'S';
            break;
        }
        if (strcmp(input, "sair") == 0) {
            printf(BRIGHT_YELLOW "Saindo do jogo.\n" RESET);
            resultadoPartida = 'X';
            break;
        }

        char input2[10];
        if (scanf("%9s", input2) != 1) {
            printf(BRIGHT_RED "Entrada inválida. Tente novamente.\n" RESET);
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        colunaOrigem = tolower(input[0]);
        linhaOrigem = atoi(&input[1]);
        colunaDestino = tolower(input2[0]);
        linhaDestino = atoi(&input2[1]);

        ox_input = 8 - linhaOrigem;
        oy_input = colunaOrigem - 'a';
        dx_input = 8 - linhaDestino;
        dy_input = colunaDestino - 'a';

        // --- Adicionado para depuração: Mostra as coordenadas internas ---
        printf(BRIGHT_BLUE "DEBUG: Origem (%c%d) -> (%d, %d); Destino (%c%d) -> (%d, %d)\n" RESET,
               colunaOrigem, linhaOrigem, ox_input, oy_input, colunaDestino, linhaDestino, dx_input, dy_input);
        // --- Fim da depuração ---

        if (ox_input < 0 || ox_input > 7 || oy_input < 0 || oy_input > 7 || dx_input < 0 || dx_input > 7 || dy_input < 0 || dy_input > 7) {
            printf(BRIGHT_RED "Coordenadas inválidas. Por favor, insira coordenadas dentro do tabuleiro (e.g., a1 a2).\n" RESET);
            continue;
        }
        // Adicionado: verifica se a casa de origem é NULL antes de acessar peca
        if (tabuleiro[ox_input][oy_input] == NULL || tabuleiro[ox_input][oy_input]->peca == NULL || tabuleiro[ox_input][oy_input]->peca->cor != turno) {
            printf(BRIGHT_RED "Jogada inválida: Não há sua peça na origem ou não é seu turno. Tente novamente.\n" RESET);
            continue;
        }

        if (ehMovimentoValido(ox_input, oy_input, dx_input, dy_input)) {
            if (!movimentoResultaEmXeque(ox_input, oy_input, dx_input, dy_input, turno)) {
                if (tabuleiro[dx_input][dy_input]->peca != NULL) {
                    if (tabuleiro[dx_input][dy_input]->peca->cor == 'B') {
                        if (totalPecasCapturadasBrancas < 16) {
                            pecasCapturadasBrancas[totalPecasCapturadasBrancas++] = tabuleiro[dx_input][dy_input]->peca->sigla;
                        }
                    } else {
                        if (totalPecasCapturadasPretas < 16) {
                            pecasCapturadasPretas[totalPecasCapturadasPretas++] = tabuleiro[dx_input][dy_input]->peca->sigla;
                        }
                    }
                    free(tabuleiro[dx_input][dy_input]->peca);
                    tabuleiro[dx_input][dy_input]->peca = NULL;
                }
                Peca* pecaMovida = tabuleiro[ox_input][oy_input]->peca;
                tabuleiro[dx_input][dy_input]->peca = pecaMovida;
                // CORREÇÃO AQUI: APENAS ZERA O PONTEIRO DA PECA NA CASA DE ORIGEM
                tabuleiro[ox_input][oy_input]->peca = NULL;

                if (pecaMovida->sigla == 'P' && ((pecaMovida->cor == 'B' && dx_input == 0) || (pecaMovida->cor == 'P' && dx_input == 7))) {
                    promoverPeao(dx_input, dy_input, pecaMovida->cor);
                }

                turno = (turno == 'B') ? 'P' : 'B';

                char corProximoTurno = turno;
                if (estaEmXequeMate(corProximoTurno)) {
                    printf(BRIGHT_RED "XEQUE-MATE! O Rei %s não tem movimentos legais e está sob ataque. Fim de jogo!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
                    resultadoPartida = (corProximoTurno == 'P' ? 'V' : 'P');
                } else if (estaAfogado(corProximoTurno)) {
                    printf(BRIGHT_YELLOW "EMPATE! Jogo Afogado (Pat): O Rei %s não está em xeque, mas não tem movimentos legais. Fim de jogo!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
                    resultadoPartida = 'E';
                } else if (estaEmXeque(corProximoTurno)) {
                    printf(BRIGHT_RED "XEQUE! O Rei %s está sob ataque!\n" RESET, (corProximoTurno == 'P' ? "Preto" : "Branco"));
                }

            } else {
                if (estaEmXeque(turno)) {
                    printf(BRIGHT_RED "Jogada inválida: Você continua em XEQUE! O movimento não tira seu rei do ataque.\n" RESET);
                } else {
                    printf(BRIGHT_RED "Jogada inválida: Este movimento resulta em xeque no seu próprio rei. Escolha outra jogada.\n" RESET);
                }
            }
        } else {
            printf(BRIGHT_RED "Jogada inválida para a peça selecionada. Verifique as regras de movimento da peça e tente novamente.\n" RESET);
        }
        mostrarTabuleiro();
    }

    if (resultadoPartida == 'V') {
        printf(BRIGHT_GREEN "FIM DE JOGO! As Brancas venceram!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('V');
    } else if (resultadoPartida == 'P') {
        printf(BRIGHT_RED "FIM DE JOGO! As Pretas venceram!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('D');
    } else if (resultadoPartida == 'E') {
        printf(BRIGHT_YELLOW "FIM DE JOGO! Empate!\n" RESET);
        if (usuarioLogado != -1) atualizarRanking('E');
    }

    if (resultadoPartida != 'S') {
        liberarTabuleiro();
    }
}


int main() {
    menuPrincipal();
    return 0;
}
