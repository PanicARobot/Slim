ARDUINO=/home/cuki/robotics/arduino-1.6.11

ARDUINO_PACKAGES=/home/cuki/.arduino15/packages
TOOLCHAIN_PATH=${ARDUINO_PACKAGES}/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1

BOSSAC=${ARDUINO_PACKAGES}/arduino/tools/bossac/1.6.1-arduino/bossac
PORT=ttyACM0
PORT_DEVICE=/dev/${PORT}
SERIAL_BAUD=115200

CC=${TOOLCHAIN_PATH}/bin/arm-none-eabi-gcc
CXX=${TOOLCHAIN_PATH}/bin/arm-none-eabi-g++
AR=${TOOLCHAIN_PATH}/bin/arm-none-eabi-ar
OBJCOPY=${TOOLCHAIN_PATH}/bin/arm-none-eabi-objcopy

CORE_PATH=${ARDUINO_PACKAGES}/adafruit/hardware/samd/1.0.13/cores/arduino
VARIANTS_PATH=${ARDUINO_PACKAGES}/adafruit/hardware/samd/1.0.13/variants/arduino_zero
LIBRARIES_PATH=${ARDUINO_PACKAGES}/adafruit/hardware/samd/1.0.13/libraries

ARDUINO_LIBS_PATH=${ARDUINO}/libraries

BUILD_DIR=build

OUTPUT_ELF=${BUILD_DIR}/result.elf
OUTPUT_BIN=${BUILD_DIR}/result.bin


INCLUDES=\
	-I./lsm6-arduino/ \
	-I./MahonyAHRS/src/ \
	-I${ARDUINO_PACKAGES}/arduino/tools/CMSIS/4.0.0-atmel/CMSIS/Include/ \
	-I${ARDUINO_PACKAGES}/arduino/tools/CMSIS/4.0.0-atmel/Device/ATMEL/ \
	-I${CORE_PATH} \
	-I${VARIANTS_PATH} \
	-I${LIBRARIES_PATH}/Wire/ \
	-I${LIBRARIES_PATH}/SPI/ \
	-I${ARDUINO_LIBS_PATH}/SD/src/

DEFINES=-DF_CPU=48000000L -DARDUINO=10611 -DARDUINO_SAMD_FEATHER_M0 -DARDUINO_ARCH_SAMD -DARDUINO_SAMD_ZERO -D__SAMD21G18A__ -DUSB_VID=0x239A -DUSB_PID=0x800B -DUSBCON '-DUSB_MANUFACTURER="Adafruit"' '-DUSB_PRODUCT="Feather M0"'

OPTIMISE_FLAGS=-mcpu=cortex-m0plus -mthumb -Os
WARNING_FLAGS=-Wall -Wextra
DEBUG_FLAGS=-g
COMMON_FLAGS=${OPTIMISE_FLAGS} ${DEBUG_FLAGS} ${WARNING_FLAGS} -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500

ASMFLAGS=${DEBUG_FLAGS} -x assembler-with-cpp ${DEFINES} ${INCLUDES}
CFLAGS=-std=gnu11 ${COMMON_FLAGS} ${DEFINES} ${INCLUDES}
CXXFLAGS=-std=gnu++11 ${COMMON_FLAGS} -fno-threadsafe-statics -fno-rtti -fno-exceptions ${DEFINES} ${INCLUDES}

LDFLAGS=${OPTIMISE_FLAGS} -Os -Wl,--gc-sections -save-temps "-T${VARIANTS_PATH}/linker_scripts/gcc/flash_with_bootloader.ld" --specs=nano.specs --specs=nosys.specs -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align # -Wl,--cref

ASM_CORES=pulse_asm.S

C_CORES=\
	WInterrupts.c \
	cortex_handlers.c \
	delay.c \
	hooks.c \
	itoa.c \
	pulse.c \
	startup.c \
	wiring.c \
	wiring_analog.c \
	wiring_digital.c \
	wiring_private.c \
	wiring_shift.c \
	USB/samd21_host.c \
	avr/dtostrf.c

CPP_CORES=\
	IPAddress.cpp \
	Print.cpp \
	Reset.cpp \
	RingBuffer.cpp \
	SERCOM.cpp \
	Stream.cpp \
	Tone.cpp \
	Uart.cpp \
	WMath.cpp \
	WString.cpp \
	abi.cpp \
	main.cpp \
	new.cpp \
	USB/CDC.cpp \
	USB/PluggableUSB.cpp \
	USB/USBCore.cpp

CPP_VARIANTS=variant.cpp

CPP_LIBS=\
	Wire/Wire.cpp \
	SPI/SPI.cpp

CPP_ARDUINO_LIBS=\
	SD/src/SD.cpp \
	SD/src/File.cpp \
	SD/src/utility/SdFile.cpp \
	SD/src/utility/SdVolume.cpp \
	SD/src/utility/Sd2Card.cpp

CPP_FILES=\
	oscore.cpp\
	OrientationSensorsWrapper.cpp \
	SDLogDriver.cpp \
	lsm6-arduino/LSM6.cpp \
	MahonyAHRS/src/MahonyAHRS.cpp

OBJ_FILES=\
	$(addprefix ${BUILD_DIR}/,${CPP_FILES:.cpp=.o}) \
	$(addprefix ${BUILD_DIR}/variants/,${CPP_VARIANTS:.cpp=.o}) \
	$(addprefix ${BUILD_DIR}/cores/,${ASM_CORES:.S=.o} ${C_CORES:.c=.o} ${CPP_CORES:.cpp=.o}) \
	$(addprefix ${BUILD_DIR}/libs/,${CPP_LIBS:.cpp=.o}) \
	$(addprefix ${BUILD_DIR}/arduino_libs/,${CPP_ARDUINO_LIBS:.cpp=.o})






all: bin

clean:
	rm -rf ${BUILD_DIR}

compile: ${OBJ_FILES}

link: ${OUTPUT_ELF}

bin: ${OUTPUT_BIN}

program: bin
	stty -F ${PORT_DEVICE} 1200
	sleep .5
	while [[ ! -c ${PORT_DEVICE} ]]; do sleep .1; done
	${BOSSAC} -i -d --port=${PORT} -U true -i -e -w -v ${OUTPUT_BIN} -R 

serial:
	stty -F ${PORT_DEVICE} cs8 ${SERIAL_BAUD} ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
	tail -f ${PORT_DEVICE} # Manual flushes may me needed


# Building source
${BUILD_DIR}/%.o: %.cpp
	mkdir -p $(shell dirname $@)
	${CXX} ${CXXFLAGS} -c $< -o $@

# Building libraries
${BUILD_DIR}/cores/%.o: ${CORE_PATH}/%.S
	mkdir -p $(shell dirname $@)
	${CC} ${ASMFLAGS} -c $< -o $@

${BUILD_DIR}/cores/%.o: ${CORE_PATH}/%.c
	mkdir -p $(shell dirname $@)
	${CC} ${CFLAGS} -c $< -o $@

${BUILD_DIR}/cores/%.o: ${CORE_PATH}/%.cpp
	mkdir -p $(shell dirname $@)
	${CXX} ${CXXFLAGS} -c $< -o $@

${BUILD_DIR}/libs/%.o: ${LIBRARIES_PATH}/%.cpp
	mkdir -p $(shell dirname $@)
	${CXX} ${CXXFLAGS} -c $< -o $@

${BUILD_DIR}/arduino_libs/%.o: ${ARDUINO_LIBS_PATH}/%.cpp
	mkdir -p $(shell dirname $@)
	${CXX} ${CXXFLAGS} -c $< -o $@

${BUILD_DIR}/variants/%.o: ${VARIANTS_PATH}/%.cpp
	mkdir -p $(shell dirname $@)
	${CXX} ${CXXFLAGS} -c $< -o $@

${OUTPUT_ELF}: compile
	${CC} ${LDFLAGS} -Wl,--start-group -lm ${OBJ_FILES} -Wl,--end-group -o $@

${OUTPUT_BIN}: ${OUTPUT_ELF}
	${OBJCOPY} -O binary $< $@


.PHONY: all, program, clean, compile, link, bin
