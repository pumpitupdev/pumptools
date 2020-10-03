# Hardware used by Pump It Up Games
## Main PC/CPU hardware
### MK 1
We have not seen this piece of hardware and the chances these boards still exist and are sold somewhere is rather low.
However, there are pointers in the code of the game and the evolution of the hardware that it existed for the first
few versions of PIU:
* Songs were played back entirely from CD requiring to store them as 16-bit PCM audio. Therefore, the hardware did not
have a chip for decoding MP3.
* Absence of hardware security/lockchip.

We assume that this hardware was shipped and used with 1st, 2nd and 3rd. Starting 3rd SE, song files were stored as
encoded MP3 files on the CD requiring a MP3 decoder available with [MK3 hardware](###MK 3).

### MK 3
All games from 1st to Prex3/Premiere 3 are running on this revision of hardware. 1st to 3rd are compatible with this
hardware.

System configuration dump using PC-DOCTOR DOS 3.0
```
SYSTEM CONFIGURATION=====================================================

     Operating System - DOS 7.10 in HMA
             CPU Type - 333 MHz Intel Celeron 333
                CPUID - "GenuineIntel", Family 6, Model 6, Step 5
                      - MMX available
     Coprocessor Type - 686
   Expansion Bus Type - ISA, PCI
        ROM BIOS Date - 04/20/99 
   ROM BIOS Copyright - COPYRIGHT Award Software Inc.
       Additional ROM - C800[8kB] 
          Base Memory - 640 kB
      Expanded Memory - N/A
      Extended Memory - 64512 kB (CMOS Configuration)
           XMS Memory - 62400 kB (XMS 3.00, Driver 3.95) A20=ON 
         Serial Ports - None Installed
       Parallel Ports - None Installed
        Video Adapter - VGA: 3Dfx Interactive, Inc.
                      - Total Memory : 8192 kB
    Fixed Disk Drives - 42 MB  
   Floppy Disk Drives - 1 - 1=3.5"/1.44M  
                Mouse - No Mouse
            Joysticks - None
           Sound Card - N/A
   CAS Fax/Modem Card - N/A
     Disk Compression - N/A
CD-ROM Driver Version - MSCDEX V2.25, Drive: D:
           Disk Cache - Smartdrive 5.02, Hits 76%, Size 2048 kB
   Primary IDE Master - ATAPI: HL-DT-STDVD-ROM GDR8164B
    Primary IDE Slave - IDE: QUANTUM FIREBALL_TM1280A, S/N:692708422782
 Secondary IDE Master - No Drive
  Secondary IDE Slave - No Drive
                 SCSI - N/A
              Network - N/A
     Power Management - APM V1.2, Power On
             USB Port - Installed on IRQ 10
         Chipset Type - Intel 440BX
        L2 Cache Type - 128 kB
           DRAM Row 0 - 64 MB
           DRAM Row 1 - Empty
           DRAM Row 2 - Empty
           DRAM Row 3 - Empty
           DRAM Row 4 - Empty
           DRAM Row 5 - Empty
           DRAM Row 6 - Empty
           DRAM Row 7 - Empty


PC-DOCTOR DOS 3.0 Copyr 2004 PC-Doctor, Inc. 16:15 9/25/2000
```

A bunch of custom chips for handling the following:
* Sound
    * YAMAHA YMZ280B-F (8-Channel PCM/ADPCM Decoder)
        * 16-bit PCM sound effects are played from external memory
    * YAMAHA YAC516 (Delta Sigma Modulation D/A Converter with 8 times Over-sampling Filter)
    * MICRONAS DAC3550A C2 (Stereo Audio DAC)
    * MICRONAS MAS3507D F10 (MPEG 1/2 Layer 2/3 Audio Decoder)
        * Songs are stored MP3 encoded

### MK 5

### MK 6

### MK 7

### MK 9

### MK X

## IO hardware
### ISA PIUIO

### USB PIUIO

### USB PIUIO 2

### PIUBTN

### XX Card reader

## Security
### Lockchip

### USB dongles

### ds1963s
Used for Pro 1/2 (and ITG 1/2).