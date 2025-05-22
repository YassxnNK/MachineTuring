set(EXECUTABLE_NAME project-elec)

link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(${EXECUTABLE_NAME} 
    led_management.c
    algo_management.c
    main.c
)