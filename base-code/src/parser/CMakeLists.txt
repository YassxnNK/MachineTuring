set(EXECUTABLE_NAME project-elec-parser)

link_directories(/usr/lib/x86_64-linux-gnu)


add_executable(${EXECUTABLE_NAME} 
    parser_json.c
)