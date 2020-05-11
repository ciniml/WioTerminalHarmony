/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "definitions.h"                // SYS function prototypes
#include <cstdint>
#include <array>

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

static DRV_HANDLE spiHandle;

static QueueHandle_t transferQueue;

static void SPITransferEvent(DRV_SPI_TRANSFER_EVENT event, DRV_SPI_TRANSFER_HANDLE transferHandle, std::uintptr_t context)
{
    xQueueSend(transferQueue, &transferHandle, portMAX_DELAY);
}

void APP_Initialize ( void )
{
    transferQueue = xQueueCreate(16, sizeof(DRV_SPI_TRANSFER_HANDLE));

    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    spiHandle = DRV_SPI_Open(sysObj.drvSPI0, static_cast<DRV_IO_INTENT>(DRV_IO_INTENT_BLOCKING | DRV_IO_INTENT_EXCLUSIVE | DRV_IO_INTENT_READWRITE));
    // DRV_SPI_TRANSFER_SETUP setup;
    // setup.chipSelect = SYS_PORT_PIN_NONE;
    // setup.baudRateInHz = 40000000UL;
    // setup.clockPhase = DRV_SPI_CLOCK_PHASE_VALID_LEADING_EDGE;
    // setup.clockPolarity = DRV_SPI_CLOCK_POLARITY_IDLE_LOW;
    // setup.csPolarity = DRV_SPI_CS_POLARITY_ACTIVE_LOW;
    // setup.dataBits = DRV_SPI_DATA_BITS_8;
    // DRV_SPI_TransferSetup(spiHandle, &setup);
    DRV_SPI_TransferEventHandlerSet(spiHandle, &SPITransferEvent, 0);

    USER_LED_OutputEnable();

    LCD_CS_Set();
    LCD_CS_OutputEnable();
    LCD_D_C_Set();
    LCD_D_C_OutputEnable();
    
    LCD_RESET_Clear();
    LCD_RESET_OutputEnable();

    LCD_BACKLIGHT_CTR_Clear();
    LCD_BACKLIGHT_CTR_OutputEnable();
}

static void WriteLcdCommand(std::uint8_t command)
{
    DRV_SPI_TRANSFER_HANDLE handle;
    LCD_D_C_Clear();
    DRV_SPI_WriteTransferAdd(spiHandle, &command, 1, &handle);
    DRV_SPI_TRANSFER_HANDLE completed;
    xQueueReceive(transferQueue, &completed, portMAX_DELAY);
    LCD_D_C_Set();
}

static void WriteLcdData(const std::uint8_t* data, std::size_t length)
{
    DRV_SPI_TRANSFER_HANDLE handle;
    DRV_SPI_WriteTransferAdd(spiHandle, const_cast<std::uint8_t*>(data), length, &handle);
    DRV_SPI_TRANSFER_HANDLE completed;
    xQueueReceive(transferQueue, &completed, portMAX_DELAY);
}


static void WriteLcdCommandData(std::uint8_t command, const std::uint8_t* data, std::size_t length)
{
    WriteLcdCommand(command);
    WriteLcdData(data, length);
}

template<std::size_t N>
static void WriteLcdCommandData(std::uint8_t command, const std::array<std::uint8_t, N>& data)
{
    WriteLcdCommandData(command, data.data(), data.size());
}


static constexpr const std::uint8_t TFT_NOP = 0x00;
static constexpr const std::uint8_t TFT_SWRST = 0x01;

static constexpr const std::uint8_t TFT_CASET = 0x2A;
static constexpr const std::uint8_t TFT_PASET = 0x2B;
static constexpr const std::uint8_t TFT_RAMWR = 0x2C;

static constexpr const std::uint8_t TFT_RAMRD = 0x2E;
static constexpr const std::uint8_t TFT_IDXRD = 0xDD; // ILI9341 only, indexed control register read

static constexpr const std::uint8_t TFT_MADCTL = 0x36;
static constexpr const std::uint8_t TFT_MAD_MY = 0x80;
static constexpr const std::uint8_t TFT_MAD_MX = 0x40;
static constexpr const std::uint8_t TFT_MAD_MV = 0x20;
static constexpr const std::uint8_t TFT_MAD_ML = 0x10;
static constexpr const std::uint8_t TFT_MAD_BGR = 0x08;
static constexpr const std::uint8_t TFT_MAD_MH = 0x04;
static constexpr const std::uint8_t TFT_MAD_RGB = 0x00;

static constexpr const std::uint8_t TFT_INVOFF = 0x20;
static constexpr const std::uint8_t TFT_INVON = 0x21;

static constexpr const std::uint8_t ILI9341_NOP = 0x00;
static constexpr const std::uint8_t ILI9341_SWRESET = 0x01;
static constexpr const std::uint8_t ILI9341_RDDID = 0x04;
static constexpr const std::uint8_t ILI9341_RDDST = 0x09;

static constexpr const std::uint8_t ILI9341_SLPIN = 0x10;
static constexpr const std::uint8_t ILI9341_SLPOUT = 0x11;
static constexpr const std::uint8_t ILI9341_PTLON = 0x12;
static constexpr const std::uint8_t ILI9341_NORON = 0x13;

static constexpr const std::uint8_t ILI9341_RDMODE = 0x0A;
static constexpr const std::uint8_t ILI9341_RDMADCTL = 0x0B;
static constexpr const std::uint8_t ILI9341_RDPIXFMT = 0x0C;
static constexpr const std::uint8_t ILI9341_RDIMGFMT = 0x0A;
static constexpr const std::uint8_t ILI9341_RDSELFDIAG = 0x0F;

static constexpr const std::uint8_t ILI9341_INVOFF = 0x20;
static constexpr const std::uint8_t ILI9341_INVON = 0x21;
static constexpr const std::uint8_t ILI9341_GAMMASET = 0x26;
static constexpr const std::uint8_t ILI9341_DISPOFF = 0x28;
static constexpr const std::uint8_t ILI9341_DISPON = 0x29;

static constexpr const std::uint8_t ILI9341_CASET = 0x2A;
static constexpr const std::uint8_t ILI9341_PASET = 0x2B;
static constexpr const std::uint8_t ILI9341_RAMWR = 0x2C;
static constexpr const std::uint8_t ILI9341_RAMRD = 0x2E;

static constexpr const std::uint8_t ILI9341_PTLAR = 0x30;
static constexpr const std::uint8_t ILI9341_VSCRDEF = 0x33;
static constexpr const std::uint8_t ILI9341_MADCTL = 0x36;
static constexpr const std::uint8_t ILI9341_VSCRSADD = 0x37;
static constexpr const std::uint8_t ILI9341_PIXFMT = 0x3A;

static constexpr const std::uint8_t ILI9341_WRDISBV = 0x51;
static constexpr const std::uint8_t ILI9341_RDDISBV = 0x52;
static constexpr const std::uint8_t ILI9341_WRCTRLD = 0x53;

static constexpr const std::uint8_t ILI9341_FRMCTR1 = 0xB1;
static constexpr const std::uint8_t ILI9341_FRMCTR2 = 0xB2;
static constexpr const std::uint8_t ILI9341_FRMCTR3 = 0xB3;
static constexpr const std::uint8_t ILI9341_INVCTR = 0xB4;
static constexpr const std::uint8_t ILI9341_DFUNCTR = 0xB6;

static constexpr const std::uint8_t ILI9341_PWCTR1 = 0xC0;
static constexpr const std::uint8_t ILI9341_PWCTR2 = 0xC1;
static constexpr const std::uint8_t ILI9341_PWCTR3 = 0xC2;
static constexpr const std::uint8_t ILI9341_PWCTR4 = 0xC3;
static constexpr const std::uint8_t ILI9341_PWCTR5 = 0xC4;
static constexpr const std::uint8_t ILI9341_VMCTR1 = 0xC5;
static constexpr const std::uint8_t ILI9341_VMCTR2 = 0xC7;

static constexpr const std::uint8_t ILI9341_RDID4 = 0xD3;
static constexpr const std::uint8_t ILI9341_RDINDEX = 0xD9;
static constexpr const std::uint8_t ILI9341_RDID1 = 0xDA;
static constexpr const std::uint8_t ILI9341_RDID2 = 0xDB;
static constexpr const std::uint8_t ILI9341_RDID3 = 0xDC;
static constexpr const std::uint8_t ILI9341_RDIDX = 0xDD; // TBC

static constexpr const std::uint8_t ILI9341_GMCTRP1 = 0xE0;
static constexpr const std::uint8_t ILI9341_GMCTRN1 = 0xE1;

static constexpr const std::uint8_t ILI9341_MADCTL_MY = 0x80;
static constexpr const std::uint8_t ILI9341_MADCTL_MX = 0x40;
static constexpr const std::uint8_t ILI9341_MADCTL_MV = 0x20;
static constexpr const std::uint8_t ILI9341_MADCTL_ML = 0x10;
static constexpr const std::uint8_t ILI9341_MADCTL_RGB = 0x00;
static constexpr const std::uint8_t ILI9341_MADCTL_BGR = 0x08;
static constexpr const std::uint8_t ILI9341_MADCTL_MH = 0x04;

static void ResetLcd()
{
    FSYNC_OUT_Clear();
    FSYNC_OUT_OutputEnable();

    LCD_CS_Set();
    LCD_RESET_Clear();
    vTaskDelay(pdMS_TO_TICKS(150));
    LCD_RESET_Set();
    vTaskDelay(pdMS_TO_TICKS(150));
    
    LCD_CS_Clear();
    WriteLcdCommandData<3>(0xef, {0x03, 0x80, 0x02});
    WriteLcdCommandData<3>(0xcf, {0x00, 0xc1, 0x30});
    WriteLcdCommandData<4>(0xed, {0x64, 0x03, 0x12, 0x81});
    WriteLcdCommandData<3>(0xe8, {0x85, 0x00, 0x78});
    WriteLcdCommandData<5>(0xcb, {0x39, 0x2c, 0x00, 0x34, 0x02});
    WriteLcdCommandData<1>(0xf7, {0x20});
    WriteLcdCommandData<2>(0xea, {0x00, 0x00});
    WriteLcdCommandData<1>(ILI9341_PWCTR1, {0x23});
    WriteLcdCommandData<1>(ILI9341_PWCTR2, {0x10});
    WriteLcdCommandData<2>(ILI9341_VMCTR1, {0x3e, 0x28});
    WriteLcdCommandData<1>(ILI9341_VMCTR2, {0x86});
    WriteLcdCommandData<1>(ILI9341_MADCTL, {0xa8});
    WriteLcdCommandData<1>(ILI9341_PIXFMT, {0x55});
    WriteLcdCommandData<2>(ILI9341_FRMCTR1, {0x00, 0x13});
    WriteLcdCommandData<3>(ILI9341_DFUNCTR, {0x08, 0x82, 0x27});
    WriteLcdCommandData<1>(0xf2, {0x00});
    WriteLcdCommandData<1>(ILI9341_GAMMASET, {0x01});
    WriteLcdCommandData<15>(ILI9341_GMCTRP1, {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00});
    WriteLcdCommandData<15>(ILI9341_GMCTRN1, {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F});
    WriteLcdCommand(ILI9341_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(500));
    WriteLcdCommand(ILI9341_DISPON);
    WriteLcdCommandData<1>(TFT_MADCTL, {TFT_MAD_BGR | 0xe0});
    LCD_CS_Set();

    LCD_BACKLIGHT_CTR_Set();
}

static void SetLcdColumnAddress(std::uint_fast16_t start, std::uint_fast16_t end)
{
    std::array<std::uint8_t, 4> buffer = {
        static_cast<std::uint8_t>(start >> 8),
        static_cast<std::uint8_t>(start & 0xff),
        static_cast<std::uint8_t>(end >> 8),
        static_cast<std::uint8_t>(end & 0xff),
    };
    WriteLcdCommandData(0x2a, buffer);
}
static void SetLcdPageAddress(std::uint_fast16_t start, std::uint_fast16_t end)
{
    std::array<std::uint8_t, 4> buffer = {
        static_cast<std::uint8_t>(start >> 8),
        static_cast<std::uint8_t>(start & 0xff),
        static_cast<std::uint8_t>(end >> 8),
        static_cast<std::uint8_t>(end & 0xff),
    };
    WriteLcdCommandData(0x2b, buffer);
}
static void StartLcdMemoryWrite()
{
    WriteLcdCommand(0x2c);
}

static std::array<std::uint8_t, 320*2> line_buffer;
static void FillLcd(std::uint_fast16_t x0, std::uint_fast16_t y0, std::uint_fast16_t x1, std::uint_fast16_t y1, std::uint_fast16_t color)
{
    auto width = x1 - x0;
    for(std::uint_fast16_t x = 0; x < width; x++) {
        line_buffer[x*2 + 0] = static_cast<std::uint8_t>(color >> 8);
        line_buffer[x*2 + 1] = static_cast<std::uint8_t>(color & 0xff);
    }
    LCD_CS_Clear();
    SetLcdColumnAddress(x0, x1);
    SetLcdPageAddress(y0, y1);
    StartLcdMemoryWrite();
    DRV_SPI_TRANSFER_HANDLE handles[] = {
        DRV_SPI_TRANSFER_HANDLE_INVALID,
        DRV_SPI_TRANSFER_HANDLE_INVALID,
    };
    std::uint_fast32_t index = 0;
    for(std::uint_fast16_t y = y0; y < y1; y++) {
        if( handles[index] != DRV_SPI_TRANSFER_HANDLE_INVALID) {
            DRV_SPI_TRANSFER_HANDLE completed;
            xQueueReceive(transferQueue, &completed, portMAX_DELAY);
            handles[index] = DRV_SPI_TRANSFER_HANDLE_INVALID;
        }
        //WriteLcdData(line_buffer.data(), width*2);
        DRV_SPI_WriteTransferAdd(spiHandle, line_buffer.data(), width*2, handles+index);
        index ^= 1;
    }
    for(index = 0; index < 2; index++) {
        if( handles[index] != DRV_SPI_TRANSFER_HANDLE_INVALID) {
            DRV_SPI_TRANSFER_HANDLE completed;
            xQueueReceive(transferQueue, &completed, portMAX_DELAY);
        }
    }
    LCD_CS_Set();
}
/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

static int color = 0;

void APP_Tasks ( void )
{
    
    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            ResetLcd();
            FillLcd(0, 0, 320, 240, 0);
            if (appInitialized)
            {
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            USER_LED_Toggle();
            FSYNC_OUT_Set();
            switch(color)
            {
                case 0: FillLcd(0, 0, 320, 240, 0x1f << 11); break;
                case 1: FillLcd(0, 0, 320, 240, 0x3f << 5); break;
                case 2: FillLcd(0, 0, 320, 240, 0x1f << 0); break;
                case 3: FillLcd(0, 0, 320, 240, 0); break;
            }
            color = (color + 1) & 3;
            FSYNC_OUT_Clear();
            break;
        }

        
        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
