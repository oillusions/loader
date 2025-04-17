#pragma once
#include "IStorageDevice.h"
#include "W25Qxx.h"


class W25QxxStorageDevice : public IStorageDevice
{
    protected:
        W25Qxx flash;
        size_t _storage_size;
        size_t _logical_sector_size = 512;
        size_t _logical_sector_count;
        uint8_t* buffer = nullptr;
    public:
        W25QxxStorageDevice(SPI_HandleTypeDef* hspix);

        void  Init() override;

        void EraseSector(uint32_t address, size_t size) override;
        void ReadSector(uint32_t address, size_t size, uint8_t* buffer) override;
        void WriteSector(uint32_t address, size_t size, uint8_t* data) override;

        size_t GetSectorCount() const override;
        size_t GetSectorSize() const override;
        W25Qxx& GetW25QxxInstance();

        ~W25QxxStorageDevice() override;
};
