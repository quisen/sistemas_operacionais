// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
// bem que podia colocar as coisas ai, como vou saber o que tem que ter?
// Estruturas de dados internas do sistema operacional

#include <ucontext.h>
#ifndef __DATATYPES__
#define __DATATYPES__

typedef enum status {READY, EXEC, SUSP, ENDED} status_t;

// Estrutura que define uma tarefa
typedef struct task_t {
    struct task_t *prev, *next ;	// para usar com a biblioteca de filas (cast)
    int tid ;	// ID da tarefa
    // demais informações da tarefa
    ucontext_t context;
    struct queue_t **my_queue;  //fila em que essa task esta inserida
    int static_prio;
    int dinamic_prio;
    unsigned int init_time;     //tempo de inicio da tarefa em milisegundos
    unsigned int proc_time;     //tempo de processamento da tarefa em milisegundos
    unsigned int exec_time;     //tempo de execução da tarefa em milisegundos
    unsigned int activations;   // vezes em que a tarefa foi ativada;
    int exit_code;              // quando a tarefa sai salva aqui o codigo
    status_t status;
    struct task_t *wait_me_q;   //tarefa me esperando terminar
    int lock;                   // evitar que preempção estrague as coisas
} task_t ;

// estrutura que define um semáforo
typedef struct {
    // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct {
    // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct {
    // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct {
    // preencher quando necessário
} mqueue_t ;

#endif
