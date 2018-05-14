#add_application(helloworld helloworld.c TB run_mem_dpi.tcl)
#add_application(helloworld helloworld.c TB run_spi_standalone.tcl)
#add_application(helloworld helloworld.c TB run_spi.tcl)
add_application(helloworld helloworld.c TB run_standalone.tcl)
#add_application(helloworld math_test.c) 
#add_application(helloworld boot_code_sram.c TB run_spi_standalone.tcl)

add_application(helloworldcpp helloworld.cpp SUBDIR "../helloworldcpp" LABELS "hello")
