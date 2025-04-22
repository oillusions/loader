#include "W25QxxStorageDevice.h"
#include "usart.h"

W25QxxStorageDevice::W25QxxStorageDevice(SPI_HandleTypeDef* hspix)
    : flash(hspix)
{
    buffer = new uint8_t[0x1000];
}

void W25QxxStorageDevice::Init()
{
    flash.Init();
    _storage_size = flash.GetChipSize();
    _logical_sector_count = _storage_size / _logical_sector_size;
}

void W25QxxStorageDevice::EraseSector(uint32_t address, size_t size)
{
    if (size == 0) return;
    uint32_t start_sector_address = address & 0xFFFFF000;
    uint32_t logical_sector_address = address & 0x00000E00;
    size_t erase_sector_size = ((size / 8)+((size % 8)!= 0 ? 1 : 0))+
    ((((logical_sector_address >> 9) + size) / 8));

    for (size_t i = 0; i < erase_sector_size; i++)
    {
        uint32_t sector_address = start_sector_address + (0x1000 * i);
        flash.ResetRead();
        flash.Read(sector_address, buffer, 0x1000);
        
        flash.EraseSector(sector_address);
        flash.WaitUntilReady();
        for (size_t il = 0; il < 8; il++)
        {
            uint32_t write_sector_address = sector_address + (512 * il);
            if ((write_sector_address <(start_sector_address + logical_sector_address))||
                write_sector_address >=((start_sector_address + logical_sector_address) +(size * 512)))
            {
                flash.Write(write_sector_address, 256, &buffer[(il * 512)]);
                flash.WaitUntilReady();
                flash.Write(write_sector_address, 256, &buffer[(il * 512)+ 256]);
                flash.WaitUntilReady();
            }
        }
    }
}

void W25QxxStorageDevice::ReadSector(uint32_t address, size_t size, uint8_t* buffer)
{
    if (size == 0 || buffer == nullptr) return;
    uint32_t sector_address = address & 0xFFFFFE00;

    flash.ResetRead();
    flash.Read(sector_address, buffer, size * 512);
    flash.WaitUntilReady();
}


void W25QxxStorageDevice::WriteSector(uint32_t address, size_t size, uint8_t* data)
{
    if (size == 0 || data == nullptr) return;
    uint32_t sector_address = address & 0xFFFFFE00;

    for (size_t i = 0; i < size; i++)
    {
        flash.Write(sector_address + (i * 512), 256, &data[(i * 512)]);
        flash.WaitUntilReady();
        flash.Write(sector_address + (i * 512)+ 256, 256, &data[(i * 512)+ 256]);
        flash.WaitUntilReady();
    }
}

size_t W25QxxStorageDevice::GetSectorCount() const
{
    return _logical_sector_count;
}

size_t W25QxxStorageDevice::GetSectorSize() const
{
    return _logical_sector_size;
}

W25Qxx& W25QxxStorageDevice::GetW25QxxInstance()
{
    return flash;
}

W25QxxStorageDevice::~W25QxxStorageDevice()
{
    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }
}


