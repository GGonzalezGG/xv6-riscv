# Informe tarea 1 Sistemas operativos
#### Guillermo González

## Funcionamiento de las llamadas al sistema

Para esta tarea agregamos 2 llamadas al sistema, que funcionan de la siguiente forma:

### getppid() - get parent process id
Obtiene la id del padre del proceso en el cual estamos y la retorna.
Solo revisa al proceso padre y devuelve su id, si no exite retorna -1.

### getancestor()
Devuelve el ancestro por nivel, si el nivel es 0, da la id del proceso actual, si es 1 el padre, 2 el abuelo y así sucesivamente. Si ya no queda ancestro devuelve -1.

## Explicación de las modificaciones realizadas

### Modificaciones de sistema

#### syscall.h
Agregamos las referencias a las nuevas llamadas, para que estas puedan ser utilzadas y referenciadas desde otras partes del sistema en base a la modularidad.

```
#define SYS_getppid 22 //agregamos referencia a nueva llamada get parent process id
#define SYS_getancestor 22 //referencia a getancestor
```

#### syscall.c
Agregamos las nuevas llamadas al archivo que maneja todas las llamadas al sistema, para que estas se puedan comunicar con el sistema y ser ejecutadas. Agregamos a un array que nos trae las funciones declaradas desde syscall.h para que luego se puedan manejar desde la función.

```
// Prototypes for the functions that handle system calls.
(...)
extern uint64 sys_getppid(void); //externalizamos getppid
extern uint64 sys_getancestor(void); //externalizamos getancestor
(...)

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
static uint64 (*syscalls[])(void) = {
(...)
[SYS_getpid]  sys_getpid,
[SYS_getppid] sys_getppid,
[SYS_getancestor] sys_getancestor,
(...)
}
```

#### sysproc.c
Aquí agregamos las funciones y la lógica de las nuevas llamadas que agregamos.

```
uint64
sys_getppid(void)
{
  struct proc *p = myproc();  // Obtener el proceso actual
  
  // Verificar que el proceso tenga un padre válido
  if(p->parent == 0)
    return -1;  // No tiene padre (es init)
    
  return p->parent->pid;  // Retornar el PID del padre
}

//agregamos getancestor
uint64
sys_getancestor(void) 
{
    int level;
    
    // argint() retorna void, así que no verificamos su retorno
    argint(0, &level);
    
    struct proc *p = myproc();
    
    if(level < 0)
        return -1;
    
    if(level == 0)
        return p->pid;
    
    struct proc *current = p;
    for(int i = 0; i < level; i++) {
        if(current->parent == 0)
            return -1;
        current = current->parent;
    }
    
    return current->pid;
}
```

## Modificaciones de usuario

#### user.h
Agregamos las funciones en conjunto con las otras que se utilizan para hacer llamadas al sistema. Así los usuarios podrán hacer llamadas y utilizar funciones de sistema utilizando la función de forma correcta.
```
int getppid(void); //agregamos referencia para que pueda llamar a getppid
int getancestor(int); //agregamos referencia a getancestor
```

#### usys.pl
Agregamos entradas para que el apartado del usuario se pueda comunicar bien con el kernel, aqui se agrega el prefijo de sys_ que se utiliza en syscall.c para que estos se comuniquen de forma efectiva y se pueda hacer la llamada de forma correcta.

```
entry("getppid"); #Agregamos referencia para que usuarios puedan comunicarse con getppid
entry("getancestor"); #agregamos referencia a getancestor
```

## Dificultades encontradas

Mi primera complicación fue al hacer getppid, que en mi primer intento no compiló, ya que me faltó el ; después de entry("getppid), lo agregué y funcionó sin problemas.

Luego, tuve otra dificutad, al intentar declarar y hacer la función de getancestor(), puesto que en syscall.c la declaré como:

```
extern uint64 sys_getancestor(int); //externalizamos getancestor
```
Y de forma similar hice que recibiera el argumento directamente en sysproc.c. Esto no funcionaba, puesto que xv6 pide que todas las llamadas reciban void, por lo que tuve que cambiarlo recibiendo el argumento utilizando la función argint(), para poder aceptar distintos niveles.
Al utilizar argint() intenté hacer una comparación directamente con esta, la cual también retornaba void por lo que dio otro error, pero saque la comparación y deje que la función solo extrayera el argumento, dejando la lógica en condicionales, y de esta forma funciono bien.
En resumen, cambie el int por un void en syscall.c y cambie la forma en la que getancestor() recibía argumentos en sysproc.c, utilizando ahora argint(), y de esta forma solucioné las dificultades encontradas en la tarea.