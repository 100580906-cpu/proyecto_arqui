/*
 * Fichero: program.c
 * Descripción: Programa principal que coordina el emulador de planificador de procesos.
 * Arquitectura de Sistemas - UC3M - Irene Rossi Albarrán
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "menu.h"
#include "input_usr.h"
#include "item.h"
#include "collection.h"

#define FIFO_NAME "MYFIFO"

static const char *nombres_estado[] = {"READY", "RUNNING", "STOPPED", "TERMINATED"};

typedef struct {
    char user[64];
    unsigned int P;
    unsigned int C;
} file_record_t;

static void handle_add(struct collection *col);
static void handle_delete(struct collection *col);
static void handle_info(const struct collection *col);
static void handle_info_all(const struct collection *col);
static void handle_clear_all(struct collection *col);
static void handle_sort(struct collection *col);
static void handle_load(struct collection *col);
static void handle_save(const struct collection *col);
static void handle_execute(struct collection *col);

int main(int argc, char *argv[]) {
    int err = SUCCESS;
    struct collection col = initialize_collection(&err);

    if (err != SUCCESS) {
        fprintf(stderr, "Error al inicializar la colección: %s\n", get_error_msg_col(err));
        return EXIT_FAILURE;
    }

    print_main_menu();

    int running = 1;
    while (running) {
        int opt = get_menu_option();

        switch (opt) {
            case 1:
                handle_add(&col);
                break;
            case 2:
                handle_delete(&col);
                break;
            case 3:
                handle_info(&col);
                break;
            case 4:
                handle_info_all(&col);
                break;
            case 5:
                handle_clear_all(&col);
                break;
            case 6:
                handle_sort(&col);
                break;
            case 7:
                handle_load(&col);
                break;
            case 8:
                handle_save(&col);
                break;
            case 9:
                handle_execute(&col);
                break;
            case 10:
                print_main_menu();
                break;
            case 0:
            case INPUT_EOF:
                running = 0;
                break;
            case INPUT_INVALID:
            default:
                fprintf(stderr, "Opción no válida. Por favor, intente de nuevo.\n");
                break;
        }
    }

    col = clean_collection(col);

    #ifdef DEBUG
    fprintf(stderr, "[DEBUG] Programa finalizado correctamente sin fugas de memoria.\n");
    #endif

    return EXIT_SUCCESS;
}

static void handle_add(struct collection *col) {
    printf("Añadiendo un nuevo proceso.\n");

    char owner[64];
    printf("Introduzca el nombre de la propietaria: ");
    fflush(stdout);
    if (read_string(owner, sizeof(owner)) == INPUT_EOF) return;

    int p = 0;
    printf("Introduzca la prioridad del proceso: ");
    fflush(stdout);
    int r_p = read_int(&p);
    if (r_p == INPUT_EOF) return;
    if (r_p == INPUT_INVALID || p <= 0) {
        fprintf(stderr, "Error: La prioridad debe ser un número entero mayor que 0.\n");
        return;
    }

    int c = 0;
    printf("Introduzca el tiempo de ejecución (C): ");
    fflush(stdout);
    int r_c = read_int(&c);
    if (r_c == INPUT_EOF) return;
    if (r_c == INPUT_INVALID || c <= 0) {
        fprintf(stderr, "Error: El tiempo de ejecución debe ser un número entero mayor que 0.\n");
        return;
    }

    int is_duplicate = 0;
    for (int i = 0; i < col->number_occupied; i++) {
        if (col->array[i] != NULL &&
            col->array[i]->P == (unsigned int)p &&
            col->array[i]->C == (unsigned int)c &&
            strcmp(col->array[i]->user, owner) == 0) {
            is_duplicate = 1;
            break;
        }
    }

    if (is_duplicate) {
        printf("Hay un proceso con esos datos en la planificación.\n");
        char ans[16];
        while (1) {
            printf("¿Desea añadirlo de todos modos? Responda SoN: ");
            fflush(stdout);
            int status = read_string(ans, sizeof(ans));
            if (status == INPUT_EOF) return;
            if (strcmp(ans, "s") == 0 || strcmp(ans, "S") == 0) {
                break;
            } else if (strcmp(ans, "n") == 0 || strcmp(ans, "N") == 0) {
                return;
            }
            printf("No le he entendido.\n");
        }
    }

    int err = SUCCESS;
    *col = add_item(*col, owner, (unsigned int)p, (unsigned int)c, &err);
    if (err != SUCCESS) {
        fprintf(stderr, "Error al añadir el proceso: %s\n", get_error_msg_col(err));
        return;
    }

    printf("Proceso añadido a la planificación en la posición %d\n", col->number_occupied);
}

static void handle_delete(struct collection *col) {
    printf("Borrando un proceso.\n");
    int pid = 0;
    printf("Introduzca el pid del proceso a borrar: ");
    fflush(stdout);
    if (read_int(&pid) == INPUT_EOF) return;

    int err = SUCCESS;
    *col = delete_item(*col, pid, &err);

    if (err == SUCCESS) {
        printf("Proceso borrado satisfactoriamente\n");
    } else {
        printf("Proceso no encontrado en la planificación.\n");
    }
}

static void handle_info(const struct collection *col) {
    printf("Información de un proceso.\n");
    int pid = 0;
    printf("Introduzca el pid del proceso a mostrar: ");
    fflush(stdout);
    if (read_int(&pid) == INPUT_EOF) return;

    int idx = -1;
    for (int i = 0; i < col->number_occupied; i++) {
        if (col->array[i] != NULL && col->array[i]->pid == pid) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Proceso no encontrado en la planificación.\n");
    } else {
        printf("POS\n%5d\nPID\n%5d\nP\n%5u\nC\n%5u\nSTATE\n%10s\nOWNER\n%-20s\n",
               idx + 1,
               col->array[idx]->pid,
               col->array[idx]->P,
               col->array[idx]->C,
               nombres_estado[col->array[idx]->estado],
               col->array[idx]->user);
    }
}

static void handle_info_all(const struct collection *col) {
    printf("Mostrando toda la planificación.\n");
    if (col->number_occupied == 0) {
        printf("No hay procesos en la planificación.\n");
        return;
    }

    printf("POS   PID     P     C      STATE OWNER               \n");
    for (int i = 0; i < col->number_occupied; i++) {
        if (col->array[i] != NULL) {
            printf("%5d %5d %5u %5u %10s %-20s\n",
                   i + 1,
                   col->array[i]->pid,
                   col->array[i]->P,
                   col->array[i]->C,
                   nombres_estado[col->array[i]->estado],
                   col->array[i]->user);
        }
    }
}

static void handle_clear_all(struct collection *col) {
    char ans[16];
    int confirm = -1;
    while (1) {
        printf("¿Quiere borrar toda la información? [S/N]: ");
        fflush(stdout);
        int status = read_string(ans, sizeof(ans));
        if (status == INPUT_EOF) return;
        if (strcmp(ans, "S") == 0 || strcmp(ans, "s") == 0) {
            confirm = 1;
            break;
        } else if (strcmp(ans, "N") == 0 || strcmp(ans, "n") == 0) {
            confirm = 0;
            break;
        }
        printf("No le he entendido\n");
    }

    if (confirm == 1) {
        int old_next_pid = col->next_pid;
        *col = clean_collection(*col);
        int err = SUCCESS;
        *col = initialize_collection(&err);
        col->next_pid = old_next_pid;
        printf("Borrando toda la planificación\n");
    } else {
        printf("Borrado abortado\n");
    }
}

static void handle_sort(struct collection *col) {
    int criterion = 0;
    while (1) {
        printf("Ordenando la colección por PID (1), P(2), C(3), OWNER (4): ");
        fflush(stdout);
        int status = read_int(&criterion);
        if (status == INPUT_EOF) return;
        if (status == INPUT_OK && criterion >= 1 && criterion <= 4) {
            break;
        }
        printf("Opción no reconocida\n");
    }

    int (*cmp_func)(const void *, const void *) = NULL;
    switch (criterion) {
        case 1: cmp_func = compare_PID; break;
        case 2: cmp_func = compare_P; break;
        case 3: cmp_func = compare_C; break;
        case 4: cmp_func = compare_OWNER; break;
    }

    int err = SUCCESS;
    *col = sort_collection(*col, cmp_func, &err);
    if (err == SUCCESS) {
        printf("Colección ordenada\n");
    } else {
        fprintf(stderr, "Error al ordenar la colección: %s\n", get_error_msg_col(err));
    }
}

static void handle_load(struct collection *col) {
    printf("Cargando un fichero a la planificación.\n");
    char filename[256];
    printf("Indique el nombre de fichero: ");
    fflush(stdout);
    if (read_string(filename, sizeof(filename)) == INPUT_EOF) return;

    if (access(filename, R_OK) != 0) {
        printf("El fichero no existe o no se puede leer.\n");
        return;
    }

    if (col->number_occupied > 0) {
        printf("La planificación actual no está vacía.\n");
        char ans[16];
        printf("¿Desea seguir? Responder sí la borrará (S/N): ");
        fflush(stdout);
        if (read_string(ans, sizeof(ans)) == INPUT_EOF) return;
        if (strcmp(ans, "S") != 0 && strcmp(ans, "s") != 0) {
            return;
        }
        int old_next_pid = col->next_pid;
        *col = clean_collection(*col);
        int err = SUCCESS;
        *col = initialize_collection(&err);
        col->next_pid = old_next_pid;
        printf("Planificación actual borrada\n");
    }

    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error al abrir el fichero %s: %s\n", filename, strerror(errno));
        return;
    }

    file_record_t rec;
    int err = SUCCESS;
    while (fread(&rec, sizeof(file_record_t), 1, f) == 1) {
        *col = add_item(*col, rec.user, rec.P, rec.C, &err);
    }

    fclose(f);
    printf("Planificación contenida en %s cargada\n", filename);
}

static void handle_save(const struct collection *col) {
    printf("Guardando la planificación a fichero.\n");
    char filename[256];
    printf("Indique el nombre de fichero: ");
    fflush(stdout);
    if (read_string(filename, sizeof(filename)) == INPUT_EOF) return;

    if (access(filename, F_OK) == 0) {
        if (access(filename, W_OK) != 0) {
            printf("El fichero existe, pero no se puede escribir.\n");
            return;
        }
        char ans[16];
        printf("El fichero existe, ¿quiere sobreescribirlo? (S/N): ");
        fflush(stdout);
        if (read_string(ans, sizeof(ans)) == INPUT_EOF) return;
        if (strcmp(ans, "S") != 0 && strcmp(ans, "s") != 0) {
            return;
        }
    }

    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Error al crear/abrir el fichero %s: %s\n", filename, strerror(errno));
        return;
    }

    for (int i = 0; i < col->number_occupied; i++) {
        if (col->array[i] != NULL) {
            file_record_t rec;
            memset(&rec, 0, sizeof(rec));
            strncpy(rec.user, col->array[i]->user, sizeof(rec.user) - 1);
            rec.P = col->array[i]->P;
            rec.C = col->array[i]->C;
            fwrite(&rec, sizeof(file_record_t), 1, f);
        }
    }

    fclose(f);
    printf("Planificación guardada en el fichero %s\n", filename);
}

static void handle_execute(struct collection *col) {
    if (col->number_occupied == 0) {
        printf("La planificación actual está vacía. No se puede ejecutar.\n");
        return;
    }

    int quantum = 1;
    printf("Introduzca el tamaño de la ranura temporal (quantum en segundos): ");
    fflush(stdout);
    int status = read_int(&quantum);
    if (status == INPUT_EOF) return;
    if (status == INPUT_INVALID || quantum <= 0) {
        fprintf(stderr, "Error: El quantum debe ser mayor que 0.\n");
        return;
    }

    printf("Ejecutando la planificación actual\n");
    handle_info_all(col);

    int num_procs = col->number_occupied;
    pid_t *real_pids = malloc(num_procs * sizeof(pid_t));
    int *c_rem = malloc(num_procs * sizeof(int));
    int *r_time = malloc(num_procs * sizeof(int));
    int *rtt_time = malloc(num_procs * sizeof(int));
    int *first_run = malloc(num_procs * sizeof(int));

    if (!real_pids || !c_rem || !r_time || !rtt_time || !first_run) {
        fprintf(stderr, "Error al asignar memoria dinámica para estadísticas.\n");
        free(real_pids); free(c_rem); free(r_time); free(rtt_time); free(first_run);
        return;
    }

    for (int i = 0; i < num_procs; i++) {
        c_rem[i] = (int)col->array[i]->C;
        r_time[i] = 0;
        rtt_time[i] = 0;
        first_run[i] = 0;
        col->array[i]->estado = READY;

        char pid_arg[16];
        snprintf(pid_arg, sizeof(pid_arg), "%d", col->array[i]->pid);

        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Error en fork: %s\n", strerror(errno));
            for (int j = 0; j < i; j++) {
                kill(real_pids[j], SIGTERM);
                waitpid(real_pids[j], NULL, 0);
            }
            free(real_pids); free(c_rem); free(r_time); free(rtt_time); free(first_run);
            return;
        }

        if (pid == 0) {
            execl("./proceso", "proceso", pid_arg, (char *)NULL);
            fprintf(stderr, "Error al ejecutar ./proceso con execl: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            real_pids[i] = pid;
        }
    }

    for (int i = 0; i < num_procs; i++) {
        kill(real_pids[i], SIGSTOP);
        col->array[i]->estado = STOPPED;
    }

    int active = num_procs;
    int current_time = 0;

    while (active > 0) {
        for (int i = 0; i < num_procs; i++) {
            if (c_rem[i] <= 0) continue;

            if (!first_run[i]) {
                r_time[i] = current_time;
                first_run[i] = 1;
            }

            col->array[i]->estado = RUNNING;
            kill(real_pids[i], SIGCONT);

            int run_time = (c_rem[i] < quantum) ? c_rem[i] : quantum;
            sleep(run_time);
            current_time += run_time;
            c_rem[i] -= run_time;

            if (c_rem[i] <= 0) {
                rtt_time[i] = current_time;
                col->array[i]->estado = TERMINATED;
                kill(real_pids[i], SIGTERM);
                waitpid(real_pids[i], NULL, 0);
                active--;
            } else {
                kill(real_pids[i], SIGSTOP);
                col->array[i]->estado = STOPPED;
            }
        }
    }

    printf("\nInformación de planificación:\n");
    printf("Quantum: %d\n", quantum);
    printf("Número de procesos: %d\n", num_procs);
    printf("  POS REAL_PID   PID     P     C     R   RTT      STATE OWNER               \n");
    for (int i = 0; i < num_procs; i++) {
        printf("%5d %8d %5d %5u %5u %5d %5d %10s %-20s\n",
               i, (int)real_pids[i], col->array[i]->pid, col->array[i]->P, col->array[i]->C,
               r_time[i], rtt_time[i], nombres_estado[col->array[i]->estado], col->array[i]->user);
    }

    int fifo_fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
    if (fifo_fd != -1) {
        dprintf(fifo_fd, "Información de planificación:\nQuantum: %d\nNúmero de procesos: %d\n", quantum, num_procs);
        for (int i = 0; i < num_procs; i++) {
            dprintf(fifo_fd, "%5d %8d %5d %5u %5u %5d %5d %10s %-20s\n",
                    i, (int)real_pids[i], col->array[i]->pid, col->array[i]->P, col->array[i]->C,
                    r_time[i], rtt_time[i], nombres_estado[col->array[i]->estado], col->array[i]->user);
        }
        close(fifo_fd);
    }

    free(real_pids);
    free(c_rem);
    free(r_time);
    free(rtt_time);
    free(first_run);
}
este es el de program.c
