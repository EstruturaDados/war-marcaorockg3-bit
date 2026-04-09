#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_MISSOES 5

typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// ===================== Funções auxiliares =====================
void removerQuebraLinha(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

void lerString(char *msg, char *destino, int tamanho) {
    printf("%s", msg);
    fgets(destino, tamanho, stdin);
    removerQuebraLinha(destino);
}

int lerInteiroPositivo(char *msg) {
    char buffer[50];
    int valor;
    while (1) {
        printf("%s", msg);
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &valor) == 1 && valor >= 0) return valor;
        printf("Entrada invalida!\n");
    }
}

// ===================== Cadastro e exibição =====================
void cadastrarTerritorios(Territorio *mapa, int n) {
    for (int i = 0; i < n; i++) {
        printf("\n=== Territorio %d ===\n", i + 1);
        lerString("Nome: ", mapa[i].nome, 30);
        lerString("Cor do exercito: ", mapa[i].cor, 10);
        mapa[i].tropas = lerInteiroPositivo("Quantidade de tropas: ");
    }
}

void exibirTerritorios(Territorio *mapa, int n) {
    printf("\n=== MAPA ATUAL ===\n");
    for (int i = 0; i < n; i++) {
        printf("[%d] %s | Cor: %s | Tropas: %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// ===================== Ataque =====================
void atacar(Territorio *atacante, Territorio *defensor) {
    if (atacante->tropas <= 0) {
        printf("❌ Este territorio nao possui tropas para atacar!\n");
        return;
    }
    int dadoAtacante = rand() % 6 + 1;
    int dadoDefensor = rand() % 6 + 1;

    printf("\n🎲 Batalha!\n");
    printf("Atacante (%s) tirou: %d\n", atacante->nome, dadoAtacante);
    printf("Defensor (%s) tirou: %d\n", defensor->nome, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("✅ O atacante venceu!\n");
        if (defensor->tropas > 0) defensor->tropas--;
        if (defensor->tropas == 0) {
            printf("🏳️ Territorio conquistado!\n");
            strcpy(defensor->cor, atacante->cor);
            int transferidas = atacante->tropas / 2;
            if (transferidas < 1) transferidas = 1;
            defensor->tropas = transferidas;
            atacante->tropas -= transferidas;
        }
    } else {
        printf("❌ O atacante perdeu!\n");
        if (atacante->tropas > 0) atacante->tropas--;
    }
}

// ===================== Missoes =====================
void atribuirMissao(char **destino, char *missoes[], int totalMissoes, Territorio *mapa, int n) {
    int indice = rand() % totalMissoes;
    if (strstr(missoes[indice], "Conquiste o territorio X")) {
        int terr = rand() % n;
        int tamanho = snprintf(NULL, 0, "Conquiste o territorio %s", mapa[terr].nome) + 1;
        *destino = (char *)malloc(tamanho);
        snprintf(*destino, tamanho, "Conquiste o territorio %s", mapa[terr].nome);
    } else {
        *destino = (char *)malloc(strlen(missoes[indice]) + 1);
        strcpy(*destino, missoes[indice]);
    }
}

int verificarMissao(char *missao, Territorio *mapa, int n, char *corJogador) {
    if (strstr(missao, "Conquiste o territorio")) {
        char nomeTerritorio[30];
        sscanf(missao, "Conquiste o territorio %29[^\n]", nomeTerritorio);
        for (int i = 0; i < n; i++) {
            if (strcmp(mapa[i].nome, nomeTerritorio) == 0 &&
                strcmp(mapa[i].cor, corJogador) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

// ===================== Loop de Ataque =====================
void menuAtaque(Territorio *mapa, int n, char *missaoJogador, char *corJogador, int *jogoAtivo) {
    char entrada[50];
    while (1) {
        exibirTerritorios(mapa, n);
        printf("\n=== MENU DE ATAQUE ===\n");
        printf("Digite o numero do territorio atacante: ");
        fgets(entrada, sizeof(entrada), stdin);
        removerQuebraLinha(entrada);

        if (strcmp(entrada, "sair") == 0) {
            *jogoAtivo = 0;
            printf("Voce saiu do jogo.\n");
            break;
        }

        int atk = atoi(entrada);

        printf("Digite o numero do territorio defensor: ");
        fgets(entrada, sizeof(entrada), stdin);
        removerQuebraLinha(entrada);
        int def = atoi(entrada);

        if (atk < 0 || atk >= n || def < 0 || def >= n) { printf("Indices invalidos!\n"); continue; }
        if (atk == def) { printf("Territorios iguais!\n"); continue; }
        if (strcmp(mapa[atk].cor, mapa[def].cor) == 0) { printf("Mesma cor! Ataque invalido.\n"); continue; }
        if (mapa[atk].tropas <= 0) { printf("Territorio atacante sem tropas!\n"); continue; }

        atacar(&mapa[atk], &mapa[def]);

        if (verificarMissao(missaoJogador, mapa, n, corJogador)) {
            printf("\n🏆 Parabens! Voce cumpriu sua missao e venceu o jogo!\n");
            *jogoAtivo = 0;
            break;
        }
    }
}

// ===================== Main =====================
int main() {
    srand(time(NULL));
    printf("=== WAR - Sistema com Missoes Estrategicas ===\n");

    int n = lerInteiroPositivo("Digite o numero de territorios: ");
    Territorio *mapa = (Territorio *)calloc(n, sizeof(Territorio));
    if (!mapa) { printf("Erro de memoria!\n"); return 1; }

    cadastrarTerritorios(mapa, n);

    char *missoes[TOTAL_MISSOES] = {
        "Conquistar 3 territorios seguidos",
        "Elimine todas as tropas da cor Vermelho",
        "Controle 5 territorios",
        "Mantenha pelo menos 2 territorios com mais de 3 tropas",
        "Conquiste o territorio X"
    };

    char *missaoJogador;
    atribuirMissao(&missaoJogador, missoes, TOTAL_MISSOES, mapa, n);

    char corJogador[10];
    strcpy(corJogador, mapa[0].cor); // cor do jogador

    printf("\n🎯 Sua missão inicial é: %s\n", missaoJogador);

    int jogoAtivo = 1;
    while (jogoAtivo) {
        int opcao;
        printf("\n=== MENU DE ACOES ===\n");
        printf("1. Atacar\n2. Verificar missao\n3. Sair\nEscolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();

        switch(opcao) {
            case 1:
                menuAtaque(mapa, n, missaoJogador, corJogador, &jogoAtivo);
                break;
            case 2:
                printf("\n🎯 Sua missao e: %s\n", missaoJogador);
                break;
            case 3:
                jogoAtivo = 0;
                printf("\nVoce saiu do jogo.\n");
                break;
            default:
                printf("Opcao invalida! Escolha 1, 2 ou 3.\n");
                break;
        }
    }

    free(missaoJogador);
    free(mapa);
    printf("\nFim do jogo!\n");
    return 0;
}
