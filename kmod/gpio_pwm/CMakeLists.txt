message("Compiling gpio_pwm kernel module")
execute_process(COMMAND "make -C ${CURRENT_SOURCE_DIR}/")
