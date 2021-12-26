#include "new_exe/mainWindow.h"
#include <iostream>
#include <filesystem>
#include <core/utils/fileVisitor.h>
#include <core/utils/vectorVisitor.h>
#include <core/bus.h>
#include <core/cartridge.h>
#include <vector>


namespace fs = std::filesystem;

void LoadNewGame(std::string path, NesEmulator::Bus& bus)
{
    NesEmulator::Utils::FileReadVisitor visitor(path);


    auto cartridge = std::make_shared<NesEmulator::Cartridge>(visitor);

    bus.InsertCartridge(cartridge);
    bus.Reset();
}

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

    //path = root / "roms" / "smb.nes";

    // Check the arg, if there is a file to load
    if (argc > 1)
    {
        path = fs::path(argv[1]);
        if (path.is_relative())
            path = root / path;
    }

    NesEmulator::Bus bus;
    LoadNewGame(path.string(), bus);

    std::vector<uint8_t> stateData;

    {
        NesEmulatorGL::MainWindow mainWindow(256*3, 240*3, bus.GetPPU().GetWidth(), bus.GetPPU().GetHeight());
        mainWindow.ConnectController(bus);

        while (!mainWindow.RequestedClose())
        {
            std::string newFileToLoad = mainWindow.GetPathToNewGame();
            if (!newFileToLoad.empty())
            {
                LoadNewGame(newFileToLoad, bus);
            }

            if (mainWindow.ShouldSaveState())
            {
                stateData.clear();
                NesEmulator::Utils::VectorWriteVisitor visitor(stateData);
                bus.SerializeTo(visitor);
            }

            if (mainWindow.ShouldLoadState() && !stateData.empty())
            {
                NesEmulator::Utils::VectorReadVisitor visitor(stateData);
                bus.Reset();
                bus.DeserializeFrom(visitor);
            }

            do
            {
                bus.Clock();
            } while (!bus.GetPPU().IsFrameComplete());

            mainWindow.Update(bus);
        }
    }

    return 0;
}