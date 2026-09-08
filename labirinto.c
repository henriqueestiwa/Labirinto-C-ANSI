#include <stdio.h>
#include <stdlib.h>

#define LINHAS 21
#define COLUNAS 51

typedef struct {
    int linha;
    int coluna;
} Posicao;

typedef struct NoFila{
    Posicao pos;
    struct NoFila *prox;
} NoFila;

char labirinto[LINHAS][COLUNAS] = {
    "#################################################", 
    "E..#..#...........#.................#...........#",
    "#.###.#.#.#######.#.#####.#########.#.#####.###.#", 
    "#...#.#.#.......#.#.....#.....#.....#.....#...#.#",
    "###.#.#.#######.#.#####.#####.#.#########.###.#.#",
    "#...#.#.......#.....#...#.....#.......#.....#...#",
    "#.###.#######.#######.###.###########.#.#####.###",
    "#.....#.....#.......#...#.....#.......#.....#...#",
    "#####.#.###.#######.###.###.#.#.###########.###.#",
    "#.....#...#.......#.....#...#.#.....#.......#...#",
    "#.#######.#######.#######.###.#####.#.#######.#.#",
    "#.......#.....#.........#...#.....#.#.......#.#.#",
    "#######.#####.#.#######.###.#####.#.#######.#.#.#",
    "#.....#.....#.#.....#...#.......#.#.....#...#.#.#",
    "#.###.#####.#.###.#.#.#########.#.#####.#.###.#.#",
    "#...#.......#.....#.#.........#.#.......#.....#.#",
    "###.###############.#########.#.###############.#",
    "#.................#.........#.#.................#",
    "#.#########################.#.#################.#",
    "#...........................#...................S",
    "#################################################",
};

char solucao[LINHAS][COLUNAS];
void exibirLabirinto(char lab[LINHAS][COLUNAS]);
Posicao localizarEntrada();
Posicao localizarSaida();
NoFila *inserirFila(NoFila *fila, Posicao p);
NoFila *removerFila(NoFila *fila, Posicao *p);
int filaVazia(NoFila *fila);
int visitado[LINHAS][COLUNAS];
Posicao anterior[LINHAS][COLUNAS];
int posicaoValida(int linha, int coluna);
void inicializarControle();
int bfs (Posicao entrada, Posicao saida);
int marcarCaminho(Posicao entrada, Posicao saida);

int main (void){
    Posicao entrada, saida;
    int encontrou, passos;

    printf(" --- Labirinto com Grafos - BFS --- \n\n");

    entrada = localizarEntrada();
    saida = localizarSaida();

    if (entrada.linha == -1){
        printf("Erro, nao foi encontrado a entrada E do labirinto.\n");
        return 1;
    }
    if (saida.linha == -1){
        printf("Erro, nao foi encontrado a saida S do labirinto.\n");
        return 1;
    }

    printf("Entrada: linha %i, coluna %i\n", entrada.linha, entrada.coluna);
    printf("Saida: linha %i, coluna %i \n\n", saida.linha, saida.coluna);

    printf("--- Labirinto Original -- \n\n");
    exibirLabirinto(labirinto);

    inicializarControle();

    encontrou = bfs(entrada, saida);

    if (!encontrou){
        printf("\nNao existe caminho entre a entrada e saida.\n");
        return 0;
    }

    passos = marcarCaminho(entrada, saida);

    printf("\n--- Labirinto Resolvido ---\n\n");
    exibirLabirinto(solucao);

    printf("\nCaminho encontrado com %i passos.", passos);

    return 0;
}

void exibirLabirinto(char lab[LINHAS][COLUNAS]){
    for(int i = 0; i < LINHAS; i++){
        for(int j = 0; j < COLUNAS; j++){
            printf ("%c", lab[i][j]);
        }
        printf("\n");
    }
}

Posicao localizarEntrada(){
    Posicao p;
    p.linha = -1;
    p.coluna = -1;
    for(int i = 0; i < LINHAS; i++){
        for(int j = 0; j < COLUNAS; j++){
            if (labirinto[i][j] == 'E'){
                p.linha = i;
                p.coluna = j;
                return p;
            }
        }
    }
    return p;
}

Posicao localizarSaida(){
    Posicao p;
    p.linha = -1;
    p.coluna = -1;
    for(int i = 0; i < LINHAS; i++){
        for(int j = 0; j < COLUNAS; j++){
            if (labirinto[i][j] == 'S'){
                p.linha = i;
                p.coluna = j;
                return p;
            }
        }
    }
    return p;
}

NoFila *inserirFila(NoFila *fila, Posicao p){
    NoFila *novo = (NoFila *) malloc(sizeof(NoFila));
    NoFila *aux;
    novo->pos = p;
    novo->prox = NULL;
    if (fila == NULL){
        return novo;
    }
    aux = fila;
    while (aux->prox !=NULL){
        aux = aux->prox;
    }
    aux->prox = novo;
    return fila;
}

NoFila *removerFila(NoFila *fila, Posicao *p){
    NoFila *aux = fila;
    *p = fila->pos;
    fila = fila->prox;
    free(aux);
    return fila;
}

int filaVazia(NoFila *fila){
    return fila == NULL;
}

void inicializarControle(){
    for (int i = 0; i < LINHAS; i++){
        for (int j = 0; j < COLUNAS; j++){
            visitado[i][j] = 0;
            anterior[i][j].linha = -1;
            anterior[i][j].coluna = -1;
            solucao[i][j] = labirinto[i][j];
        }
    }
}

int posicaoValida(int linha, int coluna){
    if (linha < 0 || linha >= LINHAS){
        return 0;
    }
    if (coluna < 0 || coluna >= COLUNAS){
        return 0;
    }
    if (labirinto[linha][coluna] == '#'){
        return 0;
    }
    if (visitado[linha][coluna] == 1){
        return 0;
    }
    return 1;
}

int bfs (Posicao entrada, Posicao saida){
    NoFila *fila = NULL;
    Posicao atual, vizinho;
    int dLinha[4] = {-1, 1, 0, 0};
    int dColuna[4] = {0, 0, -1, 1};

    visitado[entrada.linha][entrada.coluna] = 1;
    fila = inserirFila(fila, entrada);

    while (!filaVazia(fila)){
        fila = removerFila(fila, &atual);

        if (atual.linha == saida.linha && atual.coluna == saida.coluna){
            return 1;
        }

        for (int i = 0; i <4; i++){
            vizinho.linha = atual.linha + dLinha[i];
            vizinho.coluna = atual.coluna + dColuna[i];

            if (posicaoValida(vizinho.linha, vizinho.coluna)){
                visitado[vizinho.linha][vizinho.coluna] = 1;
                anterior[vizinho.linha][vizinho.coluna] = atual;
                fila = inserirFila(fila, vizinho);
            }
        }
    }
    return 0;
}

int marcarCaminho(Posicao entrada, Posicao saida){
    Posicao atual;
    int passos = 0;

    atual = saida;

    while (!(atual.linha == entrada.linha && atual.coluna == entrada.coluna)){
        if (solucao[atual.linha][atual.coluna] != 'E' && solucao[atual.linha][atual.coluna] != 'S'){
            solucao[atual.linha][atual.coluna] = '*';
        }
        passos++;
        atual = anterior[atual.linha][atual.coluna];
    }
    passos++;
    return passos;
}