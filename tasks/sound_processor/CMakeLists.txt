add_executable(sound_processor
    main.cpp
    application.cpp
    args_parser.cpp
    converter.cpp
    filters.cpp
    pipeline.cpp
    waveform.cpp
    wav_io.cpp
)

add_executable(test_sound_processor
    test.cpp
    application.cpp
    args_parser.cpp
    converter.cpp
    filters.cpp
    pipeline.cpp
    waveform.cpp
    wav_io.cpp
)

target_link_libraries(test_sound_processor PRIVATE XiCatch2WithMain)
add_dependencies(test_sound_processor sound_processor)
