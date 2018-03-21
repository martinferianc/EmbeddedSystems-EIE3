echo "========================================"
echo "Compiling the code"
mbed-cli compile --profile=build_profile1.json
echo "========================================"
echo "Copying the code"
cp BUILD/NUCLEO_F303K8/GCC_ARM/src.bin /dev/tty.usbmodem1423/src.bin
echo "========================================"
echo "Running the code"
screen /dev/tty.usbmodem1423 9600
