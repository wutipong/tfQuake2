add_library(gainput 
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/gainput.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/GainputAllocator.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/GainputInputDeltaState.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/GainputInputDevice.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/GainputInputManager.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/GainputInputMap.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/GainputInputState.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/GainputMapFilters.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/GainputHID.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/GainputHIDWhitelist.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserPS4Controller.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserPS4Controller.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserPS4Controller.h"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserPS5Controller.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserPS5Controller.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserPS5Controller.h"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserSwitchController.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/HIDParserSwitchController.h"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/keyboard/GainputInputDeviceKeyboard.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/mouse/GainputInputDeviceMouse.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/pad/GainputInputDevicePad.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/touch/GainputInputDeviceTouch.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/hidapi/windows/hid.c"
)

if (MSVC)
    target_compile_definitions (gainput PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif ()

target_include_directories(gainput PUBLIC
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source"
)

add_library(the-forge
    "The-Forge/Common_3/Application/CameraController.cpp"
    "The-Forge/Common_3/Application/Fonts/FontSystem.cpp"
    "The-Forge/Common_3/Application/Fonts/stbtt.cpp"
    "The-Forge/Common_3/Application/InputSystem.cpp"
    "The-Forge/Common_3/Application/Profiler/GpuProfiler.cpp"
    "The-Forge/Common_3/Application/Profiler/ProfilerBase.cpp"
    "The-Forge/Common_3/Application/RemoteControl.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/imgui/imgui_demo.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/imgui/imgui_draw.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/imgui/imgui_tables.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/imgui/imgui_widgets.cpp"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/imgui/imgui.cpp"
    "The-Forge/Common_3/Application/UI/UI.cpp"
    "The-Forge/Common_3/Game/Scripting/LuaManager.cpp"
    "The-Forge/Common_3/Game/Scripting/LuaManagerImpl.cpp"
    "The-Forge/Common_3/Game/Scripting/LuaSystem.cpp"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lapi.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lauxlib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lbaselib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lbitlib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lcode.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lcorolib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lctype.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/ldblib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/ldebug.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/ldo.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/ldump.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lfunc.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lgc.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/linit.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/liolib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/llex.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lmathlib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lmem.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/loadlib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lobject.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lopcodes.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/loslib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lparser.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lstate.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lstring.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lstrlib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/ltable.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/ltablib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/ltm.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lundump.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lutf8lib.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lvm.c"
    "The-Forge/Common_3/Game/ThirdParty/OpenSource/lua-5.3.5/src/lzio.c"
    "The-Forge/Common_3/Graphics/CommonShaderReflection.cpp"
    "The-Forge/Common_3/Graphics/Direct3D11/Direct3D11.cpp"
    "The-Forge/Common_3/Graphics/Direct3D11/Direct3D11ShaderReflection.cpp"
    "The-Forge/Common_3/Graphics/Direct3D12/Direct3D12.cpp"
    "The-Forge/Common_3/Graphics/Direct3D12/Direct3D12Hooks.cpp"
    "The-Forge/Common_3/Graphics/Direct3D12/Direct3D12Raytracing.cpp"
    "The-Forge/Common_3/Graphics/Direct3D12/Direct3D12ShaderReflection.cpp"
    "The-Forge/Common_3/Graphics/GPUConfig.cpp"
    "The-Forge/Common_3/Graphics/PickRenderingAPI.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_cfg.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_cpp.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_cross_parsed_ir.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_cross_util.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_cross.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_glsl.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_hlsl.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_msl.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_parser.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/spirv_reflect.cpp"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/SpirvTools.cpp"
    "The-Forge/Common_3/Graphics/Vulkan/Vulkan.cpp"
    "The-Forge/Common_3/Graphics/Vulkan/VulkanRaytracing.cpp"
    "The-Forge/Common_3/Graphics/Vulkan/VulkanShaderReflection.cpp"
    "The-Forge/Common_3/OS/CPUConfig.cpp"
    "The-Forge/Common_3/OS/ThirdParty/OpenSource/cpu_features/src/impl_x86_windows.c"
    "The-Forge/Common_3/OS/Windows/WindowsBase.cpp"
    "The-Forge/Common_3/OS/Windows/WindowsFileSystem.cpp"
    "The-Forge/Common_3/OS/Windows/WindowsLog.c"
    "The-Forge/Common_3/OS/Windows/WindowsStackTraceDump.cpp"
    "The-Forge/Common_3/OS/Windows/WindowsThread.c"
    "The-Forge/Common_3/OS/Windows/WindowsTime.c"
    "The-Forge/Common_3/OS/Windows/WindowsToolsFileSystem.cpp"
    "The-Forge/Common_3/OS/Windows/WindowsWindow.cpp"
    "The-Forge/Common_3/OS/WindowSystem/WindowSystem.cpp"
    "The-Forge/Common_3/Resources/ResourceLoader/ResourceLoader.cpp"
    "The-Forge/Common_3/Tools/Network/Network.c"
    "The-Forge/Common_3/Tools/ReloadServer/ReloadClient.cpp"
    "The-Forge/Common_3/Utilities/FileSystem/FileSystem.c"
    "The-Forge/Common_3/Utilities/FileSystem/SystemRun.c"
    "The-Forge/Common_3/Utilities/Log/Log.c"
    "The-Forge/Common_3/Utilities/Math/Algorithms.c"
    "The-Forge/Common_3/Utilities/Math/StbDs.c"
    "The-Forge/Common_3/Utilities/MemoryTracking/MemoryTracking.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/basis_universal/transcoder/basisu_transcoder.cpp"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/bstrlib/bstrlib.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/lz4/lz4.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/lz4/lz4.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/lz4/lz4hc.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/lz4/lz4hc.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/bits.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/bitstream.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/compiler.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/cpu.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/debug.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/debug.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/entropy_common.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/error_private.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/error_private.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/fse_decompress.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/fse.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/huf.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/mem.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/pool.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/pool.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/portability_macros.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/threading.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/threading.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/xxhash.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/xxhash.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/zstd_common.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/zstd_deps.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/zstd_internal.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/common/zstd_trace.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/huf_decompress.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/zstd_ddict.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/zstd_ddict.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/zstd_decompress_block.c"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/zstd_decompress_block.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/zstd_decompress_internal.h"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/zstd_decompress.c"
    "The-Forge/Common_3/Utilities/Timer.c"
)

if (MSVC)
    target_compile_definitions (the-forge PRIVATE -D_UNICODE -DUNICODE)
    target_compile_definitions (the-forge PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif ()

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_library(soloud 
    "soloud/src/audiosource/wav/dr_flac.h"
    "soloud/src/audiosource/wav/dr_impl.cpp"
    "soloud/src/audiosource/wav/dr_mp3.h"
    "soloud/src/audiosource/wav/dr_wav.h"
    "soloud/src/audiosource/wav/soloud_wav.cpp"
    "soloud/src/audiosource/wav/soloud_wavstream.cpp"
    "soloud/src/audiosource/wav/stb_vorbis.c"
    "soloud/src/audiosource/wav/stb_vorbis.h"
    "soloud/src/backend/alsa/soloud_alsa.cpp"
    "soloud/src/backend/coreaudio/soloud_coreaudio.cpp"
    "soloud/src/backend/jack/soloud_jack.cpp"
    "soloud/src/backend/miniaudio/miniaudio.h"
    "soloud/src/backend/miniaudio/soloud_miniaudio.cpp"
    "soloud/src/backend/nosound/soloud_nosound.cpp"
    "soloud/src/backend/null/soloud_null.cpp"
    "soloud/src/backend/openal/soloud_openal_dll.c"
    "soloud/src/backend/openal/soloud_openal.cpp"
    "soloud/src/backend/opensles/soloud_opensles.cpp"
    "soloud/src/backend/oss/soloud_oss.cpp"
    "soloud/src/backend/portaudio/soloud_portaudio_dll.c"
    "soloud/src/backend/portaudio/soloud_portaudio.cpp"
    "soloud/src/backend/wasapi/soloud_wasapi.cpp"
    "soloud/src/backend/winmm/soloud_winmm.cpp"
    "soloud/src/backend/xaudio2/soloud_xaudio2.cpp"
    "soloud/src/core/soloud_audiosource.cpp"
    "soloud/src/core/soloud_bus.cpp"
    "soloud/src/core/soloud_core_3d.cpp"
    "soloud/src/core/soloud_core_basicops.cpp"
    "soloud/src/core/soloud_core_faderops.cpp"
    "soloud/src/core/soloud_core_filterops.cpp"
    "soloud/src/core/soloud_core_getters.cpp"
    "soloud/src/core/soloud_core_setters.cpp"
    "soloud/src/core/soloud_core_voicegroup.cpp"
    "soloud/src/core/soloud_core_voiceops.cpp"
    "soloud/src/core/soloud_fader.cpp"
    "soloud/src/core/soloud_fft_lut.cpp"
    "soloud/src/core/soloud_fft.cpp"
    "soloud/src/core/soloud_file.cpp"
    "soloud/src/core/soloud_filter.cpp"
    "soloud/src/core/soloud_misc.cpp"
    "soloud/src/core/soloud_queue.cpp"
    "soloud/src/core/soloud_thread.cpp"
    "soloud/src/core/soloud.cpp"
)

target_include_directories(soloud PRIVATE
    "soloud/include"
)

if(WIN32)
    target_compile_definitions(soloud PRIVATE -DWITH_WINMM)
endif()

add_executable(tf-quake2-main
    "src/sys_win32.cpp"
    "src/sys_main.cpp"
    "src/sys_event.cpp"
    "src/sys_event.h"
    "src/gra_common.cpp"
    "src/gra_common.h"
    "src/vid_common.cpp"
    "src/in_common.cpp"
    "src/sys_export.cpp"
    "src/cd_soloud.cpp"

    "${CMAKE_SOURCE_DIR}/client/cl_cin.c"
    "${CMAKE_SOURCE_DIR}/client/cl_ents.c"
    "${CMAKE_SOURCE_DIR}/client/cl_fx.c"
    "${CMAKE_SOURCE_DIR}/client/cl_input.c"
    "${CMAKE_SOURCE_DIR}/client/cl_inv.c"
    "${CMAKE_SOURCE_DIR}/client/cl_main.c"
    "${CMAKE_SOURCE_DIR}/client/cl_newfx.c"
    "${CMAKE_SOURCE_DIR}/client/cl_parse.c"
    "${CMAKE_SOURCE_DIR}/client/cl_pred.c"
    "${CMAKE_SOURCE_DIR}/client/cl_scrn.c"
    "${CMAKE_SOURCE_DIR}/client/cl_tent.c"
    "${CMAKE_SOURCE_DIR}/client/cl_view.c"
    "${CMAKE_SOURCE_DIR}/client/console.c"
    "${CMAKE_SOURCE_DIR}/client/keys.c"
    "${CMAKE_SOURCE_DIR}/client/menu.c"
    "${CMAKE_SOURCE_DIR}/client/qmenu.c"
    "${CMAKE_SOURCE_DIR}/client/snd_dma.c"
    "${CMAKE_SOURCE_DIR}/client/snd_mem.c"
    "${CMAKE_SOURCE_DIR}/client/snd_miniaudio.c"
    "${CMAKE_SOURCE_DIR}/client/snd_mix.c"
    "${CMAKE_SOURCE_DIR}/game/m_flash.c"
    "${CMAKE_SOURCE_DIR}/game/q_shared.c"
    "${CMAKE_SOURCE_DIR}/qcommon/cmd.c"
    "${CMAKE_SOURCE_DIR}/qcommon/cmodel.c"
    "${CMAKE_SOURCE_DIR}/qcommon/common.c"
    "${CMAKE_SOURCE_DIR}/qcommon/crc.c"
    "${CMAKE_SOURCE_DIR}/qcommon/cvar.c"
    "${CMAKE_SOURCE_DIR}/qcommon/files.c"
    "${CMAKE_SOURCE_DIR}/qcommon/md4.c"
    "${CMAKE_SOURCE_DIR}/qcommon/net_chan.c"
    "${CMAKE_SOURCE_DIR}/qcommon/pmove.c"
    "${CMAKE_SOURCE_DIR}/server/sv_ccmds.c"
    "${CMAKE_SOURCE_DIR}/server/sv_ents.c"
    "${CMAKE_SOURCE_DIR}/server/sv_game.c"
    "${CMAKE_SOURCE_DIR}/server/sv_init.c"
    "${CMAKE_SOURCE_DIR}/server/sv_main.c"
    "${CMAKE_SOURCE_DIR}/server/sv_send.c"
    "${CMAKE_SOURCE_DIR}/server/sv_user.c"
    "${CMAKE_SOURCE_DIR}/server/sv_world.c"
    "${CMAKE_SOURCE_DIR}/null/snddma_null.c"
    "${CMAKE_SOURCE_DIR}/win32/conproc.c"
    # "${CMAKE_SOURCE_DIR}/win32/in_win.c"
    "${CMAKE_SOURCE_DIR}/win32/net_wins.c"
    "${CMAKE_SOURCE_DIR}/win32/q_shwin.c"

    # "${CMAKE_SOURCE_DIR}/win32/snd_win.c"
    # "${CMAKE_SOURCE_DIR}/win32/sys_win.c"
    # "${CMAKE_SOURCE_DIR}/win32/vid_dll.c"
    "${CMAKE_SOURCE_DIR}/win32/vid_menu.c"
    "${CMAKE_SOURCE_DIR}/client/anorms.h"
    "${CMAKE_SOURCE_DIR}/client/cdaudio.h"
    "${CMAKE_SOURCE_DIR}/client/client.h"
    "${CMAKE_SOURCE_DIR}/client/console.h"
    "${CMAKE_SOURCE_DIR}/client/input.h"
    "${CMAKE_SOURCE_DIR}/client/keys.h"
    "${CMAKE_SOURCE_DIR}/client/miniaudio.h"
    "${CMAKE_SOURCE_DIR}/client/qmenu.h"
    "${CMAKE_SOURCE_DIR}/client/ref.h"
    "${CMAKE_SOURCE_DIR}/client/screen.h"
    "${CMAKE_SOURCE_DIR}/client/snd_loc.h"
    "${CMAKE_SOURCE_DIR}/client/snd_miniaudio.h"
    "${CMAKE_SOURCE_DIR}/client/sound.h"
    "${CMAKE_SOURCE_DIR}/client/vid.h"
    "${CMAKE_SOURCE_DIR}/game/game.h"
    "${CMAKE_SOURCE_DIR}/game/q_shared.h"
    "${CMAKE_SOURCE_DIR}/qcommon/qcommon.h"
    "${CMAKE_SOURCE_DIR}/qcommon/qfiles.h"
    "${CMAKE_SOURCE_DIR}/server/server.h"
    # "${CMAKE_SOURCE_DIR}/win32/conproc.h"
    # "${CMAKE_SOURCE_DIR}/win32/winquake.h"
)

target_compile_features(tf-quake2-main PRIVATE cxx_std_20)
set_property(TARGET tf-quake2-main PROPERTY CXX_STANDARD 20)
set_property(TARGET tf-quake2-main PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(tf-quake2-main PRIVATE
    "The-Forge/Common_3/Application/Interfaces"
    "The-Forge/Common_3/Application/ThirdParty/OpenSource/gainput/lib/source"
    "The-Forge/Common_3/Graphics/Interfaces"
    "The-Forge/Common_3/Graphics/ThirdParty/OpenSource/DirectXShaderCompiler/inc"
    "The-Forge/Common_3/OS/Interfaces"
    "The-Forge/Common_3/Resources/ResourceLoader/Interfaces"
    "The-Forge/Common_3/Utilities"
    "The-Forge/Common_3/Utilities/Interfaces"
    "The-Forge/Common_3/Utilities/ThirdParty/OpenSource/Nothings/"
    ${VULKAN_HEADERS_INCLUDE_DIRS}

    "soloud/include"
)

if (WIN32)
    target_link_libraries(tf-quake2-main PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/OS/ThirdParty/OpenSource/winpixeventruntime/bin/WinPixEventRuntime.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/Graphics/ThirdParty/OpenSource/ags/ags_lib/lib/amd_ags_x64.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/Graphics/ThirdParty/OpenSource/nvapi/amd64/nvapi64.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/Graphics/ThirdParty/OpenSource/DirectXShaderCompiler/lib/x64/dxcompiler.lib"
        XInput 
    )

    add_custom_target(CopyDlls ALL      
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tf-quake2-main>/"      
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/OS/ThirdParty/OpenSource/winpixeventruntime/bin/WinPixEventRuntime.dll" "$<TARGET_FILE_DIR:tf-quake2-main>/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/Graphics/ThirdParty/OpenSource/ags/ags_lib/lib/amd_ags_x64.dll" "$<TARGET_FILE_DIR:tf-quake2-main>/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/Graphics/ThirdParty/OpenSource/DirectXShaderCompiler/bin/x64/dxcompiler.dll" "$<TARGET_FILE_DIR:tf-quake2-main>/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/Graphics/ThirdParty/OpenSource/DirectXShaderCompiler/bin/x64/dxil.dll" "$<TARGET_FILE_DIR:tf-quake2-main>/"
    )
endif ()

target_link_libraries(tf-quake2-main PRIVATE  
    the-forge
    gainput
    soloud
)

add_custom_target(CopyGpuCfg ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tf-quake2-main>/GPUCfg/"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Examples_3/Unit_Tests/src/01_Transformations/GPUCfg/gpu.cfg" "$<TARGET_FILE_DIR:tf-quake2-main>/GPUCfg/"
)

add_custom_target(CopyFonts ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tf-quake2-main>/Fonts/TitilliumText/"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Examples_3/Unit_Tests/UnitTestResources/Fonts/TitilliumText/TitilliumText-Bold.otf" "$<TARGET_FILE_DIR:tf-quake2-main>/Fonts/TitilliumText/"
)


if(WIN32)
    set(Python3_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Tools/python-3.6.0-embed-amd64")
endif()

find_package(Python3 3.6 EXACT COMPONENTS Interpreter)

function(compile_shaders)
    set(oneValueArgs TARGET SHADER_LIST)
    cmake_parse_arguments(COMPILE_SHADERS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_custom_target(${COMPILE_SHADERS_TARGET} ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tf-quake2-main>/Shaders"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tf-quake2-main>/CompiledShaders"
        COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/The-Forge/Common_3/Tools/ForgeShadingLanguage/fsl.py"
            "-dShaders" "-bCompiledShaders" "-l DIRECT3D12 VULKAN" "--compile" "--verbose" 
            "${COMPILE_SHADERS_SHADER_LIST}" "--cache-args"
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:tf-quake2-main>"
)
endfunction()

compile_shaders(
    TARGET Shader
    SHADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/shaders/ShaderList.fsl"
)