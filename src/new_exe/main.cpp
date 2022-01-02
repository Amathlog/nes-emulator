#include <new_exe/audio/nesAudioSystem.h>
#include <new_exe/messageService/coreMessageService.h>
#include <new_exe/messageService/messageService.h>
#include <new_exe/messageService/messages/coreMessage.h>
#include <new_exe/mainWindow.h>
#include <iostream>
#include <filesystem>
#include <core/utils/fileVisitor.h>
#include <core/utils/vectorVisitor.h>
#include <core/bus.h>
#include <core/cartridge.h>
#include <vector>


namespace fs = std::filesystem;
using namespace NesEmulatorGL;

static bool enableAudioByDefault = false;
static bool syncWithAudio = false;

int main(int argc, char **argv)
{
    // Load a rom from a file
    auto dir = fs::weakly_canonical(fs::path(argv[0])).parent_path();
    auto root = dir / ".." / ".." / "..";

#ifdef WIN32
    root /= "..";
#endif // WIN32

    // Mapper 000 also
    auto path = root / "tests" / "test_roms" / "nestest.nes";

    // path = root / "roms" / "smb.nes";

    // Check the arg, if there is a file to load
    if (argc > 1)
    {
        path = fs::path(argv[1]);
        if (path.is_relative())
            path = root / path;
    }

    NesEmulator::Bus bus;

    NesAudioSystem audioSystem(bus, syncWithAudio, 1);
    audioSystem.Enable(enableAudioByDefault);

    bus.SetSampleFrequency(audioSystem.GetSampleRate());

    // Create the core message service and connect it
    CoreMessageService messageService(bus, dir.string());
    DispatchMessageServiceSingleton::GetInstance().Connect(&messageService);

    // Load a new game
    DispatchMessageServiceSingleton::GetInstance().Push(LoadNewGameMessage(path.string()));

    {
        NesEmulatorGL::MainWindow mainWindow(256*3, 240*3, bus.GetPPU().GetWidth(), bus.GetPPU().GetHeight());
        mainWindow.ConnectController(bus);

        if (audioSystem.Initialize() || !enableAudioByDefault)
        {            
            while (!mainWindow.RequestedClose())
            {
                if (!syncWithAudio)
                {
                    do
                    {
                        bus.Clock();
                    } while (!bus.GetPPU().IsFrameComplete());
                }
                mainWindow.Update(bus, !syncWithAudio);
            }
        }

        audioSystem.Shutdown();
    }

    // Save the game before closing
    DispatchMessageServiceSingleton::GetInstance().Push(SaveGameMessage());

    // Cleanup
    DispatchMessageServiceSingleton::GetInstance().Disconnect(&messageService);

    return 0;
}