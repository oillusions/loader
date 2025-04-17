#include "W25Qxx.h"
#include "spi.h"
#include "gpio.h"
#include "dma.h"

// 片选引脚
#define CS(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (GPIO_PinState)x)

/**
 * @brief 发送数据-SPI
 * @param data_len: 数据长度
 * @param data: 数据指针
 * @retval none
 */
void W25Qxx::SendData(size_t data_len, uint8_t* data)
{
    HAL_SPI_Transmit_DMA(_hSPIx, data, data_len);
    while (HAL_SPI_GetState(_hSPIx) != HAL_SPI_STATE_READY);
}

/**
 * @brief 读取数据-SPI
 * @param data_len: 数据长度
 * @param data: 数据缓冲区指针
 * @retval none
 */
void W25Qxx::ReadData(uint8_t* buffer, size_t buffer_len)
{
    HAL_SPI_Receive_DMA(_hSPIx, buffer, buffer_len);
    while (HAL_SPI_GetState(_hSPIx) != HAL_SPI_STATE_READY);
}

/**
 * @brief 地址转换
 * @param address: 原地址
 * @param out_address: 输出地址
 * @retval none
 */
void W25Qxx::AddressBytes(uint32_t address, uint8_t *addrBytes) 
{
    if (address_len == 4) {
        addrBytes[0] = (address >> 24) & 0xFF;
        addrBytes[1] = (address >> 16) & 0xFF;
        addrBytes[2] = (address >> 8) & 0xFF;
        addrBytes[3] = address & 0xFF;
    } else { 
        addrBytes[0] = (address >> 16) & 0xFF;
        addrBytes[1] = (address >> 8) & 0xFF;
        addrBytes[2] = address & 0xFF;
    }
}

/**
 * @brief 事务-单指令
 * @param instruction: 指令
 * @retval none
 */
void W25Qxx::Affairs(W25Qxx_Instruction instruction)
{
    CS(0);
    uint8_t cmd = static_cast<uint8_t>(instruction);
    SendData(1, &cmd);
    CS(1);
}

/**
 * @brief 事务-指令+发送数据/参数
 * @param instruction: 指令
 * @param data_len: 数据长度
 * @param data: 数据指针
 * @retval none
 */
void W25Qxx::Affairs(W25Qxx_Instruction instruction, size_t data_len, uint8_t* data)
{
    if (data_len == 0 || data == NULL) return;
    uint8_t cmd = static_cast<uint8_t>(instruction);
    CS(0);
    SendData(1, &cmd);
    SendData(data_len, data);
    CS(1);
}

/**
 * @brief 事务-指令+读取数据
 * @param instruction: 指令
 * @param data_len: 读取大小
 * @param data: 数据缓冲区指针
 * @retval none
 */
void W25Qxx::Affairs(W25Qxx_Instruction instruction, uint8_t* buffer, size_t buffer_len)
{
    if (buffer_len == 0 || buffer == NULL) return;
    uint8_t cmd = static_cast<uint8_t>(instruction);
    CS(0);
    SendData(1, &cmd);
    ReadData(buffer, buffer_len);
    CS(1);
}

/**
 * @brief 事务-指令+发送参数+读取数据
 * @param instruction: 指令
 * @param param_len: 参数长度
 * @param buffer_len: 读取大小
 * @param param: 参数指针
 * @param buffer: 数据缓冲区指针
 * @retval none
 */
void W25Qxx::Affairs(W25Qxx_Instruction instruction, size_t param_len, size_t buffer_len, uint8_t* param, uint8_t* buffer)
{
    if (param_len == 0 || param == NULL || buffer_len == 0 || buffer == NULL) return;
    uint8_t cmd = static_cast<uint8_t>(instruction);
    CS(0);
    SendData(1, &cmd);
    SendData(param_len, param);
    ReadData(buffer, buffer_len);
    CS(1);
}

/**
 * @brief 事务-指令+发送参数+发送数据
 * @param instruction: 指令
 * @param param_len: 参数长度
 * @param param: 参数指针
 * @param buffer_len: 数据长度
 * @param data: 数据缓冲区指针
 * @retval none
 */
void W25Qxx::Affairs(W25Qxx_Instruction instruction, size_t param_len, uint8_t* param, size_t data_len, uint8_t* data)
{
    if (param_len == 0 || param == NULL || data_len == 0 || data == NULL) return;
    uint8_t cmd = static_cast<uint8_t>(instruction);
    CS(0);
    SendData(1, &cmd);
    SendData(param_len, param);
    ReadData(data, data_len);
    CS(1);
}

/**
 * @brief W25Qxx-构造方法， 负责向对象填充SPI句柄
 * @param hSPIx SPI句柄类型指针
 */
W25Qxx::W25Qxx(SPI_HandleTypeDef* hSPIx)
{
    if (hSPIx == NULL) return;

    this->_hSPIx = hSPIx;
}

/**
 * @brief W25Qxx-初始化方法, 负责初始化对象，并读取设备ID和设备UID, 判断芯片容量. 请注意， 在调用前请初始化所需的: GPIO, SPI, DMA 
 * @retval none
 */
void W25Qxx::Init()
{
    HAL_Delay(10);

    Affairs(W25Qxx_Read_Chip_DID, chip_id, 3);
    Affairs(W25Qxx_Read_Chip_UID, 4, 8, (uint8_t[]){0x00, 0x00, 0x00, 0x00}, reinterpret_cast<uint8_t*>(&chip_UID));
    address_len = 3;

    switch (chip_id[2])
    {
        case 0x13: {
            chip_size = 0x80000;
            break;
        }
        case 0x14: {
            chip_size = 0x100000;
            break;
        }
        case 0x15: {
            chip_size = 0x200000;
            break;
        }
        case 0x16: {
            chip_size = 0x400000;
            break;
        }
        case 0x17: {
            chip_size = 0x800000;
            break;
        }
        case 0x18: {
            chip_size = 0x1000000;
            address_len = 4;
            break;
        }
        case 0x19: {
            chip_size = 0x2000000;
            address_len = 4;
            break;
        }
    }
}

/**
 * @brief W25Qxx-获取芯片容量方法
 * @retval size_t
 */
size_t W25Qxx::GetChipSize() const
{
    
    return chip_size;
}

/**
 * @brief W25Qxx-配置方法-单指令
 * @param instruction: 指令
 * @retval none
 */
void W25Qxx::Config(W25Qxx_Instruction instruction)
{
    Affairs(instruction);
}

/**
 * @brief W25Qxx-配置方法-指令+参数
 * @param instruction: 指令
 * @param data_len: 数据长度
 * @param data: 数据指针
 * @retval none
 */
void W25Qxx::Config(W25Qxx_Instruction instruction, size_t data_len, uint8_t* data)
{
    Affairs(instruction, data_len, data);
}

/**
 * @brief W25Qxx-配置方法-指令+指针
 * @param instruction: 指令
 * @param address: 地址
 * @retval none
 */
void W25Qxx::Config(W25Qxx_Instruction instruction, uint32_t address)
{
    uint8_t address_bytes[4];
    AddressBytes(address, address_bytes);
    Affairs(instruction, address_len, address_bytes);
}

/**
 * @brief W25Qxx-写入方法
 * @param address: 地址
 * @param size: 数据大小
 * @param data: 数据指针
 * @retval none
 */
void W25Qxx::Write(uint32_t address, size_t size, uint8_t* data)
{
    uint8_t address_bytes[4];

    Affairs(W25Qxx_Write_Enable);
    AddressBytes(address, address_bytes);
    Affairs(W25Qxx_Write_Data, address_len, address_bytes, size, data);
}

/**
 * @brief W25Qxx-擦除方法
 * @param address: 地址
 * @retval none
 */
void W25Qxx::EraseSector(uint32_t address)
{
    uint8_t address_bytes[4];

    Affairs(W25Qxx_Write_Enable);
    AddressBytes(address, address_bytes);
    Affairs(W25Qxx_Erase_Sector, address_len, address_bytes);
}

/**
 * @brief W25Qxx-读取方法
 * @param address: 地址
 * @param buffer: 数据缓冲区指针
 * @param size: 读取大小
 * @retval none
 */
void W25Qxx::Read(uint32_t address, uint8_t* buffer, size_t size)
{
    uint8_t address_bytes[4];
    
    AddressBytes(address, address_bytes);
    Affairs(W25Qxx_Read_Data, address_len, size, address_bytes, buffer);
}

/**
 * @brief W25Qxx-重置读取计数器方法
 * @retval none
 */
void W25Qxx::ResetRead()
{
    Config(W25Qxx_ReadMode_Continuous_Reset);
}

/**
 * @brief W25Qxx-获取芯片ID方法
 * @retval uint8_t
 */
const uint8_t* W25Qxx::GetChipID() const 
{
    return chip_id;
}

/**
 * @brief W25Qxx-获取芯片UID方法
 * @retval uint64_t
 */
uint64_t W25Qxx::GetChipUID() const
{
    return this->chip_UID;
}

/**
 * @brief W25Qxx-获取芯片忙状态方法
 * @retval bool
 */
bool W25Qxx::IsBusy()
{
    uint8_t status;
    Affairs(W25Qxx_Read_Status_Down, &status, 1);
    return (status & 0x01);
}

/**
 * @brief W25Qxx-忙等待方法
 * @retval none
 */
void W25Qxx::WaitUntilReady()
{
    while (IsBusy()) {
        HAL_Delay(1);
    }
}

/**
 * @brief W25Qxx-析构方法
 */
W25Qxx::~W25Qxx()
{

}

