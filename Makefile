##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [4.3.0-B58] date: [Sun Jun 09 13:15:59 CEST 2024] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = WB55_Zigbee_Srv


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/main.c \
Core/Src/app_threadx.c \
Core/Src/app_entry.c \
Core/Src/hw_timerserver.c \
Core/Src/stm32_lpm_if.c \
Core/Src/stm_logging.c \
Core/Src/stm32wbxx_it.c \
Core/Src/stm32wbxx_hal_msp.c \
Core/Src/stm32wbxx_hal_timebase_tim.c \
STM32_WPAN/App/app_zigbee.c \
STM32_WPAN/Target/hw_ipcc.c \
AZURE_RTOS/App/app_azure_rtos.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_hsem.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rcc.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc_ex.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash_ex.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_gpio.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma_ex.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr_ex.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_cortex.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_exti.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_ipcc.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rtc.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rtc_ex.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim_ex.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart.c \
Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart_ex.c \
Core/Src/system_stm32wbxx.c \
Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/tl_mbox.c \
Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci/shci.c \
Middlewares/ST/STM32_WPAN/utilities/dbg_trace.c \
Middlewares/ST/STM32_WPAN/utilities/otp.c \
Middlewares/ST/STM32_WPAN/utilities/stm_list.c \
Middlewares/ST/STM32_WPAN/utilities/stm_queue.c \
Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/tl_zigbee_hci.c \
Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl.c \
Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl_if.c \
Middlewares/ST/STM32_WPAN/zigbee/platform/ee.c \
Middlewares/ST/STM32_WPAN/zigbee/platform/hw_flash.c \
Middlewares/ST/STM32_WPAN/zigbee/core/src/zigbee_core_wb.c \
Middlewares/ST/threadx/common/src/tx_initialize_high_level.c \
Middlewares/ST/threadx/common/src/tx_initialize_kernel_enter.c \
Middlewares/ST/threadx/common/src/tx_initialize_kernel_setup.c \
Middlewares/ST/threadx/common/src/tx_block_allocate.c \
Middlewares/ST/threadx/common/src/tx_block_pool_cleanup.c \
Middlewares/ST/threadx/common/src/tx_block_pool_create.c \
Middlewares/ST/threadx/common/src/tx_block_pool_delete.c \
Middlewares/ST/threadx/common/src/tx_block_pool_info_get.c \
Middlewares/ST/threadx/common/src/tx_block_pool_initialize.c \
Middlewares/ST/threadx/common/src/tx_block_pool_prioritize.c \
Middlewares/ST/threadx/common/src/tx_block_release.c \
Middlewares/ST/threadx/common/src/tx_byte_allocate.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_cleanup.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_create.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_delete.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_info_get.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_initialize.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_prioritize.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_search.c \
Middlewares/ST/threadx/common/src/tx_byte_release.c \
Middlewares/ST/threadx/common/src/tx_event_flags_cleanup.c \
Middlewares/ST/threadx/common/src/tx_event_flags_create.c \
Middlewares/ST/threadx/common/src/tx_event_flags_delete.c \
Middlewares/ST/threadx/common/src/tx_event_flags_get.c \
Middlewares/ST/threadx/common/src/tx_event_flags_info_get.c \
Middlewares/ST/threadx/common/src/tx_event_flags_initialize.c \
Middlewares/ST/threadx/common/src/tx_event_flags_set.c \
Middlewares/ST/threadx/common/src/tx_event_flags_set_notify.c \
Middlewares/ST/threadx/common/src/tx_mutex_cleanup.c \
Middlewares/ST/threadx/common/src/tx_mutex_create.c \
Middlewares/ST/threadx/common/src/tx_mutex_delete.c \
Middlewares/ST/threadx/common/src/tx_mutex_get.c \
Middlewares/ST/threadx/common/src/tx_mutex_info_get.c \
Middlewares/ST/threadx/common/src/tx_mutex_initialize.c \
Middlewares/ST/threadx/common/src/tx_mutex_prioritize.c \
Middlewares/ST/threadx/common/src/tx_mutex_priority_change.c \
Middlewares/ST/threadx/common/src/tx_mutex_put.c \
Middlewares/ST/threadx/common/src/tx_queue_cleanup.c \
Middlewares/ST/threadx/common/src/tx_queue_create.c \
Middlewares/ST/threadx/common/src/tx_queue_delete.c \
Middlewares/ST/threadx/common/src/tx_queue_flush.c \
Middlewares/ST/threadx/common/src/tx_queue_front_send.c \
Middlewares/ST/threadx/common/src/tx_queue_info_get.c \
Middlewares/ST/threadx/common/src/tx_queue_initialize.c \
Middlewares/ST/threadx/common/src/tx_queue_prioritize.c \
Middlewares/ST/threadx/common/src/tx_queue_receive.c \
Middlewares/ST/threadx/common/src/tx_queue_send.c \
Middlewares/ST/threadx/common/src/tx_queue_send_notify.c \
Middlewares/ST/threadx/common/src/tx_semaphore_ceiling_put.c \
Middlewares/ST/threadx/common/src/tx_semaphore_cleanup.c \
Middlewares/ST/threadx/common/src/tx_semaphore_create.c \
Middlewares/ST/threadx/common/src/tx_semaphore_delete.c \
Middlewares/ST/threadx/common/src/tx_semaphore_get.c \
Middlewares/ST/threadx/common/src/tx_semaphore_info_get.c \
Middlewares/ST/threadx/common/src/tx_semaphore_initialize.c \
Middlewares/ST/threadx/common/src/tx_semaphore_prioritize.c \
Middlewares/ST/threadx/common/src/tx_semaphore_put.c \
Middlewares/ST/threadx/common/src/tx_semaphore_put_notify.c \
Middlewares/ST/threadx/common/src/tx_thread_create.c \
Middlewares/ST/threadx/common/src/tx_thread_delete.c \
Middlewares/ST/threadx/common/src/tx_thread_entry_exit_notify.c \
Middlewares/ST/threadx/common/src/tx_thread_identify.c \
Middlewares/ST/threadx/common/src/tx_thread_info_get.c \
Middlewares/ST/threadx/common/src/tx_thread_initialize.c \
Middlewares/ST/threadx/common/src/tx_thread_preemption_change.c \
Middlewares/ST/threadx/common/src/tx_thread_priority_change.c \
Middlewares/ST/threadx/common/src/tx_thread_relinquish.c \
Middlewares/ST/threadx/common/src/tx_thread_reset.c \
Middlewares/ST/threadx/common/src/tx_thread_resume.c \
Middlewares/ST/threadx/common/src/tx_thread_shell_entry.c \
Middlewares/ST/threadx/common/src/tx_thread_sleep.c \
Middlewares/ST/threadx/common/src/tx_thread_stack_analyze.c \
Middlewares/ST/threadx/common/src/tx_thread_stack_error_handler.c \
Middlewares/ST/threadx/common/src/tx_thread_stack_error_notify.c \
Middlewares/ST/threadx/common/src/tx_thread_suspend.c \
Middlewares/ST/threadx/common/src/tx_thread_system_preempt_check.c \
Middlewares/ST/threadx/common/src/tx_thread_system_resume.c \
Middlewares/ST/threadx/common/src/tx_thread_system_suspend.c \
Middlewares/ST/threadx/common/src/tx_thread_terminate.c \
Middlewares/ST/threadx/common/src/tx_thread_time_slice.c \
Middlewares/ST/threadx/common/src/tx_thread_time_slice_change.c \
Middlewares/ST/threadx/common/src/tx_thread_timeout.c \
Middlewares/ST/threadx/common/src/tx_thread_wait_abort.c \
Middlewares/ST/threadx/common/src/tx_time_get.c \
Middlewares/ST/threadx/common/src/tx_time_set.c \
Middlewares/ST/threadx/common/src/txe_block_allocate.c \
Middlewares/ST/threadx/common/src/txe_block_pool_create.c \
Middlewares/ST/threadx/common/src/txe_block_pool_delete.c \
Middlewares/ST/threadx/common/src/txe_block_pool_info_get.c \
Middlewares/ST/threadx/common/src/txe_block_pool_prioritize.c \
Middlewares/ST/threadx/common/src/txe_block_release.c \
Middlewares/ST/threadx/common/src/txe_byte_allocate.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_create.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_delete.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_info_get.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_prioritize.c \
Middlewares/ST/threadx/common/src/txe_byte_release.c \
Middlewares/ST/threadx/common/src/txe_event_flags_create.c \
Middlewares/ST/threadx/common/src/txe_event_flags_delete.c \
Middlewares/ST/threadx/common/src/txe_event_flags_get.c \
Middlewares/ST/threadx/common/src/txe_event_flags_info_get.c \
Middlewares/ST/threadx/common/src/txe_event_flags_set.c \
Middlewares/ST/threadx/common/src/txe_event_flags_set_notify.c \
Middlewares/ST/threadx/common/src/txe_mutex_create.c \
Middlewares/ST/threadx/common/src/txe_mutex_delete.c \
Middlewares/ST/threadx/common/src/txe_mutex_get.c \
Middlewares/ST/threadx/common/src/txe_mutex_info_get.c \
Middlewares/ST/threadx/common/src/txe_mutex_prioritize.c \
Middlewares/ST/threadx/common/src/txe_mutex_put.c \
Middlewares/ST/threadx/common/src/txe_queue_create.c \
Middlewares/ST/threadx/common/src/txe_queue_delete.c \
Middlewares/ST/threadx/common/src/txe_queue_flush.c \
Middlewares/ST/threadx/common/src/txe_queue_front_send.c \
Middlewares/ST/threadx/common/src/txe_queue_info_get.c \
Middlewares/ST/threadx/common/src/txe_queue_prioritize.c \
Middlewares/ST/threadx/common/src/txe_queue_receive.c \
Middlewares/ST/threadx/common/src/txe_queue_send.c \
Middlewares/ST/threadx/common/src/txe_queue_send_notify.c \
Middlewares/ST/threadx/common/src/txe_semaphore_ceiling_put.c \
Middlewares/ST/threadx/common/src/txe_semaphore_create.c \
Middlewares/ST/threadx/common/src/txe_semaphore_delete.c \
Middlewares/ST/threadx/common/src/txe_semaphore_get.c \
Middlewares/ST/threadx/common/src/txe_semaphore_info_get.c \
Middlewares/ST/threadx/common/src/txe_semaphore_prioritize.c \
Middlewares/ST/threadx/common/src/txe_semaphore_put.c \
Middlewares/ST/threadx/common/src/txe_semaphore_put_notify.c \
Middlewares/ST/threadx/common/src/txe_thread_create.c \
Middlewares/ST/threadx/common/src/txe_thread_delete.c \
Middlewares/ST/threadx/common/src/txe_thread_entry_exit_notify.c \
Middlewares/ST/threadx/common/src/txe_thread_info_get.c \
Middlewares/ST/threadx/common/src/txe_thread_preemption_change.c \
Middlewares/ST/threadx/common/src/txe_thread_priority_change.c \
Middlewares/ST/threadx/common/src/txe_thread_relinquish.c \
Middlewares/ST/threadx/common/src/txe_thread_reset.c \
Middlewares/ST/threadx/common/src/txe_thread_resume.c \
Middlewares/ST/threadx/common/src/txe_thread_suspend.c \
Middlewares/ST/threadx/common/src/txe_thread_terminate.c \
Middlewares/ST/threadx/common/src/txe_thread_time_slice_change.c \
Middlewares/ST/threadx/common/src/txe_thread_wait_abort.c \
Middlewares/ST/threadx/common/src/tx_timer_activate.c \
Middlewares/ST/threadx/common/src/tx_timer_change.c \
Middlewares/ST/threadx/common/src/tx_timer_create.c \
Middlewares/ST/threadx/common/src/tx_timer_deactivate.c \
Middlewares/ST/threadx/common/src/tx_timer_delete.c \
Middlewares/ST/threadx/common/src/tx_timer_expiration_process.c \
Middlewares/ST/threadx/common/src/tx_timer_info_get.c \
Middlewares/ST/threadx/common/src/tx_timer_initialize.c \
Middlewares/ST/threadx/common/src/tx_timer_system_activate.c \
Middlewares/ST/threadx/common/src/tx_timer_system_deactivate.c \
Middlewares/ST/threadx/common/src/tx_timer_thread_entry.c \
Middlewares/ST/threadx/common/src/txe_timer_activate.c \
Middlewares/ST/threadx/common/src/txe_timer_change.c \
Middlewares/ST/threadx/common/src/txe_timer_create.c \
Middlewares/ST/threadx/common/src/txe_timer_deactivate.c \
Middlewares/ST/threadx/common/src/txe_timer_delete.c \
Middlewares/ST/threadx/common/src/txe_timer_info_get.c \
Middlewares/ST/threadx/common/src/tx_trace_buffer_full_notify.c \
Middlewares/ST/threadx/common/src/tx_trace_disable.c \
Middlewares/ST/threadx/common/src/tx_trace_enable.c \
Middlewares/ST/threadx/common/src/tx_trace_event_filter.c \
Middlewares/ST/threadx/common/src/tx_trace_event_unfilter.c \
Middlewares/ST/threadx/common/src/tx_trace_initialize.c \
Middlewares/ST/threadx/common/src/tx_trace_interrupt_control.c \
Middlewares/ST/threadx/common/src/tx_trace_isr_enter_insert.c \
Middlewares/ST/threadx/common/src/tx_trace_isr_exit_insert.c \
Middlewares/ST/threadx/common/src/tx_trace_object_register.c \
Middlewares/ST/threadx/common/src/tx_trace_object_unregister.c \
Middlewares/ST/threadx/common/src/tx_trace_user_event_insert.c \
Utilities/lpm/tiny_lpm/stm32_lpm.c \
Core/Src/sysmem.c \
Core/Src/syscalls.c \
Core/Src/hw_uart.c

# ASM sources
ASM_SOURCES =  \
startup_stm32wb55xx_cm4.s

# ASM sources
ASMM_SOURCES =  \
Core/Src/tx_initialize_low_level.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_context_restore.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_context_save.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_interrupt_control.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_interrupt_disable.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_interrupt_restore.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_schedule.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_stack_build.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_system_return.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_timer_interrupt.S


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DZIGBEE_WB \
-DTX_INCLUDE_USER_DEFINE_FILE \
-DUSE_HAL_DRIVER \
-DSTM32WB55xx


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ICore/Inc \
-IAZURE_RTOS/App \
-ISTM32_WPAN/App \
-IDrivers/STM32WBxx_HAL_Driver/Inc \
-IDrivers/STM32WBxx_HAL_Driver/Inc/Legacy \
-IUtilities/lpm/tiny_lpm \
-IMiddlewares/ST/STM32_WPAN \
-IMiddlewares/ST/STM32_WPAN/interface/patterns/ble_thread \
-IMiddlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl \
-IMiddlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci \
-IMiddlewares/ST/STM32_WPAN/utilities \
-I../Middlewares/ST/STM32_WPAN/zigbee/stack/include/extras \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include/zgp \
-IDrivers/CMSIS/Device/ST/STM32WBxx/Include \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include/mac \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include/zcl \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/key \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/se \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/security \
-IMiddlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/general \
-IMiddlewares/ST/STM32_WPAN/zigbee/platform \
-IMiddlewares/ST/STM32_WPAN/zigbee/core/inc \
-IDrivers/CMSIS/Include \
-IMiddlewares/ST/threadx/common/inc/ \
-IMiddlewares/ST/threadx/ports/cortex_m4/gnu/inc/


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = stm32wb55xx_flash_cm4.ld

# libraries
LIBS = -lc -lm -lnosys  \
-l:stm32wb_zigbee_wb_lib.a
LIBDIR =  \
-LMiddlewares/ST/STM32_WPAN/zigbee/lib
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
