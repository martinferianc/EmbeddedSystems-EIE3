echo "========================================"
echo "Connecting to the board"
ls /dev/{tty,cu}.*
PORT=$1
BAUD_RATE=$2
screen $1 $2
