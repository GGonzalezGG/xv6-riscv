#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 1. Usar N=10 para cumplir con el mínimo del enunciado 
#define N 10
#define DURATION_SECONDS 5

struct result {
  int id;
  int tickets;
  int slices;
};

/*
 * busywork: Función que consume CPU durante un tiempo determinado
 * y reporta sus métricas por un pipe.
 */
void
busywork(int id, int tickets, int duration_seconds, int pipe_fd)
{
  int start_time, current_time;
  volatile int sum = 0; // volatile para evitar que el compilador lo optimice
  int i, j;
  int slices_start, slices_end;
  struct result res;
  
  slices_start = getslices(); 
  start_time = uptime(); // uptime() devuelve 'ticks' del sistema
  
  while(1) {
    current_time = uptime();
    // 10 'ticks' por segundo en xv6-riscv
    if((current_time - start_time) >= duration_seconds * 10) { 
      break;
    }
    
    // Trabajo arbitrario para consumir CPU
    for(i = 0; i < 1000; i++) {
      for(j = 0; j < 100; j++) {
        sum += i * j;
        sum = sum * 3;
        sum = sum / 3;
      }
    }
  }
  
  slices_end = getslices();
  
  // Preparar y enviar resultados al padre
  res.id = id;
  res.tickets = tickets;
  res.slices = slices_end - slices_start;
  
  write(pipe_fd, &res, sizeof(res));
  close(pipe_fd);
}

int
main(void)
{
  int i, pid;
  int pipes[N][2];
  struct result results[N];
  int total_tickets = 0;
  int total_slices = 0;
  
  printf("Lottery Scheduling Demo (N=%d)\n", N);
  printf("================================\n\n");
  
  // --- Creación de Procesos Hijos ---
  for(i = 0; i < N; i++) {
    if(pipe(pipes[i]) < 0) {
      printf("pipe error\n");
      exit(1);
    }

    // 2. Usar la fórmula del enunciado para asignar tickets 
    int tickets = 50 * (i + 1); 

    pid = fork();

    if(pid < 0) {
      printf("fork error\n");
      exit(1);
    }
    
    if(pid == 0) {
      // --- Código del Hijo ---
      close(pipes[i][0]); // Cerrar lado de lectura del pipe
      settickets(tickets); // Asignar tickets usando la nueva syscall
      busywork(i, tickets, DURATION_SECONDS, pipes[i][1]);
      exit(0);
    }
    // --- Código del Padre ---
    close(pipes[i][1]); // Cerrar lado de escritura del pipe
  }
  
  // --- Recolección de Resultados ---
  for(i = 0; i < N; i++) {
    wait(0); // Esperar a que termine un hijo
    // Leer el struct 'result' desde el pipe
    read(pipes[i][0], &results[i], sizeof(struct result)); 
    close(pipes[i][0]);
  }
  
  // --- Cálculo de Totales ---
  for(i = 0; i < N; i++) {
    total_tickets += results[i].tickets;
    total_slices += results[i].slices;
  }
  
  // --- Impresión de la Tabla ---
  printf("\nRESULTADOS (Total Slices: %d):\n", total_slices);
  printf("+----+---------+--------+----------+----------+\n");
  printf("| ID | Tickets | Slices | Real pct | Esper pct|\n");
  printf("+----+---------+--------+----------+----------+\n");
  
  // 3. REPARACIÓN DE ERROR (División por cero)
  if (total_slices == 0 || total_tickets == 0) {
    printf("|          ERROR: total_slices o total_tickets es 0          |\n");
  } else {
    // Imprimir fila por cada proceso
    for(i = 0; i < N; i++) {
      int actual_pct = (results[i].slices * 100) / total_slices;
      int expected_pct = (results[i].tickets * 100) / total_tickets;
      
      // Usar %d simple, sin anchos (%2d), para compatibilidad con xv6
      printf("| %d | %d | %d | %d | %d |\n",
           results[i].id, 
           results[i].tickets, 
           results[i].slices,
           actual_pct,
           expected_pct);
    }
  }
  printf("+----+---------+--------+----------+----------+\n\n");
  
  // --- Análisis de Proporcionalidad ---
  // Compara el proceso con menos tickets (ID 0) vs el que tiene más (ID N-1)
  printf("ANALISIS (ID 0 vs ID %d):\n", N-1);
  struct result first = results[0];
  struct result last = results[N-1];

  printf("  ID %d: %d tickets -> %d slices\n", first.id, first.tickets, first.slices);
  printf("  ID %d: %d tickets -> %d slices\n", last.id, last.tickets, last.slices);

  // 4. REPARACIÓN DE ERROR (División por cero)
  // Verificar que el proceso con menos tickets tenga slices antes de dividir
  if (first.slices == 0) {
    printf("\n  [WARN] No se puede calcular ratio real: ID 0 tiene 0 slices.\n");
    printf("  [WARN] Aumente DURATION_SECONDS para un mejor test.\n");
  } else {
    // Calcular ratio con 1 decimal (multiplicando por 10)
    int ratio_real = (last.slices * 10) / first.slices; 
    int ratio_esperado = (last.tickets * 10) / first.tickets;
  
    printf("  Ratio real:     %d.%d:1\n", ratio_real/10, ratio_real%10);
    printf("  Ratio esperado: %d.%d:1\n", ratio_esperado/10, ratio_esperado%10);
  
    int diff = ratio_real - ratio_esperado;
    if(diff < 0) diff = -diff;
  
    // Usar una tolerancia (ej. 25% del ratio esperado)
    if(diff <= (ratio_esperado / 4)) { 
      printf("  [OK] Proporcionalidad correcta dentro de la tolerancia.\n");
    } else {
      printf("  [WARN] Desviacion detectada.\n");
    }
  }
  
  exit(0);
}