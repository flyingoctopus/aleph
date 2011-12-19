#ifndef _CONFIG_H_
#define _CONFIG_H_

// define encoder pins
#define CON_ENC0_S0		AVR32_PIN_PA25
#define CON_ENC0_S1		AVR32_PIN_PA26
#define CON_ENC1_S0		AVR32_PIN_PA27
#define CON_ENC1_S1		AVR32_PIN_PA28

// define switch pins
#define CON_SW0			AVR32_PIN_PX16
#define CON_SW1			AVR32_PIN_PX19
#define CON_SW2			AVR32_PIN_PX22

// define SPI pins
#define BFIN_SPI                   (&AVR32_SPI0)
#define BFIN_SPI_NPCS              0
#define BFIN_SPI_SCK_PIN           AVR32_SPI0_SCK_0_0_PIN
#define BFIN_SPI_SCK_FUNCTION      AVR32_SPI0_SCK_0_0_FUNCTION
#define BFIN_SPI_MISO_PIN          AVR32_SPI0_MISO_0_0_PIN
#define BFIN_SPI_MISO_FUNCTION     AVR32_SPI0_MISO_0_0_FUNCTION
#define BFIN_SPI_MOSI_PIN          AVR32_SPI0_MOSI_0_0_PIN
#define BFIN_SPI_MOSI_FUNCTION     AVR32_SPI0_MOSI_0_0_FUNCTION
#define BFIN_SPI_NPCS_PIN          AVR32_SPI0_NPCS_0_0_PIN
#define BFIN_SPI_NPCS_FUNCTION     AVR32_SPI0_NPCS_0_0_FUNCTION

// define UART pins
// #define BFIN_UART
// #define DBG_UART

#endif // header guard
