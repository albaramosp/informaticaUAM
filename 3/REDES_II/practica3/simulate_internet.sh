
#!/bin/bash

# Script para simular los retrasos y pérdidas de paquetes en la interfaz de loopback
# Hecho por Carlos Ruiz Pastor

usage()
{
    echo "usage: sysinfo_page [[[-d delay ] [-l loss_per ] [-c corrupt_per ] | [-h]]"
}


##### Main

DELAY=10
JITTER=100
LOSS_PER=10
CORRUPT_PER=5
NET_INT=lo

while [ "$1" != "" ]; do
    case $1 in
        -d | --delay )          shift
                                DELAY=$1
                                ;;
        -j | --jitter )          shift
                                JITTER=$1
                                ;;
        -l | --loss )           shift
                                LOSS_PER=$1
                                ;;
        -c | --corrupt )        shift
                                CORRUPT_PER=$1
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage
                                exit 1
    esac
    shift
done

COMMAND="tc qdisc add dev ${NET_INT} root netem"

if (( $DELAY > 0 && $JITTER > 0 )); then
    COMMAND="$COMMAND delay ${DELAY}ms ${JITTER}ms"
fi

if (( $LOSS_PER > 0 )); then
    COMMAND="$COMMAND loss $LOSS_PER%"
fi

if (( $CORRUPT_PER > 0 )); then
    COMMAND="$COMMAND corrupt $CORRUPT_PER%"
fi

# Anade regla
# echo "Añadimos la siguiente regla: ${COMMAND}"
eval $COMMAND

# Muestra las reglas en uso:
echo "Reglas en uso:"
tc qdisc show dev $NET_INT

# Esperamos a una pulsación para terminar
read -n1 -r -p "Press any key to finish..." key
echo ""

# Elimina las reglas que hemos puesto sobre la interfaz
echo "Eliminamos las reglas añadidas"
tc qdisc del dev $NET_INT root

# Muestra las reglas en uso:
echo "Reglas en uso (por defecto en el sistema)"
tc qdisc show dev $NET_INT