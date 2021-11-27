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

    //auto path = root / "tests" / "test_roms" / "nestest.nes";
    auto path = root / "roms" / "smb.nes";
    //auto path = root / "roms" / "donkey_kong.nes";
    //auto path = root / "roms" / "ice_climber.nes";
    NesEmulator::Utils::FileReadVisitor visitor(path.string());


    auto cartridge = std::make_shared<NesEmulator::Cartridge>(visitor);

    NesEmulator::Bus bus;
    bus.InsertCartridge(cartridge);
    bus.Reset();
    //bus.GetCPU().SetPC(0xc000);

    QApplication app (argc, argv);

    NesEmulatorExe::Mode mode = NesEmulatorExe::Mode::NORMAL;

    NesEmulatorExe::MainWindow mainWindow(bus, mode);
    mainWindow.show();

    return app.exec();
}
