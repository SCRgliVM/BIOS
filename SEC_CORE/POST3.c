#include <SEC_CORE.h>

extern PCIE_CONG_SPACE_LENGTH_FLAG, PCIEXBA, MCHBA;

enum PCIEXBAR {
    _64MB = 4,
    _128MB = 2,
    _256MB = 0
};

#define enable 1
#define MCHBAR 0x48     // Host Memory Mapped Register Range Base
#define P2SB   0xF9000  // P2S Bridge
#define HPTC   0x60     // High Performance Event Timer Configuration

void Init_HPET_MMIO_and_Compare_Register(void);

void init_PCIE_and_host_bridge_config_space(void){
    unsigned short int PCIE_CONG_SPACE_LENGTH;

    __outdword(PCI_CSAR, 0x80000060);          // Host Bridge PCI Express Register Range Base Address(PCIEXBAR)
    // Choose length
    PCIE_CONG_SPACE_LENGTH = _64MB;
    if      (PCIE_CONG_SPACE_LENGTH_FLAG == 0x10000000){
        PCIE_CONG_SPACE_LENGTH = _256MB;
    }
    else if (PCIE_CONG_SPACE_LENGTH_FLAG == 0x8000000) {
        PCIE_CONG_SPACE_LENGTH = _128MB;
    }
    // PCIEBAR = 0xE000_0000 in case 256MB
    __outdword(PCI_CSDR, PCIEXBA | PCIE_CONG_SPACE_LENGTH | enable);
    // MCHBA = 0xFED1_0000
    PCIEXBA[MCHBAR] = MCHBA | enable;
    Init_HPET_MMIO_and_Compare_Register();
};

void Init_HPET_MMIO_and_Compare_Register(void){
    #define Address_Enable 0x80
    PCIEXBA[P2SB + HPTC] = Address_Enable; // HPET on address FED0_0000h - FED0_03FFh
    #undef Address_Enable
    #define HPET 0xFED00000
    // Init HPET, not start
    HPET[0x108] = 0; // Compare register 0 Timer: 0x108-0x10F
    HPET[0x10C] = 0;
    #undef HPET

    __outdword(PCI_CSAR, 0x800000DC); // Special BIOS Storage
    unsigned int BIOS_STORAGE = __indword(PCI_CSDR);
    if ( HIBYTE(BIOS_STORAGE) == 0xFF ){ // If set some flag? Or check default value?
        __outbyte(RST_CNT, 6);           // Reset CPU
    }
    // JUMP to POST4
};