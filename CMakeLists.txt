cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME WB55_Zigbee_Srv)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# HAL library sources
set(HAL_SRC
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_hsem.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_rcc.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rcc_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_flash_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_gpio.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_dma_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pwr_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_cortex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_exti.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_ipcc.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rtc.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_rtc_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_tim_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_uart_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pcd.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_hal_pcd_ex.c
	Drivers/STM32WBxx_HAL_Driver/Src/stm32wbxx_ll_usb.c
)

set_source_files_properties(${HAL_SRC} PROPERTIES COMPILE_FLAGS "-Wno-pedantic -Wno-unused-parameter")

# ThreadX library sources
set(THREADX_SRC
	Middlewares/ST/threadx/common/src/tx_initialize_high_level.c
	Middlewares/ST/threadx/common/src/tx_initialize_kernel_enter.c
	Middlewares/ST/threadx/common/src/tx_initialize_kernel_setup.c
	Middlewares/ST/threadx/common/src/tx_block_allocate.c
	Middlewares/ST/threadx/common/src/tx_block_pool_cleanup.c
	Middlewares/ST/threadx/common/src/tx_block_pool_create.c
	Middlewares/ST/threadx/common/src/tx_block_pool_delete.c
	Middlewares/ST/threadx/common/src/tx_block_pool_info_get.c
	Middlewares/ST/threadx/common/src/tx_block_pool_initialize.c
	Middlewares/ST/threadx/common/src/tx_block_pool_prioritize.c
	Middlewares/ST/threadx/common/src/tx_block_release.c
	Middlewares/ST/threadx/common/src/tx_byte_allocate.c
	Middlewares/ST/threadx/common/src/tx_byte_pool_cleanup.c
	Middlewares/ST/threadx/common/src/tx_byte_pool_create.c
	Middlewares/ST/threadx/common/src/tx_byte_pool_delete.c
	Middlewares/ST/threadx/common/src/tx_byte_pool_info_get.c
	Middlewares/ST/threadx/common/src/tx_byte_pool_initialize.c
	Middlewares/ST/threadx/common/src/tx_byte_pool_prioritize.c
	Middlewares/ST/threadx/common/src/tx_byte_pool_search.c
	Middlewares/ST/threadx/common/src/tx_byte_release.c
	Middlewares/ST/threadx/common/src/tx_event_flags_cleanup.c
	Middlewares/ST/threadx/common/src/tx_event_flags_create.c
	Middlewares/ST/threadx/common/src/tx_event_flags_delete.c
	Middlewares/ST/threadx/common/src/tx_event_flags_get.c
	Middlewares/ST/threadx/common/src/tx_event_flags_info_get.c
	Middlewares/ST/threadx/common/src/tx_event_flags_initialize.c
	Middlewares/ST/threadx/common/src/tx_event_flags_set.c
	Middlewares/ST/threadx/common/src/tx_event_flags_set_notify.c
	Middlewares/ST/threadx/common/src/tx_mutex_cleanup.c
	Middlewares/ST/threadx/common/src/tx_mutex_create.c
	Middlewares/ST/threadx/common/src/tx_mutex_delete.c
	Middlewares/ST/threadx/common/src/tx_mutex_get.c
	Middlewares/ST/threadx/common/src/tx_mutex_info_get.c
	Middlewares/ST/threadx/common/src/tx_mutex_initialize.c
	Middlewares/ST/threadx/common/src/tx_mutex_prioritize.c
	Middlewares/ST/threadx/common/src/tx_mutex_priority_change.c
	Middlewares/ST/threadx/common/src/tx_mutex_put.c
	Middlewares/ST/threadx/common/src/tx_queue_cleanup.c
	Middlewares/ST/threadx/common/src/tx_queue_create.c
	Middlewares/ST/threadx/common/src/tx_queue_delete.c
	Middlewares/ST/threadx/common/src/tx_queue_flush.c
	Middlewares/ST/threadx/common/src/tx_queue_front_send.c
	Middlewares/ST/threadx/common/src/tx_queue_info_get.c
	Middlewares/ST/threadx/common/src/tx_queue_initialize.c
	Middlewares/ST/threadx/common/src/tx_queue_prioritize.c
	Middlewares/ST/threadx/common/src/tx_queue_receive.c
	Middlewares/ST/threadx/common/src/tx_queue_send.c
	Middlewares/ST/threadx/common/src/tx_queue_send_notify.c
	Middlewares/ST/threadx/common/src/tx_semaphore_ceiling_put.c
	Middlewares/ST/threadx/common/src/tx_semaphore_cleanup.c
	Middlewares/ST/threadx/common/src/tx_semaphore_create.c
	Middlewares/ST/threadx/common/src/tx_semaphore_delete.c
	Middlewares/ST/threadx/common/src/tx_semaphore_get.c
	Middlewares/ST/threadx/common/src/tx_semaphore_info_get.c
	Middlewares/ST/threadx/common/src/tx_semaphore_initialize.c
	Middlewares/ST/threadx/common/src/tx_semaphore_prioritize.c
	Middlewares/ST/threadx/common/src/tx_semaphore_put.c
	Middlewares/ST/threadx/common/src/tx_semaphore_put_notify.c
	Middlewares/ST/threadx/common/src/tx_thread_create.c
	Middlewares/ST/threadx/common/src/tx_thread_delete.c
	Middlewares/ST/threadx/common/src/tx_thread_entry_exit_notify.c
	Middlewares/ST/threadx/common/src/tx_thread_identify.c
	Middlewares/ST/threadx/common/src/tx_thread_info_get.c
	Middlewares/ST/threadx/common/src/tx_thread_initialize.c
	Middlewares/ST/threadx/common/src/tx_thread_preemption_change.c
	Middlewares/ST/threadx/common/src/tx_thread_priority_change.c
	Middlewares/ST/threadx/common/src/tx_thread_relinquish.c
	Middlewares/ST/threadx/common/src/tx_thread_reset.c
	Middlewares/ST/threadx/common/src/tx_thread_resume.c
	Middlewares/ST/threadx/common/src/tx_thread_shell_entry.c
	Middlewares/ST/threadx/common/src/tx_thread_sleep.c
	Middlewares/ST/threadx/common/src/tx_thread_stack_analyze.c
	Middlewares/ST/threadx/common/src/tx_thread_stack_error_handler.c
	Middlewares/ST/threadx/common/src/tx_thread_stack_error_notify.c
	Middlewares/ST/threadx/common/src/tx_thread_suspend.c
	Middlewares/ST/threadx/common/src/tx_thread_system_preempt_check.c
	Middlewares/ST/threadx/common/src/tx_thread_system_resume.c
	Middlewares/ST/threadx/common/src/tx_thread_system_suspend.c
	Middlewares/ST/threadx/common/src/tx_thread_terminate.c
	Middlewares/ST/threadx/common/src/tx_thread_time_slice.c
	Middlewares/ST/threadx/common/src/tx_thread_time_slice_change.c
	Middlewares/ST/threadx/common/src/tx_thread_timeout.c
	Middlewares/ST/threadx/common/src/tx_thread_wait_abort.c
	Middlewares/ST/threadx/common/src/tx_time_get.c
	Middlewares/ST/threadx/common/src/tx_time_set.c
	Middlewares/ST/threadx/common/src/txe_block_allocate.c
	Middlewares/ST/threadx/common/src/txe_block_pool_create.c
	Middlewares/ST/threadx/common/src/txe_block_pool_delete.c
	Middlewares/ST/threadx/common/src/txe_block_pool_info_get.c
	Middlewares/ST/threadx/common/src/txe_block_pool_prioritize.c
	Middlewares/ST/threadx/common/src/txe_block_release.c
	Middlewares/ST/threadx/common/src/txe_byte_allocate.c
	Middlewares/ST/threadx/common/src/txe_byte_pool_create.c
	Middlewares/ST/threadx/common/src/txe_byte_pool_delete.c
	Middlewares/ST/threadx/common/src/txe_byte_pool_info_get.c
	Middlewares/ST/threadx/common/src/txe_byte_pool_prioritize.c
	Middlewares/ST/threadx/common/src/txe_byte_release.c
	Middlewares/ST/threadx/common/src/txe_event_flags_create.c
	Middlewares/ST/threadx/common/src/txe_event_flags_delete.c
	Middlewares/ST/threadx/common/src/txe_event_flags_get.c
	Middlewares/ST/threadx/common/src/txe_event_flags_info_get.c
	Middlewares/ST/threadx/common/src/txe_event_flags_set.c
	Middlewares/ST/threadx/common/src/txe_event_flags_set_notify.c
	Middlewares/ST/threadx/common/src/txe_mutex_create.c
	Middlewares/ST/threadx/common/src/txe_mutex_delete.c
	Middlewares/ST/threadx/common/src/txe_mutex_get.c
	Middlewares/ST/threadx/common/src/txe_mutex_info_get.c
	Middlewares/ST/threadx/common/src/txe_mutex_prioritize.c
	Middlewares/ST/threadx/common/src/txe_mutex_put.c
	Middlewares/ST/threadx/common/src/txe_queue_create.c
	Middlewares/ST/threadx/common/src/txe_queue_delete.c
	Middlewares/ST/threadx/common/src/txe_queue_flush.c
	Middlewares/ST/threadx/common/src/txe_queue_front_send.c
	Middlewares/ST/threadx/common/src/txe_queue_info_get.c
	Middlewares/ST/threadx/common/src/txe_queue_prioritize.c
	Middlewares/ST/threadx/common/src/txe_queue_receive.c
	Middlewares/ST/threadx/common/src/txe_queue_send.c
	Middlewares/ST/threadx/common/src/txe_queue_send_notify.c
	Middlewares/ST/threadx/common/src/txe_semaphore_ceiling_put.c
	Middlewares/ST/threadx/common/src/txe_semaphore_create.c
	Middlewares/ST/threadx/common/src/txe_semaphore_delete.c
	Middlewares/ST/threadx/common/src/txe_semaphore_get.c
	Middlewares/ST/threadx/common/src/txe_semaphore_info_get.c
	Middlewares/ST/threadx/common/src/txe_semaphore_prioritize.c
	Middlewares/ST/threadx/common/src/txe_semaphore_put.c
	Middlewares/ST/threadx/common/src/txe_semaphore_put_notify.c
	Middlewares/ST/threadx/common/src/txe_thread_create.c
	Middlewares/ST/threadx/common/src/txe_thread_delete.c
	Middlewares/ST/threadx/common/src/txe_thread_entry_exit_notify.c
	Middlewares/ST/threadx/common/src/txe_thread_info_get.c
	Middlewares/ST/threadx/common/src/txe_thread_preemption_change.c
	Middlewares/ST/threadx/common/src/txe_thread_priority_change.c
	Middlewares/ST/threadx/common/src/txe_thread_relinquish.c
	Middlewares/ST/threadx/common/src/txe_thread_reset.c
	Middlewares/ST/threadx/common/src/txe_thread_resume.c
	Middlewares/ST/threadx/common/src/txe_thread_suspend.c
	Middlewares/ST/threadx/common/src/txe_thread_terminate.c
	Middlewares/ST/threadx/common/src/txe_thread_time_slice_change.c
	Middlewares/ST/threadx/common/src/txe_thread_wait_abort.c
	Middlewares/ST/threadx/common/src/tx_timer_activate.c
	Middlewares/ST/threadx/common/src/tx_timer_change.c
	Middlewares/ST/threadx/common/src/tx_timer_create.c
	Middlewares/ST/threadx/common/src/tx_timer_deactivate.c
	Middlewares/ST/threadx/common/src/tx_timer_delete.c
	Middlewares/ST/threadx/common/src/tx_timer_expiration_process.c
	Middlewares/ST/threadx/common/src/tx_timer_info_get.c
	Middlewares/ST/threadx/common/src/tx_timer_initialize.c
	Middlewares/ST/threadx/common/src/tx_timer_system_activate.c
	Middlewares/ST/threadx/common/src/tx_timer_system_deactivate.c
	Middlewares/ST/threadx/common/src/tx_timer_thread_entry.c
	Middlewares/ST/threadx/common/src/txe_timer_activate.c
	Middlewares/ST/threadx/common/src/txe_timer_change.c
	Middlewares/ST/threadx/common/src/txe_timer_create.c
	Middlewares/ST/threadx/common/src/txe_timer_deactivate.c
	Middlewares/ST/threadx/common/src/txe_timer_delete.c
	Middlewares/ST/threadx/common/src/txe_timer_info_get.c
	Middlewares/ST/threadx/common/src/tx_trace_buffer_full_notify.c
	Middlewares/ST/threadx/common/src/tx_trace_disable.c
	Middlewares/ST/threadx/common/src/tx_trace_enable.c
	Middlewares/ST/threadx/common/src/tx_trace_event_filter.c
	Middlewares/ST/threadx/common/src/tx_trace_event_unfilter.c
	Middlewares/ST/threadx/common/src/tx_trace_initialize.c
	Middlewares/ST/threadx/common/src/tx_trace_interrupt_control.c
	Middlewares/ST/threadx/common/src/tx_trace_isr_enter_insert.c
	Middlewares/ST/threadx/common/src/tx_trace_isr_exit_insert.c
	Middlewares/ST/threadx/common/src/tx_trace_object_register.c
	Middlewares/ST/threadx/common/src/tx_trace_object_unregister.c
	Middlewares/ST/threadx/common/src/tx_trace_user_event_insert.c
)

#set_source_files_properties(${THREADX_SRC} PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

# WPAN library sources
set(WPAN_SRC
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/tl_mbox.c
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci/shci.c
	Middlewares/ST/STM32_WPAN/utilities/dbg_trace.c
	Middlewares/ST/STM32_WPAN/utilities/otp.c
	Middlewares/ST/STM32_WPAN/utilities/stm_list.c
	Middlewares/ST/STM32_WPAN/utilities/stm_queue.c
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/tl_zigbee_hci.c
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl.c
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl/shci_tl_if.c
	Middlewares/ST/STM32_WPAN/zigbee/platform/ee.c
	Middlewares/ST/STM32_WPAN/zigbee/platform/hw_flash.c
	Middlewares/ST/STM32_WPAN/zigbee/core/src/zigbee_core_wb.c
	STM32_WPAN/App/app_zigbee.c
	STM32_WPAN/Target/hw_ipcc.c	
)

set_source_files_properties(${WPAN_SRC} PROPERTIES COMPILE_FLAGS "-Wno-pedantic -Wno-unused-parameter -Wno-sign-compare")

# ASM sources
set(ASM_SRC
	startup_stm32wb55xx_cm4.s
)

# ASMM sources
set(ASMM_SRC
	Core/Src/tx_initialize_low_level.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_context_restore.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_context_save.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_interrupt_control.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_interrupt_disable.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_interrupt_restore.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_schedule.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_stack_build.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_system_return.S
	Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_timer_interrupt.S
)	

set_source_files_properties(Core/Src/hw_timerserver.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties(Core/Src/app_entry.c PROPERTIES COMPILE_FLAGS "-Wno-pedantic -Wno-unused-parameter -Wno-unused-function")
set_source_files_properties(Core/Src/stm_logging.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
#set_source_files_properties(Core/Src/app_threadx.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
#set_source_files_properties(AZURE_RTOS/App/app_azure_rtos.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

# project source files
file(GLOB PROJECT_SOURCE_FILES CONFIGURE_DEPENDS Core/Src/*.c)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	${PROJECT_SOURCE_FILES}
	${ASM_SRC}
	${ASMM_SRC}
	${HAL_SRC}
	${THREADX_SRC}
	${WPAN_SRC}
	AZURE_RTOS/App/app_azure_rtos.c
	Utilities/lpm/tiny_lpm/stm32_lpm.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
	NetXDuo/App
	Core/Inc
	AZURE_RTOS/App
	STM32_WPAN/App
	Drivers/STM32WBxx_HAL_Driver/Inc
	Drivers/STM32WBxx_HAL_Driver/Inc/Legacy
	Utilities/lpm/tiny_lpm
	Middlewares/ST/STM32_WPAN
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl
	Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci
	Middlewares/ST/STM32_WPAN/utilities
	../Middlewares/ST/STM32_WPAN/zigbee/stack/include/extras
	Middlewares/ST/STM32_WPAN/zigbee/stack/include/zgp
	Drivers/CMSIS/Device/ST/STM32WBxx/Include
	Middlewares/ST/STM32_WPAN/zigbee/stack/include
	Middlewares/ST/STM32_WPAN/zigbee/stack/include/mac
	Middlewares/ST/STM32_WPAN/zigbee/stack/include/zcl
	Middlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/key
	Middlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/se
	Middlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/security
	Middlewares/ST/STM32_WPAN/zigbee/stack/include/zcl/general
	Middlewares/ST/STM32_WPAN/zigbee/platform
	Middlewares/ST/STM32_WPAN/zigbee/core/inc
	Drivers/CMSIS/Include
	Middlewares/ST/threadx/common/inc/
	Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/
	.
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
	ZIGBEE_WB
	TX_INCLUDE_USER_DEFINE_FILE
	USE_HAL_DRIVER
	STM32WB55xx
	TX_SINGLE_MODE_NON_SECURE=1
	USE_NUCLEO_64
	STM32_THREAD_SAFE_STRATEGY=2	
)

# Disable linker segmentation warning
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "LINKER:--no-warn-rwx-segments")

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

