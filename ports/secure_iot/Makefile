# Include the core environment definitions; this will set $(TOP).
#$(TOP) = ../..
include ../../py/mkenv.mk
#include $(TOP)/py/qstrdefs.mk
#CFLAGS += -I$(TOP)/py -I$(TOP)/extmod -I$(TOP)/shared

# Include py core make definitions.
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

QSTR_DEFS = \
  $(TOP)/py/qstrdefs.h \
  $(TOP)/ports/minimal_new/qstrdefsport.h

QSTR_GLOBAL_DEPENDENCIES = mpconfigport.h
CROSS_COMPILE ?= riscv64-unknown-elf-

# Set CFLAGS and libraries.
#CFLAGS += -march=rv64imac -mabi=lp64
#Incase of soft float enable
CFLAGS += -march=rv64imafd -mabi=lp64d
#CFLAGS += -mno-c
CFLAGS += -fno-builtin-printf
CFLAGS += -fno-builtin-memcpy
CFLAGS += -fno-builtin-malloc
CFLAGS += -Wall -Wcomment -Og -ggdb
CFLAGS += -mcmodel=medany -static -g
CFLAGS += -std=gnu99
CFLAGS += -fstack-protector-all -D_FORTIFY_SOURCE=2 -g
#For hard float enabled
CFLAGS += -I. -I$(BUILD) -I$(TOP)
# Add after existing CFLAGS
CFLAGS += -I$(TOP)/py -I$(TOP)/extmod -I$(TOP)/shared
#Since I have already put extmod I guess there is no need to add it separately.

#INC+= -I$(TOP)/lib/oofatfs
# Add after existing CFLAGS
#CFLAGS += -I$(TOP)
#CFLAGS += -I$(TOP)/py -I$(TOP)/extmod -I$(TOP)/shared
INC += -I$(TOP)/shared/runtime
#CFLAGS += -I$(TOP)/ports/minimal/build/genhdr
CFLAGS += -I$(BUILD)/genhdr
CFLAGS += $(INC)
LIBS += -lm

# Define the required source files.
#	../../py/objcomplex.c \
	../../py/objfloat.c \ --> Linking two times in the source it is aldready included in the source files

SRC_C = \
    $(PY_CORE_SRC) \
    $(EXTMOD_SRC) \
    $(SHARED_SRC_C) \
	py/modbuiltins.c \
	py/modsys.c \
	py/modgc.c \
	py/modmicropython.c \
	main.c \
	mphalport.c \
	time.c \
	delay.c \
	machine_uart.c \
	machine_wdt.c \
	machine_pin_new.c \
	machine_i2c.c \
	machine_spi.c \
	machine_pwm.c \
	modmachine.c \
	modtime.c \
	random_module.c \
	uart_driver.c \
	i2c_driver.c \
	RTC_driver.c \
	pinmux_driver.c \
	uart_core.c \
	util.c \
	plic_driver.c \
	init.c \
	log.c \
	traps.c \
	stub.c \
	reader_stub.c \
	io.c \
	dummy_io.c\
	wdtimer_driver.c \
	gpio_driver.c \
	gptimer_driver.c \
	spi_driver.c \
	pwm_driver.c \
	shared/readline/readline.c \
	shared/runtime/gchelper_generic.c \
	shared/runtime/pyexec.c \
	shared/runtime/sys_stdio_mphal.c \
	shared/runtime/stdout_helpers.c \
	string_functions.c \
	math_functions.c \
	memory_functions.c \
	module.c \
	scipy/integrate/integrate.c \
	scipy/linalg/linalg.c \
	scipy/optimize/optimize.c \
	scipy/signal/signal.c \
	scipy/special/special.c \
	ndarray_operators.c \
	ulab.c \
	ulab_tools.c \
	ndarray.c \
	numpy/ndarray/ndarray_iter.c \
	ndarray_properties.c \
	numpy/approx.c \
	numpy/bitwise.c \
	numpy/compare.c \
	numpy/carray/carray.c \
	numpy/carray/carray_tools.c \
	numpy/create.c \
	numpy/fft/fft.c \
	numpy/fft/fft_tools.c \
	numpy/filter.c \
	numpy/io/io.c \
	numpy/linalg/linalg_tools.c \
	numpy/linalg/linalg.c \
	numpy/numerical.c \
	numpy/poly.c \
	numpy/random/random.c \
	numpy/stats.c \
	numpy/transform.c \
	numpy/vector.c \
	numpy/numpy.c \
	scipy/scipy.c \
	scipy/integrate/integrate.c \
	user/user.c \
	utils/utils.c \
	py/modmath.c \

#no issues with adding modmath module as well --> for math functionality
#	py/objcomplex.c \

#SRC_C += genhdr/qstrdefs.generated.c
#SOURCES += genhdr/qstrdefs.generated.c
#SRC_C += $(BUILD)/genhdr/qstrdefs.generated.c //not needed pola
# SRC_C += $(PY_CORE_SRC)
# SRC_C += $(EXTMOD_SRC)
#SRC_C += ../../py/modbuiltins.c -->redundant
#SRC_C += $(TOP)/py/modio.c #--> 2 new files due to change in version

LIBS += -lgcc

SRC_S = start.S trap.S

LDFLAGS += -T link_ram.ld  -Wl,--gc-sections # Use custom linker script
LDFLAGS += -L$(shell dirname $(shell $(CC) -print-libgcc-file-name))

# Source files containing qstrs.
#SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c
SRC_QSTR += ../../ports/minimal_new/qstrdefsport.h
SRC_QSTR += \
    $(PY_CORE_SRC) \
    $(EXTMOD_SRC) \
    $(SHARED_SRC_C) \
	py/modbuiltins.c \
	py/objexcept.c  \
	py/modsys.c \
	py/modgc.c \
	py/modmicropython.c \
	main.c \
	mphalport.c \
	time.c \
	delay.c \
	machine_uart.c \
	machine_wdt.c \
	machine_pin_new.c \
	machine_i2c.c \
	machine_spi.c \
	machine_pwm.c \
	modmachine.c \
	modtime.c \
	random_module.c \
	uart_driver.c \
	i2c_driver.c \
	RTC_driver.c \
	spi_driver.c \
	gptimer_driver.c \
	pinmux_driver.c \
	uart_core.c \
	util.c \
	plic_driver.c \
	init.c \
	log.c \
	traps.c \
	stub.c \
	reader_stub.c \
	io.c \
	string_functions.c \
	math_functions.c \
	memory_functions.c \
	module.c \
	wdtimer_driver.c \
	gpio_driver.c \
	pwm_driver.c \
	scipy/integrate/integrate.c \
	scipy/linalg/linalg.c \
	scipy/optimize/optimize.c \
	scipy/signal/signal.c \
	scipy/special/special.c \
	ndarray_operators.c \
	ulab_tools.c \
	ndarray.c \
	numpy/ndarray/ndarray_iter.c \
	ndarray_properties.c \
	numpy/approx.c \
	numpy/bitwise.c \
	numpy/compare.c \
	numpy/carray/carray.c \
	numpy/carray/carray_tools.c \
	numpy/create.c \
	numpy/fft/fft.c \
	numpy/fft/fft_tools.c \
	numpy/filter.c \
	numpy/io/io.c \
	numpy/linalg/linalg_tools.c \
	numpy/linalg/linalg.c \
	numpy/numerical.c \
	numpy/poly.c \
	numpy/random/random.c \
	numpy/stats.c \
	numpy/transform.c \
	numpy/vector.c \
	numpy/numpy.c \
	scipy/scipy.c \
	ports/minimal_new/scipy/integrate/integrate.c \
	user/user.c \
	utils/utils.c \
	ulab.c \
	../../py/objcomplex.c \
	../../py/objfloat.c \
	py/modmath.c \
		
#QSTR_GLOBAL_DEPENDENCIES = mpconfigport.h

LIBS += -lm -lgcc

SHARED_SRC_C = $(addprefix shared/,\
	libc/__errno.c \
	libc/string0.c \
	netutils/netutils.c \
	readline/readline.c \
	runtime/interrupt_char.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	timeutils/timeutils.c \
	)

CFLAGS += -I./numpy
CFLAGS += -I./numpy/ndarray
CFLAGS += -I./scipy
CFLAGS += -I./utils

OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
# Define object files.
OBJ = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.S=.o)) #| $(BUILD)/genhdr/qstrdefs.generated.h

include $(TOP)/py/mkrules.mk 
# # Add this to make sure qstrdefs.generated.c is built
# $(BUILD)/genhdr/qstrdefs.generated.c: $(BUILD)/genhdr/qstrdefs.generated.h
# 	python3 ../../py/makeqstrdata.py $< > $@
build/genhdr/qstrdefs.generated.c: build/genhdr/qstrdefs.preprocessed.h
	python3 ../../py/makeqstrdata.py build/genhdr/qstrdefs.preprocessed.h > build/genhdr/qstrdefs.generated.c
# Top-level target: build ELF and BIN
all: $(BUILD)/firmware.elf $(BUILD)/firmware.bin 
# Link firmware.elf
$(BUILD)/firmware.elf: $(OBJ) #$(BUILD)/genhdr/qstrdefs.generated.c
	echo $(TOP)
	$(ECHO) "LINK $@"
	$(Q)$(CC) -nostartfiles $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

# Generate binary image for flashing
$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $< $@

# Run using QEMU
run: $(BUILD)/firmware.elf
	@echo "Ctrl-A C for QEMU console, then 'quit' to exit"
	qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -kernel $(BUILD)/firmware.elf

# Clean build files
clean:
	$(Q)rm -f $(OBJ) $(BUILD)/firmware.elf $(BUILD)/firmware.bin 
#$(BUILD)/genhdr/qstrdefs.generated.h $(BUILD)/genhdr/qstrdefs.collected.hash

# Size of firmware
size: $(BUILD)/firmware.elf
	@echo "Finding the size of the file firmware.elf"
	size $(BUILD)/firmware.elf


# dump: $(BUILD)/firmware.elf
#     @echo "Generating The obj.dump file from firmware.elf code"
#     riscv64-unknown-elf-objdump -D -S $(BUILD)/firmware.elf >> $(BUILD)/firmware.dump
#     cat $(BUILD)/firmware.dump

# # Clean build files
# clean:
#  	$(Q)rm -f $(OBJ) $(BUILD)/firmware.elf $(BUILD)/firmware.bin

# size: $(BUILD)/firmware.elf
#     @echo "Finding the size of the file firmware.elf"
#     size $(BUILD)/firmware.elf I