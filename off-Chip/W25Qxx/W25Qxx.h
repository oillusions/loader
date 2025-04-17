#pragma once
#include "stm32g0xx_hal.h"


typedef enum {
    //读取状态寄存器高位
    W25Qxx_Read_Status_Up = 0x35,
    //读取状态寄存器低位
    W25Qxx_Read_Status_Down = 0x05,

    //读取芯片唯一ID
    W25Qxx_Read_Chip_UID = 0x4B,
    //读取芯片ID
    W25Qxx_Read_Chip_DID = 0x9F,

    //读取数据
    W25Qxx_Read_Data = 0x03,

    //写入使能
    W25Qxx_Write_Enable = 0x06,
    //写入失能
    W25Qxx_Write_Disable = 0x04,

    //写入状态寄存器
    W25Qxx_Write_Status = 0x01,

    //写入数据
    W25Qxx_Write_Data = 0x02,

    //擦除块-64KB
    W25Qxx_Erase_Block_64 = 0xD8,
    //擦除块-32KB
    W25Qxx_Erase_Block_32 = 0x52,
    //擦除扇区-4KB
    W25Qxx_Erase_Sector = 0x20,
    //擦除整片
    W25Qxx_Erase_Chip = 0xC7,
    //擦除暂停
    W25Qxx_Stop_Erase = 0x75,
    //擦除恢复
    W25Qxx_Resume_Erase = 0x7A,
    //低功耗电源模式
    W25Qxx_PowerMode_Low = 0xB9,
    //正常电源模式
    W25Qxx_PowerMode_Reset = 0xAB,
    //批量读取模式重置
    W25Qxx_ReadMode_Continuous_Reset = 0xFF,
    
    // W25Qxx_ = 0x90,
    
    //高性能模式
    W25Qxx_Performance_High  = 0xA3,
    
    //写入数据-QSPI
    W25Qxx_Write_Data_4 = 0x32,
    // W25Qxx_Read_Data_4 = 0x0B,
    // W25Qxx_Read_Data_2 = 0x3B,
    // W25Qxx_Read_ = 0xBB,
    // W25Qxx_ = 0x6B,
    // W25Qxx_ = 0xEB,
    // W25Qxx_ = 0xE3,
} W25Qxx_Instruction;

class W25Qxx
{
    protected:
        uint8_t chip_id[3];         // 芯片ID
        uint64_t chip_UID;          // 芯片唯一ID
        uint32_t chip_size;         // 芯片容量
        uint8_t address_len = 3;    // 地址长度
        uint8_t buffer[256] = {0};  // 操作缓冲区[弃用]
        
        SPI_HandleTypeDef* _hSPIx;

        void SendData(size_t data_len, uint8_t* data);
        void ReadData( uint8_t* buffer, size_t buffer_len);
        void AddressBytes(uint32_t address, uint8_t *addrBytes);

        void Affairs(W25Qxx_Instruction instruction);
        void Affairs(W25Qxx_Instruction instruction, size_t data_len, uint8_t* data);
        void Affairs(W25Qxx_Instruction instruction, uint8_t* buffer, size_t buffer_len);
        void Affairs(W25Qxx_Instruction instruction, size_t data_len, size_t buffer_len, uint8_t* data, uint8_t* buffer);
        void Affairs(W25Qxx_Instruction instruction, size_t param_len, uint8_t *param,  size_t data_len, uint8_t *data);

    public:
        W25Qxx(SPI_HandleTypeDef* hSPIx);
        void Init();

        void EraseSector(uint32_t address);
        void Write(uint32_t address, size_t size, uint8_t* data);
        void Read(uint32_t address, uint8_t* buffer, size_t size);

        void Config(W25Qxx_Instruction instruction);
        void Config(W25Qxx_Instruction instruction, uint32_t address);
        void Config(W25Qxx_Instruction instruction, size_t len, uint8_t* data);
        void ResetRead();

        size_t GetChipSize() const;
        const uint8_t *GetChipID() const;
        uint64_t GetChipUID() const;

        bool IsBusy();
        void WaitUntilReady();
        ~W25Qxx();
};
