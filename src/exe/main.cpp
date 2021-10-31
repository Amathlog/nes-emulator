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
    NesEmulator::Utils::FileReadVisitor visitor(dir / ".." / ".." / ".." / "tests" / "test_roms" / "nestest.nes");

    auto cartridge = std::make_shared<NesEmulator::Cartridge>(visitor);

    NesEmulator::Bus bus;
    bus.InsertCartridge(cartridge);
    bus.Reset();
    bus.GetCPU().ForceSetPC(0xc004);

    // for (int i = 0; i < 100; ++i)
    // {
    //     bus.Clock();
    // }

    // NesEmulator::Utils::MemoryReadVisitor prgData(cartridge.GetPrgData().data(), cartridge.GetPrgData().size());

    // NesEmulator::Utils::Disassemble(prgData, 0, 0x57);

    QApplication app (argc, argv);

    NesEmulatorExe::Mode mode = NesEmulatorExe::Mode::STEP;

    NesEmulatorExe::MainWindow mainWindow(bus, mode);
    mainWindow.show();

    return app.exec();
}