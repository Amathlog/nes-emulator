
#include <new_exe/messageService/coreMessageService.h>
#include <new_exe/messageService/messages/corePayload.h>
#include <core/utils/fileVisitor.h>
#include <core/utils/vectorVisitor.h>
#include <core/bus.h>
#include <core/cartridge.h>

using NesEmulatorGL::CoreMessageService;

bool CoreMessageService::Push(const Message &message)
{
    if (message.GetType() != DefaultMessageType::CORE)
        return true;
    
    const CorePayload* payload = reinterpret_cast<const CorePayload*>(message.GetPayload());

    switch(payload->m_type)
    {
    case DefaultCoreMessageType::LOAD_NEW_GAME:
        return LoadNewGame(payload->m_data);
    case DefaultCoreMessageType::LOAD_STATE:
        return LoadState();
    case DefaultCoreMessageType::SAVE_STATE:
        return SaveState();
    }

    return true;
}

bool CoreMessageService::LoadNewGame(const std::string& file)
{
    NesEmulator::Utils::FileReadVisitor visitor(file);

    if (!visitor.IsValid())
        return false;

    auto cartridge = std::make_shared<NesEmulator::Cartridge>(visitor);

    m_bus.InsertCartridge(cartridge);
    m_bus.Reset();

    return true;
}

bool CoreMessageService::SaveState()
{
    m_stateData.clear();
    NesEmulator::Utils::VectorWriteVisitor visitor(m_stateData);
    m_bus.SerializeTo(visitor);

    return true;
}

bool CoreMessageService::LoadState()
{
    if (m_stateData.empty())
        return false;

    NesEmulator::Utils::VectorReadVisitor visitor(m_stateData);
    m_bus.Reset();
    m_bus.DeserializeFrom(visitor);

    return true;
}