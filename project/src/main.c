#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

void *lavando_carro(void *arg);
void *carro_em_lavagem(void *arg);
void *gerar_clientes(void *arg);
void *funcao_cliente(void *arg);
void *funcao_funcionario(void *arg);
void *desiste(void *arg);

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_func;
sem_t sem_client;
sem_t sem_pronto;
sem_t sem_vagas;

int vagas_livres;
int num_desistencias;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("\nEntrada invalida de dados;\n");
        printf("USO: lavacao [NUMERO DE VAGAS] [NUMERO DE CLIENTES]\n\n");
        exit(EXIT_FAILURE);
    }

    sem_init(&sem_client, 0, 0);
    sem_init(&sem_func, 0, 0);
    sem_init(&sem_pronto, 0, 0);
    sem_init(&sem_vagas, 0, 1);

    int vagas = atoi(argv[1]);
    int max_clientes = atoi(argv[2]);
    vagas_livres = vagas;

    pthread_t funcionario, gerador_clientes;

    pthread_create(&funcionario, NULL, funcao_funcionario, &max_clientes);
    pthread_create(&gerador_clientes, NULL, gerar_clientes, &max_clientes);

    pthread_join(funcionario, NULL);
    pthread_join(gerador_clientes, NULL);

    sem_close(&sem_func);
    sem_close(&sem_vagas);
    sem_close(&sem_pronto);
    sem_close(&sem_client);

    return 0;
}

void *lavando_carro(void *arg) {
    sleep(rand() % 8);
    return NULL;
}

void *carro_em_lavagem(void *arg) {
    int id = * (int *) arg;
    printf("cliente %d colocou o carro em lavagem\n", id);
    return NULL;
}

void *desiste(void *arg) {
    sem_post(&sem_vagas);
    num_desistencias ++;
    pthread_exit(NULL);
}

void *funcao_cliente(void *arg) {
    int id = * (int *) arg;
    sem_wait(&sem_vagas);
    if (vagas_livres >= 1) {
        vagas_livres --;
        printf("cliente %d entrou na loja\n", id);
        sem_post(&sem_client);
        sem_post(&sem_vagas);
        sem_wait(&sem_func);
        carro_em_lavagem(&id);
    } else {
        printf("cliente %d desistiu e foi embora!\n", id);
        desiste(NULL);
    }
    sem_wait(&sem_pronto);
    printf("cliente %d saiu da loja\n", id);

    return NULL;
}

void *funcao_funcionario(void *arg) {
    printf("funcionario chegou!\n");
    int max = * (int *) arg;
    int cnt = 0;
    while(true) {
        sem_wait(&sem_client);
        sem_wait(&sem_vagas);

        vagas_livres ++;

        sem_post(&sem_vagas);
        sem_post(&sem_func);

        pthread_mutex_lock(&lock);
        lavando_carro(NULL);
        printf("funcionario lavou um carro\n");
        sem_post(&sem_pronto);
        pthread_mutex_unlock(&lock);

        cnt ++;
        if (cnt == (max - num_desistencias)) { break; }
    }
    printf("nao ha mais clientes\n");
    return NULL;
}

void *gerar_clientes(void *arg) {
    int cnt = 0;
    int max = * (int *) arg;

    while(cnt < max) {
        pthread_t thread_cliente;
        pthread_create(&thread_cliente, NULL, funcao_cliente, &cnt);
        cnt ++;
        sleep(rand() % 4);
    }

    return NULL;
}
