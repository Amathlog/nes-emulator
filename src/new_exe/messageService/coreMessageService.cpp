
#include "new_exe/messageService/messages/coreMessage.h"
#include <new_exe/messageService/coreMessageService.h>
#include <new_exe/messageService/messages/corePayload.h>
#include <core/utils/fileVisitor.h>
#include <core/utils/vectorVisitor.h>
#include <core/bus.h>
#include <core/cartridge.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

using NesEmulatorGL::CoreMessageService;

bool CreateFolders(const std::string& file)
{
    if (file.empty())
        return true;

    auto parent = fs::path(file).parent_path();
    if (!fs::exists(parent))
    {
        if (!fs::create_directories(parent))
        {
            std::cerr << "Couldn't create the folders for file " << file << std::endl;
            return false;
        }
    }

    return true;
}

bool CoreMessageService::Push(const Message &message)
{
    if (message.GetType() != DefaultMessageType::CORE)
        return true;
    
    const CorePayload* payload = reinterpret_cast<const CorePayload*>(message.GetPayload());

    switch(payload->m_type)
    {
    case DefaultCoreMessageType::LOAD_NEW_GAME:
        return LoadNewGame(payload->m_data);
    case DefaultCoreMessageType::SAVE_GAME:
        return SaveGame(payload->m_data);
    case DefaultCoreMessageType::LOAD_SAVE:
        return LoadSaveGame(payload->m_data);
    case DefaultCoreMessageType::LOAD_STATE:
        return LoadState(payload->m_data, payload->m_saveStateNumber);
    case DefaultCoreMessageType::SAVE_STATE:
        return SaveState(payload->m_data, payload->m_saveStateNumber);
    }

    return true;
}

bool CoreMessageService::LoadNewGame(const std::string& file)
{
    // Try to save the previous game before loading
    SaveGame("");

    NesEmulator::Utils::FileReadVisitor visitor(file);

    if (!visitor.IsValid())
        return false;

    auto cartridge = std::make_shared<NesEmulator::Cartridge>(visitor);

    m_bus.InsertCartridge(cartridge);
    m_bus.Reset();

    // Try to load an existing save
    LoadSaveGame("");

    return true;
}

bool CoreMessageService::SaveState(const std::string& file, int number)
{
    std::string finalFile = file;
    if (file.empty())
    {
        finalFile = m_bus.GetSaveStateFile(m_exePath, number).string();
    }

    if (finalFile.empty())
        return true;

    CreateFolders(finalFile);

    NesEmulator::Utils::FileWriteVisitor visitor(finalFile);
    if (!visitor.IsValid())
    {
        std::cerr << "Couldn't open file " << finalFile << std::endl;
        return false; 
    }

    m_bus.SerializeTo(visitor);

    return true;
}

bool CoreMessageService::LoadState(const std::string& file, int number)
{
    std::string finalFile = file;
    if (file.empty())
    {
        finalFile = m_bus.GetSaveStateFile(m_exePath, number).string();
    }

    if (finalFile.empty())
        return true;

    NesEmulator::Utils::FileReadVisitor visitor(finalFile);
    if (!visitor.IsValid())
    {
        std::cerr << "Couldn't open file " << file << std::endl;
        return false; 
    }

    m_bus.Reset();
    m_bus.DeserializeFrom(visitor);

    return true;
}

bool CoreMessageService::SaveGame(const std::string& file)
{
    std::string finalFile = file;
    if (file.empty())
    {
        finalFile = m_bus.GetSaveFile(m_exePath).string();
    }

    // Nothing to do
    if (finalFile.empty())
        return true;

    CreateFolders(finalFile);

    NesEmulator::Utils::FileWriteVisitor visitor(finalFile);
    if (!visitor.IsValid())
    {
        std::cerr << "Couldn't open file " << finalFile << std::endl;
        return false; 
    }

    m_bus.SaveRAM(visitor);
    return true;
}

bool CoreMessageService::LoadSaveGame(const std::string& file)
{
    std::string finalFile = file;
    if (file.empty())
    {
        finalFile = m_bus.GetSaveFile(m_exePath).string();
    }

    if (finalFile.empty())
        return true;

    NesEmulator::Utils::FileReadVisitor visitor(finalFile);
    if (!visitor.IsValid())
    {
        std::cerr << "Couldn't open file " << file << std::endl;
        return false; 
    }

    m_bus.LoadRAM(visitor);
    return true;
}