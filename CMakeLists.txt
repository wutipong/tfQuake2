cmake_minimum_required(VERSION 3.17)

project(tf-quake2)

add_subdirectory(tf)
add_subdirectory(game)
add_subdirectory(ctf)
add_subdirectory(rogue)
add_subdirectory(smd)
add_subdirectory(xatrix)
add_subdirectory(zaero)

set_target_properties(
    baseq2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:tf-quake2-main>/baseq2")

set_target_properties(
    ctf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:tf-quake2-main>/ctf")

set_target_properties(
    rogue PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:tf-quake2-main>/rogue")

set_target_properties(
    smd PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:tf-quake2-main>/smd")

set_target_properties(
    xatrix PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:tf-quake2-main>/xatrix")

set_target_properties(
    zaero PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:tf-quake2-main>/zaero")
