import sys

Address = int(sys.argv[1], 16)

Bus      =  Address // 0x100000
Device   = (Address % 0x100000) // 0x8000
Function = (Address % 0x8000) // 0x1000
Offset   =  Address % 0x1000

print("Bus:"       + str(Bus)           + \
      "|Device:"   + str(Device)        + \
      "|Function:" + str(Function)      + \
      "|Offset:"   + str(hex(Offset))     \
      )