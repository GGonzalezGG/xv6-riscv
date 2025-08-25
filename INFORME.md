Pasos previos:
- Instalar Ubuntu en WSL (yo no lo descargo porque ya lo tengo)
- Hacer un fork y clonar repositorio xv6-public por error

# Pasos instalación xv6-riscv:

1. Crear un fork desde github a https://github.com/mit-pdos/xv6-riscv hacía mi cuenta
2. Clonar desde mi fork https://github.com/GGonzalezGG/xv6-riscv
3. Instalar las siguientes dependencias:
sudo apt-get install make qemu-system-misc bc gcc-riscv64-linux-gnu
4. xv6-riscv instalado, ejecutar con: make qemu

# Problemas y soluciones encontrados

5. make qemu lanza error, se necesita versión 7.2 y sistema descargó 6.2
6. sudo apt upgrade, sudo qemu update, etc.
7. Error se mantiene, llorar
8. Descargar la versión de qemu necesaria de forma directa con:

sudo apt-get update
sudo apt-get install -y git build-essential libglib2.0-dev libpixman-1-dev zlib1g-dev ninja-build
git clone https://gitlab.com/qemu-project/qemu.git
cd qemu
git checkout v7.2.0   # Forzar versión compatible
./configure --target-list=riscv64-softmmu,riscv64-linux-user
make -j$(nproc)
sudo make install

9. Borrar la versión anterior con:

sudo apt remove --purge qemu-system-misc qemu-system qemu-utils

# Confirmación de que xv6 está funcionando correctamente

10. make qemu ejecuta el sistema operativo y funcionan los comandos de prueba

$ ls
$ echo "Hola xv6"
$ cat README

Comandos se ejecutan correctamente, tarea completada :D

# Fin de la tarea 0 - Guillermo González
