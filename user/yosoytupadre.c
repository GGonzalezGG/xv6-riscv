#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("=== Prueba de llamadas al sistema de getppid===\n");
  
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
  

  printf("\n=== Prueba de llamadas al sistema de getancestor===\n");
  // Crear un proceso hijo para probar mejor
  int pid = fork();
  if(pid == 0) {
    // Proceso hijo
    printf("\n=== Desde proceso hijo ===\n");
    printf("Mi PID (hijo) (getancestor(0)): %d\n", getancestor(0));
    printf("Mi padre (getancestor(1)): %d\n", getancestor(1));
    printf("Mi abuelo (getancestor(2)): %d\n", getancestor(2));
    printf("Mi tatara-abuelo (getancestor(3)): %d\n", getancestor(3));
    printf("Mi abuelo (getancestor(4)): %d\n", getancestor(4)); // Debería retornar -1 si no hay más ancestros
    printf("Nivel inválido (getancestor(-1)): %d\n", getancestor(-1));
    printf("Nivel muy alto (getancestor(100)): %d\n", getancestor(100));
    exit(0);
  } else {
    // Proceso padre
    wait(0);  // Esperar al hijo
  }
  
  printf("\nPrueba completada!\n");
  exit(0);
}