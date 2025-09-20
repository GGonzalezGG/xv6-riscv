#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("=== Prueba de llamadas al sistema ===\n");
  
  // Obtener PID del proceso actual
  int mypid = getpid();
  printf("Mi PID: %d\n", mypid);
  
  // Probar getppid()
  int ppid = getppid();
  if(ppid != -1) {
    printf("PID de mi padre: %d\n", ppid);
  } else {
    printf("No tengo padre (soy init)\n");
  }
  
  // Crear un proceso hijo para probar mejor
  int pid = fork();
  if(pid == 0) {
    // Proceso hijo
    printf("\n=== Desde proceso hijo ===\n");
    printf("Mi PID (hijo): %d\n", getpid());
    printf("Mi padre: %d\n", getppid());
    exit(0);
  } else {
    // Proceso padre
    wait(0);  // Esperar al hijo
  }
  
  printf("\nPrueba completada!\n");
  exit(0);
}