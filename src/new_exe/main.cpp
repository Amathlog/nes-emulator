#include "new_exe/mainWindow.h"
#include <iostream>
#include <filesystem>
#include <core/utils/fileVisitor.h>
#include <core/bus.h>
#include <core/cartridge.h>


namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    // Load a rom from a file
    auto dir = fs::weakly_canonical(fs::path(argv[0])).parent_path();
    auto root = dir / ".." / ".." / "..";

    // Mapper 000 also
    auto path = root / "tests" / "test_roms" / "nestest.nes";

    // Check the arg, if there is a file to load
    if (argc > 1)
    {
        path = fs::path(argv[1]);
        if (path.is_relative())
            path = root / path;
    }

    NesEmulator::Utils::FileReadVisitor visitor(path.string());


    auto cartridge = std::make_shared<NesEmulator::Cartridge>(visitor);

    NesEmulator::Bus bus;
    bus.InsertCartridge(cartridge);
    bus.Reset();

    {
        NesEmulatorGL::MainWindow mainWindow(640, 480);

        while (!mainWindow.RequestedClose())
        {
            do
            {
                bus.Clock();
            } while (!bus.GetPPU().IsFrameComplete());

            mainWindow.Update();
        }
    }

    return 0;
}