#pragma once
#include <stdint.h>
#include <stddef.h>


class IStorageDevice
{
    public:
        void virtual Init() = 0;

        void virtual EraseSector(uint32_t address, size_t size) = 0;
        void virtual ReadSector(uint32_t address, size_t size, uint8_t* buffer) = 0;
        void virtual WriteSector(uint32_t address, size_t size, uint8_t* data) = 0;

        size_t virtual GetSectorCount() const = 0;
        size_t virtual GetSectorSize() const = 0;
        
        virtual ~IStorageDevice() = default;
};
