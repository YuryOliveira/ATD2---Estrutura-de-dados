/*
 * Atividade Prática - Implementação de Árvore AVL
 * Compilar: gcc avl.c -o avl
 * Executar: ./avl   (no Windows: avl.exe)
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_VALORES 100

// Nó da árvore: chave, altura e ponteiros para os filhos
typedef struct No {
    int chave;
    int altura;
    struct No *esquerdo;
    struct No *direito;
} No;

int maior(int a, int b) {
    return (a > b) ? a : b;
}

// Altura do nó (NULL = 0, folha = 1)
int altura(No *n) {
    return (n == NULL) ? 0 : n->altura;
}

void atualizarAltura(No *n) {
    n->altura = 1 + maior(altura(n->esquerdo), altura(n->direito));
}

// FB = altura da subárvore esquerda - altura da subárvore direita
int obterFatorBalanceamento(No *n) {
    return (n == NULL) ? 0 : altura(n->esquerdo) - altura(n->direito);
}

No* criarNo(int chave) {
    No *no = (No*) malloc(sizeof(No));
    if (no == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    no->chave = chave;
    no->altura = 1;
    no->esquerdo = NULL;
    no->direito = NULL;
    return no;
}

// ============================== ROTAÇÕES ==============================

// Simples à direita (caso LL): o filho esquerdo sobe
No* rotacaoDireita(No *y) {
    No *x = y->esquerdo;
    y->esquerdo = x->direito;
    x->direito = y;
    atualizarAltura(y);
    atualizarAltura(x);
    return x;
}

// Simples à esquerda (caso RR): o filho direito sobe
No* rotacaoEsquerda(No *x) {
    No *y = x->direito;
    x->direito = y->esquerdo;
    y->esquerdo = x;
    atualizarAltura(x);
    atualizarAltura(y);
    return y;
}

// Dupla esquerda-direita (caso LR)
No* rotacaoEsquerdaDireita(No *n) {
    n->esquerdo = rotacaoEsquerda(n->esquerdo);
    return rotacaoDireita(n);
}

// Dupla direita-esquerda (caso RL)
No* rotacaoDireitaEsquerda(No *n) {
    n->direito = rotacaoDireita(n->direito);
    return rotacaoEsquerda(n);
}

// Atualiza a altura e, se |FB| > 1, aplica a rotação do caso encontrado.
// É chamada na volta da recursão, tanto na inserção quanto na remoção.
No* balancear(No *no) {
    atualizarAltura(no);
    int fb = obterFatorBalanceamento(no);

    if (fb > 1) {  // pesado à esquerda: o sinal do FB do filho decide entre LL e LR
        if (obterFatorBalanceamento(no->esquerdo) >= 0) {
            printf("[ROTACAO] Caso LL no no %d (FB %d): rotacao simples a direita\n", no->chave, fb);
            return rotacaoDireita(no);
        }
        printf("[ROTACAO] Caso LR no no %d (FB %d): rotacao dupla esquerda-direita "
               "(esquerda em %d, direita em %d)\n", no->chave, fb, no->esquerdo->chave, no->chave);
        return rotacaoEsquerdaDireita(no);
    }
    if (fb < -1) {  // pesado à direita: o sinal do FB do filho decide entre RR e RL
        if (obterFatorBalanceamento(no->direito) <= 0) {
            printf("[ROTACAO] Caso RR no no %d (FB %d): rotacao simples a esquerda\n", no->chave, fb);
            return rotacaoEsquerda(no);
        }
        printf("[ROTACAO] Caso RL no no %d (FB %d): rotacao dupla direita-esquerda "
               "(direita em %d, esquerda em %d)\n", no->chave, fb, no->direito->chave, no->chave);
        return rotacaoDireitaEsquerda(no);
    }
    return no;
}

// ====================== INSERÇÃO, REMOÇÃO E BUSCA ======================

// Insere como numa árvore binária de busca e rebalanceia na volta da recursão
No* inserir(No *no, int chave) {
    if (no == NULL)
        return criarNo(chave);

    if (chave < no->chave) {
        no->esquerdo = inserir(no->esquerdo, chave);
    } else if (chave > no->chave) {
        no->direito = inserir(no->direito, chave);
    } else {
        printf("[AVISO] O valor %d ja existe na arvore! Duplicados nao sao permitidos.\n", chave);
        return no;
    }
    return balancear(no);
}

No* buscar(No *raiz, int chave) {
    if (raiz == NULL || raiz->chave == chave)
        return raiz;
    if (chave < raiz->chave)
        return buscar(raiz->esquerdo, chave);
    return buscar(raiz->direito, chave);
}

// Menor valor de uma subárvore (usado para achar o sucessor em ordem)
No* noMenorValor(No *no) {
    while (no->esquerdo != NULL)
        no = no->esquerdo;
    return no;
}

// Remove a chave e rebalanceia na volta da recursão
No* remover(No *no, int chave) {
    if (no == NULL) {
        printf("[AVISO] O valor %d nao foi encontrado para remocao.\n", chave);
        return NULL;
    }

    if (chave < no->chave) {
        no->esquerdo = remover(no->esquerdo, chave);
    } else if (chave > no->chave) {
        no->direito = remover(no->direito, chave);
    } else if (no->esquerdo == NULL || no->direito == NULL) {
        // Folha ou um filho: o filho (ou NULL) ocupa o lugar do nó
        No *filho = (no->esquerdo != NULL) ? no->esquerdo : no->direito;
        free(no);
        return filho;
    } else {
        // Dois filhos: copia o sucessor em ordem e remove o sucessor da subárvore direita
        No *sucessor = noMenorValor(no->direito);
        no->chave = sucessor->chave;
        no->direito = remover(no->direito, sucessor->chave);
    }
    return balancear(no);
}

// Mostra qual caso de remoção vai acontecer e remove o valor
No* removerValor(No *raiz, int valor) {
    No *alvo = buscar(raiz, valor);

    if (alvo == NULL) {
        printf("[AVISO] O valor %d nao esta na arvore.\n", valor);
        return raiz;
    }
    if (alvo->esquerdo != NULL && alvo->direito != NULL)
        printf(">> Removendo %d (dois filhos: substituido pelo sucessor %d)\n",
               valor, noMenorValor(alvo->direito)->chave);
    else if (alvo->esquerdo != NULL || alvo->direito != NULL)
        printf(">> Removendo %d (um filho)\n", valor);
    else
        printf(">> Removendo %d (folha)\n", valor);

    return remover(raiz, valor);
}

// ======================= PERCURSOS E VISUALIZAÇÃO =======================

void imprimirNo(No *n) {
    printf("[%d | Altura: %d | FB: %d] ", n->chave, n->altura, obterFatorBalanceamento(n));
}

void preOrdem(No *raiz) {
    if (raiz != NULL) {
        imprimirNo(raiz);
        preOrdem(raiz->esquerdo);
        preOrdem(raiz->direito);
    }
}

void emOrdem(No *raiz) {
    if (raiz != NULL) {
        emOrdem(raiz->esquerdo);
        imprimirNo(raiz);
        emOrdem(raiz->direito);
    }
}

void posOrdem(No *raiz) {
    if (raiz != NULL) {
        posOrdem(raiz->esquerdo);
        posOrdem(raiz->direito);
        imprimirNo(raiz);
    }
}

// Desenha a árvore deitada: raiz à esquerda, filho direito acima e esquerdo abaixo
void desenharArvore(No *raiz, int espaco) {
    int COUNT = 10;
    if (raiz == NULL)
        return;

    espaco += COUNT;
    desenharArvore(raiz->direito, espaco);

    printf("\n");
    for (int i = COUNT; i < espaco; i++)
        printf(" ");
    printf("[%d (h:%d, fb:%d)]\n", raiz->chave, raiz->altura, obterFatorBalanceamento(raiz));

    desenharArvore(raiz->esquerdo, espaco);
}

void exibirDesenhoArvore(No *raiz) {
    if (raiz == NULL) {
        printf("\nA arvore esta vazia!\n");
        return;
    }
    printf("\n===============================================\n");
    printf("         VISUALIZACAO GRAFICA DA ARVORE        \n");
    printf("===============================================\n");
    printf("Altura da arvore: %d\n", altura(raiz));
    printf("(raiz a esquerda; filho direito acima, esquerdo abaixo)\n");
    desenharArvore(raiz, 0);
    printf("\n===============================================\n");
}

void exibirPercursos(No *raiz) {
    if (raiz == NULL) {
        printf("\nA arvore esta vazia!\n");
        return;
    }
    printf("\n===============================================\n");
    printf("              PERCURSOS DA ARVORE              \n");
    printf("===============================================\n");
    printf("Altura da arvore: %d\n", altura(raiz));

    printf("\nPre-Ordem:\n");
    preOrdem(raiz);

    printf("\n\nEm-Ordem:\n");
    emOrdem(raiz);

    printf("\n\nPos-Ordem:\n");
    posOrdem(raiz);

    printf("\n\n===============================================\n");
}

void liberarArvore(No *raiz) {
    if (raiz != NULL) {
        liberarArvore(raiz->esquerdo);
        liberarArvore(raiz->direito);
        free(raiz);
    }
}

// ============================ LEITURA DO TECLADO ============================

// Lê uma linha inteira; retorna 0 se a entrada acabou
int lerLinha(char *linha, int tamanho) {
    return fgets(linha, tamanho, stdin) != NULL;
}

// Lê a opção do menu (-1 se não for um número; 0 se a entrada acabou)
int lerOpcao(void) {
    char linha[100], *fim;
    long opcao;

    if (!lerLinha(linha, sizeof(linha)))
        return 0;
    opcao = strtol(linha, &fim, 10);
    return (fim == linha) ? -1 : (int) opcao;
}

// Lê um ou mais inteiros na mesma linha, separados por espaço ou vírgula.
// Retorna quantos valores foram lidos.
int lerValores(int valores[], int max) {
    char linha[1024], *p, *fim;
    int qtd = 0;

    if (!lerLinha(linha, sizeof(linha)))
        return 0;
    for (p = linha; *p != '\0'; p++)
        if (*p == ',' || *p == ';')
            *p = ' ';

    p = linha;
    while (qtd < max) {
        long valor = strtol(p, &fim, 10);
        if (fim == p)
            break;
        valores[qtd++] = (int) valor;
        p = fim;
    }
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
        p++;
    if (*p != '\0')
        printf("[AVISO] Parte da entrada nao e um numero inteiro e foi ignorada.\n");
    if (qtd == 0)
        printf("[AVISO] Nenhum valor informado.\n");
    return qtd;
}

// ================================ MENU ================================

int main() {
    No *raiz = NULL;
    int valores[MAX_VALORES];
    int opcao, qtd, i;

    do {
        printf("\n=================================\n");
        printf("        ARVORE AVL - MENU\n");
        printf("=================================\n");
        printf("1. Inserir valor(es)\n");
        printf("2. Remover valor(es)\n");
        printf("3. Buscar valor(es)\n");
        printf("4. Visualizar arvore\n");
        printf("5. Exibir percursos\n");
        printf("6. Limpar arvore\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        opcao = lerOpcao();

        switch (opcao) {
            case 1:
                printf("Digite o(s) valor(es) para inserir (ex.: 30 7 25): ");
                qtd = lerValores(valores, MAX_VALORES);
                for (i = 0; i < qtd; i++) {
                    printf(">> Inserindo %d\n", valores[i]);
                    raiz = inserir(raiz, valores[i]);
                }
                break;
            case 2:
                printf("Digite o(s) valor(es) para remover: ");
                qtd = lerValores(valores, MAX_VALORES);
                for (i = 0; i < qtd; i++)
                    raiz = removerValor(raiz, valores[i]);
                break;
            case 3:
                printf("Digite o(s) valor(es) para buscar: ");
                qtd = lerValores(valores, MAX_VALORES);
                for (i = 0; i < qtd; i++) {
                    if (buscar(raiz, valores[i]) != NULL)
                        printf("[RESULTADO] O valor %d ESTA presente na arvore.\n", valores[i]);
                    else
                        printf("[RESULTADO] O valor %d NAO esta presente na arvore.\n", valores[i]);
                }
                break;
            case 4:
                exibirDesenhoArvore(raiz);
                break;
            case 5:
                exibirPercursos(raiz);
                break;
            case 6:
                liberarArvore(raiz);
                raiz = NULL;
                printf("[SUCESSO] Arvore esvaziada.\n");
                break;
            case 0:
                printf("Encerrando programa...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);

    liberarArvore(raiz);
    return 0;
}
