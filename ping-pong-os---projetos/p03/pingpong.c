#include "queue.h"
#include "pingpong.h"
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

#define STACKSIZE 32768		/* tamanho de pilha das threads */
//#define DEBUG true

int id = 0;	//vai somando pra cada tarefa ter id diferente
task_t main_tsk;	// main task
task_t *current_tsk; // task sendo executada no momento

task_t dispatcher;  //task que controla os despachamentos
task_t *ready_tasks = NULL; //lista de tarefas prontas
int userTasks = 0;  //contador de tarefas na fila


task_t *scheduler();
void dispatcher_body(void *arg);
//void task_print(void* task_v);   //para imprimir a lista no debug


// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void pingpong_init ()
{
    main_tsk.tid = id++;	//comeca com id 0
    current_tsk = &main_tsk;

    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;

#ifdef DEBUG
    printf("inicializando main com id %d \n", (&main_tsk)->tid);
#endif

    task_create(&dispatcher, dispatcher_body, "dispatcher ");

}

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task,			// descritor da nova tarefa
                 void (*start_func)(void *),	// funcao corpo da tarefa
                 void *arg)			// argumentos para a tarefa
{

    if(task != NULL) {
        getcontext (&(task->context));	//pega contexto atual

        char *stack = malloc (STACKSIZE);
        //fazendo o contexto
        if (stack) {
            task->context.uc_stack.ss_sp = stack;
            task->context.uc_stack.ss_size = STACKSIZE;
            task->context.uc_stack.ss_flags = 0;
            task->context.uc_link = 0;
            task->tid = id++;

        } else {
            perror ("stack nao pode ser criado ");
            return -1;
        }
    } else {
        perror ("tarefa veio com erro ");
        return -1;
    }

    //liga a funcao a este contexto
    makecontext (&task->context, (void*)(*start_func), 1, arg);

    // colocando na fila as tarefas de unuario
    if(task->tid > 1) {
        userTasks++;
        queue_append((queue_t **) &ready_tasks, (queue_t *) task);
        task->my_queue = (queue_t **) &ready_tasks;
    }

#ifdef DEBUG
    printf("task_create: criou a tarefa %d\n", task->tid);
#endif

    return task->tid;
    //return 0;
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task)
{
    task_t *atual_tsk;
    if (task != NULL) {
        atual_tsk = current_tsk;
        current_tsk = task;
#ifdef DEBUG
        printf("task_switch: trocando contexto %d -> %d\n", atual_tsk->tid, current_tsk->tid);
#endif

        swapcontext(&atual_tsk->context, &current_tsk->context);
        return 0;
    } else {

        return -1;
    }
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode)
{
#ifdef DEBUG
    printf("task_exit: tarefa %d sendo encerrado com codigo %d\n", current_tsk->tid, exitCode);
#endif

    if(current_tsk->tid <= 1) {
        task_switch(&main_tsk);
    } else {
        userTasks--;
        task_switch(&dispatcher);
    }

}

// retorna o identificador da tarefa corrente (main eh 0)
int task_id ()
{
    int task_id_at = current_tsk->tid;
    return task_id_at;
}

// libera o processador para a próxima tarefa, retornando à fila de tarefas
// prontas ("ready queue")
void task_yield ()
{
#ifdef DEBUG
    printf("task_yield: id da tarefa: %d\n", current_tsk->tid);
#endif

    // colocando na fila as tarefas de usuario
    if(current_tsk->tid > 1) {
        userTasks++;
        queue_append((queue_t **) &ready_tasks, (queue_t *) current_tsk);  //colocando na fila para esperar
        current_tsk->my_queue = (queue_t **) &ready_tasks; // atualiza sua variavel de fila
    }

    task_switch(&dispatcher);   //volta pro dispatcher
}

void dispatcher_body(void *arg)
{
    userTasks = queue_size((queue_t *)ready_tasks);
    while ( userTasks > 0 ) {
        task_t *next = scheduler() ; // scheduler é uma função
        if (next) {
#ifdef DEBUG
            printf("dispatcher_body: proxima tarefa id: %d\n", next->tid);
            //queue_print("lista: ",(queue_t **)ready_tasks, task_print);
#endif
            // ações antes de lançar a tarefa "next", se houverem
            task_switch (next) ; // transfere controle para a tarefa "next"
            // ações após retornar da tarefa "next", se houverem
        }
        userTasks = queue_size((queue_t *)ready_tasks);
    }
    task_exit(0) ; // encerra a tarefa dispatcher
}

// do tipo FCFS
//first come first served
task_t *scheduler()
{
    //retorna nulo se nada na fila
    if(ready_tasks != NULL) {
        // lembrando que isso é a fila que eu fiz la no primeiro trabalho
        userTasks--;
        task_t * next = ready_tasks;
        queue_remove((queue_t **)&ready_tasks, (queue_t *)next);    //tira da fila
        //next->my_queue = (queue_t **) &ready_tasks;    // atualiza sua variavel de fila
        //ready_tasks = NULL;
        next->my_queue = NULL;
        return next;
    } else {
#ifdef DEBUG
        printf("Nada na fila, retornando 0 como proxima tarefa\n");
#endif
        return NULL;
    }
}

/*void task_print(void *task) //por alguma razao da erro se nao for void
{
    task_t *task_att = (task_t *) task;
    printf("-%d- ", task_att->tid);
}
//parecia uma boa ideia mas bugou*/
/*
void task_print(void* task_v)
{
    task_t *task = (task_t *) task_v;
    printf("<%d>", task->tid);
}
*/











