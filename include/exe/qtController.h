#include <core/controller.h>
#include <cstdint>
#include <memory>
#include <qcoreevent.h>
#include <qevent.h>

namespace NesEmulatorExe {

    class QtController : public NesEmulator::Controller
    {
    public:
        QtController(uint8_t controllerIndex);

        bool UpdateOnEvent(Qt::Key key, bool value);

    private:
        uint8_t m_controllerIndex;
    };
}