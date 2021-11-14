#include <cstdint>
#include <exe/qtController.h>
#include <qcoreevent.h>
#include <qnamespace.h>
#include <iostream>


using NesEmulatorExe::QtController;

QtController::QtController(uint8_t controllerIndex)
    : m_controllerIndex(controllerIndex & 0x01)
{}

bool QtController::UpdateOnEvent(Qt::Key key, bool value)
{
    switch(key)
    {
        // A
        case Qt::Key_Z: // Player1
            if (m_controllerIndex == 0) ToggleA(value);
            break;
        case Qt::Key_C: // Player2
            if (m_controllerIndex == 1) ToggleA(value);
            break;

        // B
        case Qt::Key_X: // Player1
            if (m_controllerIndex == 0) ToggleB(value);
            break;
        case Qt::Key_V: // Player2
            if (m_controllerIndex == 1) ToggleB(value);
            break;

        // Select
        case Qt::Key_A: // Player1
            if (m_controllerIndex == 0) ToggleSelect(value);
            break;
        case Qt::Key_D: // Player2
            if (m_controllerIndex == 1) ToggleSelect(value);
            break;

        // Start
        case Qt::Key_S: // Player1
            if (m_controllerIndex == 0) ToggleStart(value);
            break;
        case Qt::Key_F: // Player2
            if (m_controllerIndex == 1) ToggleStart(value);
            break;

        // Up
        case Qt::Key_Up: // Player1
            if (m_controllerIndex == 0) ToggleUp(value);
            break;
        case Qt::Key_I: // Player2
            if (m_controllerIndex == 1) ToggleUp(value);
            break;

        // Down
        case Qt::Key_Down: // Player1
            if (m_controllerIndex == 0) ToggleDown(value);
            break;
        case Qt::Key_K: // Player2
            if (m_controllerIndex == 1) ToggleDown(value);
            break;

        // Left
        case Qt::Key_Left: // Player1
            if (m_controllerIndex == 0) ToggleLeft(value);
            break;
        case Qt::Key_J: // Player2
            if (m_controllerIndex == 1) ToggleLeft(value);
            break;

        // Right
        case Qt::Key_Right: // Player1
            if (m_controllerIndex == 0) ToggleRight(value);
            break;
        case Qt::Key_L: // Player2
            if (m_controllerIndex == 1) ToggleRight(value);
            break;
        default:
            return false;
    }

    return true;
}