#include <QApplication>
#include <QPushButton>

#include <core/utils/fileVisitor.h>
#include <core/bus.h>
#include <core/utils/memoryVisitor.h>
#include <core/cartridge.h>
#include <core/utils/disassembly.h>
#include <filesystem>
#include <exe/windows/mainWindow.h>

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    // Load a rom from a file
    auto dir = fs::weakly_canonical(fs::path(argv[0])).parent_path();
    auto root = dir / ".." / ".." / "..";

    // Mapper 000 also
    auto path = root / "tests" / "test_roms" / "nestest.nes";

    // Mapper 000
    // path = root / "roms" / "smb.nes";
    // path = root / "roms" / "donkey_kong.nes";
    // path = root / "roms" / "ice_climber.nes";

    // Mapper 001
    // path = root / "roms" / "zelda1.nes";

    // Mapper 002
    // path = root / "roms" / "ducktales.nes";

    // Mapper 003
    // path = root / "roms" / "donkeykong_classics.nes";

    // Mapper 040
    // path = root / "roms" / "smb_lost_levels.nes";

    // Mapper 004
    // path = root / "roms" / "smb2.nes";
    // path = root / "roms" / "smb3.nes";

    // Mapper 066
    // path = root / "roms" / "smb_duckhunt.nes";

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
    //bus.GetCPU().SetPC(0xc000);

    QApplication app (argc, argv);

    NesEmulatorExe::Mode mode = NesEmulatorExe::Mode::NORMAL;
    NesEmulatorExe::Layout layout = NesEmulatorExe::Layout::NORMAL;

    NesEmulatorExe::MainWindow mainWindow(bus, mode, layout);
    mainWindow.show();

    return app.exec();
}
