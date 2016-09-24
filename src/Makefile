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

TARGET_ELF=${BUILD_DIR}/result.elf
TARGET_BIN=${BUILD_DIR}/result.bin

INCLUDE_DIRS=\
	./lsm6-arduino/ \
	./MahonyAHRS/src/ \
	${ARDUINO_PACKAGES}/arduino/tools/CMSIS/4.0.0-atmel/CMSIS/Include/ \
	${ARDUINO_PACKAGES}/arduino/tools/CMSIS/4.0.0-atmel/Device/ATMEL/ \
	${CORE_PATH} \
	${VARIANTS_PATH} \
	${LIBRARIES_PATH}/Wire/ \
	${LIBRARIES_PATH}/SPI/ \
	${ARDUINO_LIBS_PATH}/SD/src/
INCLUDE_FLAGS=$(addprefix -I,${INCLUDE_DIRS})

DEFINES=-DF_CPU=48000000L -DARDUINO=10611 -DARDUINO_SAMD_FEATHER_M0 -DARDUINO_ARCH_SAMD -DARDUINO_SAMD_ZERO -D__SAMD21G18A__ -DUSB_VID=0x239A -DUSB_PID=0x800B -DUSBCON '-DUSB_MANUFACTURER="Adafruit"' '-DUSB_PRODUCT="Feather M0"'

OPTIMISE_FLAGS=-mcpu=cortex-m0plus -mthumb -Os
WARNING_FLAGS=-Wall -Wextra
DEBUG_FLAGS=-g
COMMON_FLAGS=${OPTIMISE_FLAGS} ${DEBUG_FLAGS} ${WARNING_FLAGS} -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 ${DEFINES} ${INCLUDE_FLAGS} -MMD -MP

ASMFLAGS=${DEBUG_FLAGS} -x assembler-with-cpp ${DEFINES} ${INCLUDE_FLAGS}
CFLAGS=-std=gnu11 ${COMMON_FLAGS}
CXXFLAGS=-std=gnu++11 ${COMMON_FLAGS} -fno-threadsafe-statics -fno-rtti -fno-exceptions
LDFLAGS=${OPTIMISE_FLAGS} -Wl,--gc-sections -save-temps "-T${VARIANTS_PATH}/linker_scripts/gcc/flash_with_bootloader.ld" --specs=nano.specs --specs=nosys.specs -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align # -Wl,--cref


SRC_DIRS=\
	./Slim \
	./lsm6-arduino \
	./MahonyAHRS/src \
	${CORE_PATH} \
	${VARIANTS_PATH} \
	${LIBRARIES_PATH}/Wire/ \
	${LIBRARIES_PATH}/SPI/ \
	${ARDUINO_LIBS_PATH}/SD/src/

SRC_FILES=$(shell find ${SRC_DIRS} -name \*.S -or -name \*.c -or -name \*.cpp)

OBJ_FILES=$(SRC_FILES:%=${BUILD_DIR}/%.o)
DEPENDENCIES=${OBJ_FILES:.o=.d}


all: bin
compile: ${OBJ_FILES}
link: ${TARGET_ELF}
bin: ${TARGET_BIN}

${TARGET_ELF}: ${OBJ_FILES}
	${CC} ${LDFLAGS} -Wl,--start-group -lm $^ -Wl,--end-group -o $@

${TARGET_BIN}: ${TARGET_ELF}
	${OBJCOPY} -O binary $< $@

clean:
	rm -rf ${BUILD_DIR}


${BUILD_DIR}/%.S.o: %.S
	mkdir -p $(dir $@)
	${CC} ${ASMFLAGS} -c $< -o $@

${BUILD_DIR}/%.c.o: %.c
	mkdir -p $(dir $@)
	${CC} ${CFLAGS} -c $< -o $@

${BUILD_DIR}/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} -c $< -o $@


-include ${DEPENDENCIES}


program: bin
	stty -F ${PORT_DEVICE} 1200
	sleep .5
	while [[ ! -c ${PORT_DEVICE} ]]; do sleep .1; done
	${BOSSAC} -i -d --port=${PORT} -U true -i -e -w -v ${TARGET_BIN} -R

serial: program
	sleep 2
	stty -F ${PORT_DEVICE} cs8 ${SERIAL_BAUD} ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
	tail -f ${PORT_DEVICE} | tee log.txt


.PHONY: all, program, serial, clean, compile, link, bin
