#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define KEY_ESC 27
#define KEY_ENTER 10
#define ARROW_UP 119
#define ARROW_DOWN 115
#define ARROW_LEFT 97
#define ARROW_RIGHT 100
#define TEMPO_INICIAL 100
#define REDUCAO_TEMPO 5
#define TEMPO_MINIMO 20

typedef struct Snakenode {
  int Nodex, Nodey;
  struct Snakenode *next;
} Snakenode;

typedef struct {
  char Nome[4];
  int pont;
} Jogador;

typedef struct Highscore {
  Jogador jogador;
  struct Highscore *next;
} Highscore;

void exibirPlacar(int placar);
void exibirGameOver();
void adicionarSnakeNode(Snakenode **head, int x, int y);
void exibirSnake(Snakenode *head);
void limparSnake(Snakenode *head);
void liberarSnake(Snakenode **head);
void moverSnake(Snakenode **head, int x, int y);
void exibirMaca(int x, int y);
int verificarColisaoCorpo(Snakenode *head, int x, int y);
void gerarProximaMaca(int *x, int *y);
void ordenarHighscore(Highscore **cabeca, Jogador nick);
void escreverHighscore(Highscore *cabeca, FILE *fptr);
void exibirHighscore(Highscore *cabeca);
void liberarHighscore(Highscore **head);

int main() {
  Snakenode *head = NULL;
  int ch = 0, placar = 0, dirX = 1, dirY = 0, tempoIntervalo = TEMPO_INICIAL;
  FILE *fptr;
  Jogador player;

  printf("Bem vindo!\nDigite sua sigla de 3 letras: ");
  scanf("%s", player.Nome);
  screenInit(1);
  keyboardInit();
  timerInit(tempoIntervalo);

  adicionarSnakeNode(&head, 34, 12);
  srand((unsigned int)time(NULL));
  int PosMacaX = rand() % 68 + 8, PosMacaY = rand() % 16 + 4;
  exibirMaca(PosMacaX, PosMacaY);
  screenUpdate();
  sleep(3);

  while (ch != KEY_ESC) {
    if (keyhit()) {
      ch = readch();
      switch (ch) {
        case ARROW_UP: if (dirY != 1) { dirX = 0; dirY = -1; } break;
        case ARROW_DOWN: if (dirY != -1) { dirX = 0; dirY = 1; } break;
        case ARROW_LEFT: if (dirX != 1) { dirX = -1; dirY = 0; } break;
        case ARROW_RIGHT: if (dirX != -1) { dirX = 1; dirY = 0; } break;
      }
      screenUpdate();
    }

    if (timerTimeOver()) {
      int newX = head->Nodex + dirX, newY = head->Nodey + dirY;

      if (newX >= MAXX || newX <= MINX || newY >= MAXY || newY <= MINY || verificarColisaoCorpo(head, newX, newY)) break;

      if (newX == PosMacaX && newY == PosMacaY) {
        adicionarSnakeNode(&head, PosMacaX, PosMacaY);
        gerarProximaMaca(&PosMacaX, &PosMacaY);
        exibirMaca(PosMacaX, PosMacaY);
        placar++;
        tempoIntervalo = (tempoIntervalo > TEMPO_MINIMO) ? (tempoIntervalo - REDUCAO_TEMPO) : TEMPO_MINIMO;
        timerInit(tempoIntervalo);
      }

      exibirPlacar(placar);
      limparSnake(head);
      moverSnake(&head, newX, newY);
      exibirSnake(head);
      screenUpdate();
    }
  }

  liberarSnake(&head);
  keyboardDestroy();
  screenDestroy();
  player.pont = placar;
  fptr = fopen("placar.txt", "a");
  fwrite(&player, sizeof(Jogador), 1, fptr);
  fclose(fptr);

  Highscore *lista = NULL;
  fptr = fopen("placar.txt", "r");
  while (fread(&player, sizeof(Jogador), 1, fptr) == 1) {
    ordenarHighscore(&lista, player);
  }
  fclose(fptr);
  fptr = fopen("placar.txt", "w");
  escreverHighscore(lista, fptr);
  fclose(fptr);
  exibirHighscore(lista);
  liberarHighscore(&lista);
  exibirGameOver();
  timerDestroy();
  return 0;
}

void exibirGameOver() {
  screenSetColor(YELLOW, DARKGRAY);
  screenGotoxy(30, 10);
  printf("Game over!");
}

void exibirPlacar(int placar) {
  screenSetColor(YELLOW, DARKGRAY);
  screenGotoxy(35, 22);
  printf("Placar : %d", placar);
}

void adicionarSnakeNode(Snakenode **head, int x, int y) {
  Snakenode *novo = malloc(sizeof(Snakenode));
  novo->Nodex = x;
  novo->Nodey = y;
  novo->next = NULL;
  if (!*head) {
    *head = novo;
  } else {
    Snakenode *temp = *head;
    while (temp->next) temp = temp->next;
    temp->next = novo;
  }
}

void exibirSnake(Snakenode *head) {
  while (head) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(head->Nodex, head->Nodey);
    printf("X");
    head = head->next;
  }
}

void limparSnake(Snakenode *head) {
  while (head) {
    screenGotoxy(head->Nodex, head->Nodey);
    printf(" ");
    head = head->next;
  }
}

void liberarSnake(Snakenode **head) {
  while (*head) {
    Snakenode *temp = *head;
    *head = (*head)->next;
    free(temp);
  }
}

void moverSnake(Snakenode **head, int x, int y) {
  Snakenode *novo = malloc(sizeof(Snakenode));
  novo->Nodex = x;
  novo->Nodey = y;
  novo->next = *head;
  *head = novo;

  Snakenode *temp = *head;
  while (temp->next->next) temp = temp->next;
  free(temp->next);
  temp->next = NULL;
}

void exibirMaca(int x, int y) {
  screenSetColor(RED, DARKGRAY);
  screenGotoxy(x, y);
  printf("0");
}

int verificarColisaoCorpo(Snakenode *head, int x, int y) {
  while (head) {
    if (head->Nodex == x && head->Nodey == y) return 1;
    head = head->next;
  }
  return 0;
}

void gerarProximaMaca(int *x, int *y) {
  *x = rand() % 68 + 8;
  *y = rand() % 16 + 4;
}

void ordenarHighscore(Highscore **cabeca, Jogador nick) {
  Highscore *novo = malloc(sizeof(Highscore));
  novo->jogador = nick;
  novo->next = NULL;

  if (!*cabeca || (*cabeca)->jogador.pont < nick.pont) {
    novo->next = *cabeca;
    *cabeca = novo;
  } else {
    Highscore *temp = *cabeca;
    while (temp->next && temp->next->jogador.pont >= nick.pont) {
      temp = temp->next;
    }
    novo->next = temp->next;
    temp->next = novo;
  }
}

void escreverHighscore(Highscore *cabeca, FILE *fptr) {
  while (cabeca) {
    fwrite(&cabeca->jogador, sizeof(Jogador), 1, fptr);
    cabeca = cabeca->next;
  }
}

void exibirHighscore(Highscore *cabeca) {
  int i = 1;
  while (cabeca && i < 4) {
    printf("%d colocado!:\nNome: %s\nPontuação: %d\n", i, cabeca->jogador.Nome, cabeca->jogador.pont);
    cabeca = cabeca->next;
    i++;
  }
}

void liberarHighscore(Highscore **head) {
  while (*head) {
    Highscore *temp = *head;
    *head = (*head)->next;
    free(temp);
  }
}
