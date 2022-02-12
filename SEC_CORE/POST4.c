// Just Pseudocode because source is spagetti pure assembly
// All function with __ pseudofunction or compiler intrinsics

enum MMIO {
  MEMORY = 0,
  MSI_Interrupt_Memory = 0xFEE00000
};

enum MSR {
  IA32_MTRRCAP           = 0xFE,
  MSR_UNKNOWN            = 0x13A,
  MSR_UNKNOWN2           = 0x2E0,
  IA32_MTRR_DEF_TYPE     = 0x2FF,
  IA32_MTRR_FIX64K_00000 = 0x250,
  IA32_MTRR_FIX16K_80000 = 0x258,
  IA32_MTRR_FIX16K_A0000 = 0x259,
  IA32_MTRR_FIX4K_C0000  = 0x268,
  IA32_MTRR_FIX4K_C8000  = 0x269,
  IA32_MTRR_FIX4K_D0000  = 0x26A,
  IA32_MTRR_FIX4K_D8000  = 0x26B,
  IA32_MTRR_FIX4K_E0000  = 0x26C,
  IA32_MTRR_FIX4K_E8000  = 0x26D,
  IA32_MTRR_FIX4K_F0000  = 0x26E,
  IA32_MTRR_FIX4K_F8000  = 0x26F,
  IA32_MTRR_PHYSBASE0    = 0x200,
  IA32_MTRR_PHYSMASK0    = 0x201,
  IA32_MTRR_PHYSBASE1    = 0x202,
  IA32_MTRR_PHYSMASK1    = 0x203,
  IA32_MTRR_PHYSBASE2    = 0x204,
  IA32_MTRR_PHYSMASK2    = 0x205,
  IA32_MTRR_PHYSBASE3    = 0x206,
  IA32_MTRR_PHYSMASK3    = 0x207,
  IA32_MTRR_PHYSBASE4    = 0x208,
  IA32_MTRR_PHYSMASK4    = 0x209,
  IA32_MTRR_PHYSBASE5    = 0x20A,
  IA32_MTRR_PHYSMASK5    = 0x20B,
  IA32_MTRR_PHYSBASE6    = 0x20C,
  IA32_MTRR_PHYSMASK6    = 0x20D,
  IA32_MTRR_PHYSBASE7    = 0x20E,
  IA32_MTRR_PHYSMASK7    = 0x20F,
  IA32_MTRR_PHYSBASE8    = 0x210,
  IA32_MTRR_PHYSMASK8    = 0x211,
  IA32_MTRR_PHYSBASE9    = 0x212,
  IA32_MTRR_PHYSMASK9    = 0x213 
}

void Fault_CacheCorrupted();

void InitStackInCache()
{
  
  if ( MSR_UNKNOWN.bit0 == 0 )
  {
    // Some
    MSI_Interrupt_Memory[0x300] = 0xC4500;
    while ( (MSI_Interrupt_Memory[0x300] & 0x1000) != 0 ) ;

    // Find bug! Set on zero reserved bits in IA32_MTRR_DEF_TYPE
    /* -----------------------------------------------
    /* mov     ecx, MSR_IA32_MTRRCAP
    /* rdmsr
    /* movzx   ebx, al
    /* shl     ebx, 2
    /* add     ebx, 18h
    /* xor     eax, eax
    /* xor     edx, edx   
    /* InitCacheRegistrers:
    /*     add     ebx, -2
    /*     movzx   ecx, cs:ModelSpecificRegister[ebx]
    /*     wrmsr
    /*     jnz     short InitCacheRegistrers
    /* -----------------------------------------------
    /* cs:ModelSpecificRegister[0] = 0x2FF, this is IA32_MTRR_DEF_TYPE
    /* ebx on last iteration set to 0 and cycle write in reserved bits
    */
    for (int count = 2*IA32_MTRRCAP.VCNT + 12; count != 0; ) { // Mask Reg + Base Reg + Fixed + IA32_MTRR_DEF_TYPE
      *ModelSpecificRegister[--count] = 0;
    };

    IA32_MTRR_DEF_TYPE.FE   = false;
    IA32_MTRR_DEF_TYPE.E    = false;
    IA32_MTRR_DEF_TYPE.Type = UNCACHEABLE;

    // Set stack area in cache 0xFEF00000-0xFEF80000
    IA32_MTRR_PHYSBASE0.PhysBase  = 0xFEF00000;
    IA32_MTRR_PHYSBASE0.Type      = WRITEBACK;
    IA32_MTRR_PHYSMASK0.Valid     = true;
    IA32_MTRR_PHYSMASK0.PhysMask  = 0.5MB;

    // Get info about L3 Cache (my CPU not have L3 cache!)
    __GetCacheInfo();
    CacheSize = PhysicalLinePatritions * SystemCoherencyLineSize * WaysOfAssociativity * NumberOfSets;
    RemainingCache = CacheSize - 0.5MB - CachedAreaSize_0;

    // Align cache size
    if ( ( LOWORD(RemainingCache) != 0 ) {
      LOWORD(RemainingCache) = 0;
      RemainingCache += 0x10000;
    };

    MemoryMappedCacheBaseAddr = _4GB_ADDRESS_SPACE-RemainingCache;

    // Align cache addr
    if ( LOWORD(MemoryMappedCacheBaseAddr) != 0 ){
      MemoryMappedCacheBaseAddr = (MemoryMappedCacheBaseAddr + 0x10000) & 0xFFFF0000;
    }

    // Map memory all remaining cache on (0xFFFF_FFFF - RemainingCache)-0xFFFF_FFFF
    // While cache exist map its on memory sequentially
    for ( MSR_INDEX = 1, CachedAreaBase = MemoryMappedCacheBaseAddr; RemainingCache != 0; CachedAreaBase += CachedAreaSize) {

      // Get mapping size
      CachedAreaSize = __LeastSignificantBit(CachedAreaBase);
      if(CachedAreaSize > RemainingCache) {
        CachedAreaSize = __MostSignificantBit(RemainingCache);
      }

      // Set cached area
      MSR_PHYSMASK[MSR_INDEX].PhysMask = __Mask(CachedAreaSize);
      MSR_PHYSMASK[MSR_INDEX].Valid    = true;
      MSR_PHYSBASE[MSR_INDEX].PhysBase = CachedAreaBase;
      MSR_PHYSBASE[MSR_INDEX].Type     = WRITEPROTECTED;

      // Counters
      MSR_INDEX++; 
      RemainingCache -= CachedAreaSize;
      // If physical addresses are over then break
      if ( CachedAreaBase + CachedAreaSize > _4GB_ADDRESS_SPACE) break;
    }

    // Size = 4 KB, area 0x4000_0000-0x4000_1000 (1GB --- 1GB+4KB)
    MSR_PHYSMASK[MSR_INDEX].PhysMask = 4KB;
    MSR_PHYSMASK[MSR_INDEX].Valid    = true;
    MSR_PHYSBASE[MSR_INDEX].PhysBase = 0x40000000;
    MSR_PHYSBASE[MSR_INDEX].Type     = WRITECOMBINING;

    // Enable MTRR
    IA32_MTRR_DEF_TYPE.E = true;
    
    // Invalidate internal cache
    __invd();
    // Enable cache, CD and NW are set at the same time
    cr0.CD = 0;
    cr0.NW = 0;

    // Maybe flag enable cache
    MSR_UNKNOWN2.bit1 = 1;

    // 0xFFFE_0000
    mm4 = _4GB_ADDRESS_SPACE-0x20000; 

    // Init each cache string ()
    StackInCache = IA32_MTRR_PHYSBASE0.PhysBase;
    NumberCacheLines = __size(IA32_MTRR_PHYSBASE0) / 64;
    for (i = 0; i < NumberCacheLines; i++ ){
      StackInCache[i] = 0xA5A5A5A5; // Some magic number
      __mm_sfence();
    }

    // Maybe flag init cache strings
    MSR_UNKNOWN2.bit2 = 1;

    // Checks cache for correct
    NumberCacheDword = __size(StackInCache) / 4;
    for (i = 0; i<NumberCacheDword ; i++){
      StackInCache[i] = 0x5AA55AA5;
      if ( StackInCache[i] != 0x5AA55AA5 ) Fault_CacheCorrupted();
    }

    // Set stack
    esp = StackInCache + __size(StackInCache);

    sub_FFFFEE41();
    
    return ((int (__fastcall *)(_DWORD, int))_mm_cvtsi64_si32(a1))(0, v50);
  }
  else
  {
    _EAX = 0x80000008;
    __asm { cpuid }
    v48 = IA32_MTRR_PHYSMASK0;
    while ( 1 )
    {
      v49 = __readmsr(v48);
      if ( (v49 & 0x800) == 0 )
        break;
      v48 += 2;
      if ( v48 > 0x213 )
      {
        while ( 1 )
          ;
      }
    }
    return ((int (__fastcall *)(unsigned int, _DWORD))loc_FFFFF733)(v48 - 1, HIDWORD(v49));
  }
}

void sub_FFFFEE41(){

  // Init MMIO for P2SB
  PCI.P2SB_Bridge.Sideband_Register_Access_BAR = 0xFD000000;
  PCI.P2SB_Bridge.PCI_Command.Memory_Space_Enable = true;

  sub_FFFFEA1D();
}

void sub_FFFFEA1D(){
  edi = LPC_SPI_DeviceId_Analyze();

}

// Get Device ID LPC/SPI controller and translate it into some number
int LPC_SPI_DeviceId_Analyze(){

  // If not default value
  if (LPC_SPI_DeviceID_data != 3) return LPC_SPI_DeviceID_data;

  if( PCI.LPC/SPI.Device_Identificator == 0xA140   ||
      PCI.LPC/SPI.Device_Identificator == 0xA141   ||
      PCI.LPC/SPI.Device_Identificator == 0xA142   ||
      PCI.LPC/SPI.Device_Identificator == 0xA143   ||
      PCI.LPC/SPI.Device_Identificator == 0xA144   ||
      PCI.LPC/SPI.Device_Identificator == 0xA145   ||
      PCI.LPC/SPI.Device_Identificator == 0xA146   ||
      PCI.LPC/SPI.Device_Identificator == 0xA147   ||
      PCI.LPC/SPI.Device_Identificator == 0xA148   ||
      PCI.LPC/SPI.Device_Identificator == 0xA149   ||
      PCI.LPC/SPI.Device_Identificator == 0xA14A   ||
      PCI.LPC/SPI.Device_Identificator == 0xA14B   ||
      PCI.LPC/SPI.Device_Identificator == 0xA14D   ||
      PCI.LPC/SPI.Device_Identificator == 0xA14E   ||
      PCI.LPC/SPI.Device_Identificator == 0xA14F   ||
      PCI.LPC/SPI.Device_Identificator == 0xA150
      )  result = 1;
  else if(
      PCI.LPC/SPI.Device_Identificator == 0x9D40 ||
      PCI.LPC/SPI.Device_Identificator == 0x9D41 ||
      PCI.LPC/SPI.Device_Identificator == 0x9D42 ||
      PCI.LPC/SPI.Device_Identificator == 0x9D43 ||
      PCI.LPC/SPI.Device_Identificator == 0x9D46 ||
      PCI.LPC/SPI.Device_Identificator == 0x9D48 ||
      )  result = 2;
  else   result = 3;

  LPC_SPI_DeviceID_data = result;
  return result;
    
}

// Cache corrupted, unable to load BIOS
void Fault_CacheCorrupted(){
  __outbyte(POST_PORT, 208);
  // Computer frozen
  while (1); 
}

short ModelSpecificRegister[20] = {
  IA32_MTRR_DEF_TYPE,
  IA32_MTRR_FIX64K_00000,
  IA32_MTRR_FIX16K_80000,
  IA32_MTRR_FIX16K_A0000,
  IA32_MTRR_FIX4K_C0000,
  IA32_MTRR_FIX4K_C8000,
  IA32_MTRR_FIX4K_D0000,
  IA32_MTRR_FIX4K_D8000,
  IA32_MTRR_FIX4K_E0000,
  IA32_MTRR_FIX4K_E8000,
  IA32_MTRR_FIX4K_F0000,
  IA32_MTRR_FIX4K_F8000,
  IA32_MTRR_PHYSBASE0,
  IA32_MTRR_PHYSMASK0,
  IA32_MTRR_PHYSBASE1,
  IA32_MTRR_PHYSMASK1,
  IA32_MTRR_PHYSBASE2,
  IA32_MTRR_PHYSMASK2,
  IA32_MTRR_PHYSBASE3,
  IA32_MTRR_PHYSMASK3,
  IA32_MTRR_PHYSBASE4,
  IA32_MTRR_PHYSMASK4,
  IA32_MTRR_PHYSBASE5,
  IA32_MTRR_PHYSMASK5,
  IA32_MTRR_PHYSBASE6,
  IA32_MTRR_PHYSMASK6,
  IA32_MTRR_PHYSBASE7,
  IA32_MTRR_PHYSMASK7,
  IA32_MTRR_PHYSBASE8,
  IA32_MTRR_PHYSMASK8,
  IA32_MTRR_PHYSBASE9,
  IA32_MTRR_PHYSMASK9,  
}