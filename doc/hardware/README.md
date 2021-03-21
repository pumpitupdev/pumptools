# Hardware
Documentation about hardware related stuff, e.g. cabinets, PCBs etc

## References
* Own hardware
* Hardware of other cabinet owners and group members
* [GameRepair.info](https://gamerepair.info/)
* [ArcadeOtaku](https://wiki.arcadeotaku.com/w/Pump_it_UP)

## Cabinet Types

### SD
* First generation cabinets
* Similar look to original DDR cabinets
* Monitor
    * 29" CRT
    * 4:3 aspect ratio
    * Curved surface
    * VGA 640x480 native resolution
* Shipped with [MK 1](#mk-1) and [MK 3](#mk-3) PCBs
* First game version to ship in this cabinet type: [1st](../game/01-1st.md)

### SX
* First cabinet to be official sold to other regions than South Korea
* Most notable cabinet design differences
    * Single subwoofer centered on cabinet main body
    * Two coin doors, left and right of the subwoofer
* Monitor
    * 29" CRT
    * 4:3 aspect ratio
    * Curved surface
    * VGA 640x480 native resolution

### DX
* [Cabinet manual](manual/dx/cabinet.pdf)
* Deluxe version cabinet
* Monitor
    * [Manual](manual/dx/monitor.pdf)
    * 52" rear projection
    * 4:3 aspect ratio
    * VGA 640x480 native resolution
    * Further supported resolutions: CGA, EGA
* Shipped with [MK 1](#mk-1) and [MK 3](#mk-3) PCBs

### GX
* [Cabinet manual](manual/gx/cabinet-curved-screen.pdf)
* Monitor
    * 34" CRT
    * Came in two variants
        * Curved surface initial release
        * Kortek KT-3414DF: Flat surface starting Zero
            * [Monitor schematic](manual/gx/monitor-schematic.pdf)
    * 4:3 aspect ratio   
    * VGA 640x480 native resolution
    * Further resolutions supported: CGA and EGA (might require monitor board modification) and SVGA
* First game version to ship with this type
    * Curved monitor surface: [Exceed](../game/15-exc.md)
    * Flat monitor surface: [Zero](../game/17-zero.md)

### FX
* Cabinet manual
    * [NX](manual/fx/cabinet-nx.pdf)
    * [PRO](manual/fx/cabinet-pro.pdf)
* Monitor
    * [Manual](manual/fx/monitor.pdf)
    * 42" plasma
    * WSVGA 1024x768 native resolution
    * 16:9 aspect ratio
    * Further supported resolutions: VGA, SVGA
* Shipped with [MK 6](#mk-6) and [MK 9](#mk-9) PCBs
* First game version to ship in this cabinet type: [NX](../game/18-nx.md)

### Jump
* [Cabinet manual](manual/jump/cabinet.pdf)
* Cabinet specially made for the game Pump It Up Jump!
* Shipped with [MK 9](#mk-9) PCBs

### SX2
* "Refreshed" version of the [SX](#sx) type cabinet. Though it resembles more the [GX](#gx) cabinet with a smaller monitor
* Monitor
    * [Manual](manual/sx2/monitor.pdf)
    * KT-2914DF
    * 29" CRT
    * Flat surface
    * 4:3 aspect ratio
    * VGA 640x480 native resolution
    * Further resolutions supported: CGA and EGA (might require monitor board modification) and SVGA
* First game version to ship with this type: [NX2](../game/20-nx2.md)

### CX
* [Cabinet manual](manual/cx/cabinet.pdf)
* Monitor
    * 42" LCD
    * 16:9 aspect ratio
* Shipped with [MK 9](#mk-9) PCBs
* Cabinet buttons ("PRO button board") built-in
* First game version to ship in this cabinet type: [Fiesta](../game/23-fst.md)

### TX
* [Cabinet manual](manual/tx/cabinet.pdf)
* Monitor
    * 50" LCD
    * 16:9 aspect ratio
* Shipped with [MK 9](#mk-9) PCBs
* Cabinet buttons ("PRO button board") built-in
* First game version to ship in this cabinet type: [Fiesta](../game/23-fst.md)

### LX
* [Cabinet manual](manual/lx/cabinet.pdf)
* Monitor
    * 55" LCD
    * 16:9 aspect ratio
* Shipped with [MK X](#mk-x) PCBs
* First game version to ship in this cabinet type: [Prime 2](../game/28_pri2)

## Main Processing Unit
Aka PCB, (main) CPU, PC, etc.

### MK 1
The oldest piece of hardware we have seen was a MK3 board so far. But there are several hints that some piece of
hardware pre-dating the MK3 probably existed at some point:
* The first three games, 1st, 2nd and 3rd/OBG, are storing their music as actual CD audio, not MP3 files in the data
area of the discs
* MP3 hardware decoder chips were likely not cheap at that time. Furthermore, the total audio length of all songs still
fit onto the disc as CD audio
* The first three games did not use a lock-chip as a means for copy-protection. Possible, that this piece of hardware
wasn't available from the beginning and therefore not used

#### Compatible Games
* [1st](../game/01-1st.md)
* [2nd](../game/02-2nd.md)
* [3rd OBG](../game/03-3rd.md)

Anything newer might not have worked because of the above assumption that additional required hardware was missing.

### MK 3

#### General Specifications
* Custom PCB assembly with daughter boards stacked
* DOS OS stored on ROM chip
* Games stored and loaded from CD
* PIUIO interface connected via ISA Bus
* Hardware audio decoder chip for MP3 audio
* CAT702 ZN security "Lock chip" for copy-protection

#### System Configuration using DOS PC-Doctor
System configuration dump with DOS PC-Doctor. The HDD listed in the information below is part of the system that was
used to dump this information. The DVD drive is a replacement of a probably broken CD drive.

```text
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

#### On-board chips
* Audio related
    * MAS 3507D MPEG 1/2 Layer 2/3 Audio Decoder
    * DAC 3550A Stereo Audio DAC
    * YMZ280B PCMD8 8-Channel PCM/ADPCM Decoder
    * YAC516 DAC16-L Delta Sigma Modulation D/A Converter with 8 times Over-sampling Filter
* Security
    * [CAT702 ZN security](https://github.com/svn2github/mameplus/blob/master/trunk/mamep/src/mame/machine/cat702.c)

#### Compatible Games
* [1st](../game/01-1st.md)
* [2nd](../game/02-2nd.md)
* [3rd OBG](../game/03-3rd.md)
* [OBG SE](../game/04-3se.md)
* [The Collection](../game/05-tc.md)
* [The Perfect Collection](../game/06-pc.md)
* [Extra](../game/07-extra.md)
* [Premiere 1](../game/08-prem1.md)
* [Prex 1](../game/09-prex1.md)
* [Rebirth](../game/10-reb.md)
* [Premiere 2](../game/11-prem2.md)
* [Prex 2](../game/12-prex2.md)
* [Premiere 3](../game/13-prem3.md)
* [Prex 3](../game/14-prex3.md)

### MK 5

#### General Specifications
* OS: Either Windows XP Embedded (revisions of Prem3/Prex3) or Linux (starting Exceed)
* Same IO as MK3 connected to the ISA bus
* CPU: Intel Celeron 1 GHZ
* RAM: 128 MB
* GPU: Nvidia NV11 Chipset
* CDROM based for Prem3/Prex3, HDD based for Exceed and newer

#### Compatible Games
* [Premiere 3](../game/13-prem3.md)/[Prex 3](../game/14-prex3.md) (OS: Windows XP Embedded)
* [Exceed](../game/15-exc.md) (OS: Linux)
* [Exceed 2](../game/16-exc2.md) (supposed to need a RAM upgrade to 256 MB)
* [NX](../game/18-nx.md) (supposed to need a RAM upgrade to 256 MB)

### MK 6

#### General Specifications
* Introduces USB PIUIO interface for cabinet and pad inputs/outputs
    * USB PIUIO: MPUG0PCB002
    * PIUIO "JAMMA" (no, it is actually NOT JAMMA, just the same connector) connector board: MPUG0PCB002
* GPU: NVIDIA GeForce 5200 FX, 128 MB, AGP
* 256 MB RAM, e.g. PC3100 Samsung
* HDD based games only from this point
* Games compatible with this hardware, starting Exceed 2, are now protected by USB based dongles

#### Revision 1
* Started shipping with Exceed 2
* Motherboard
    * Gigabyte GA-8IPE1000MK (rev. 1.x), Socket 478
    * Gigabyte GA-8IG1000MK Rev 2.X, LGA775
* CPU
    * For socket 478: Intel Celeron D, 2.53 GHZ/256/533
* Sound chip still compatible with Exceed 2 and Zero

#### Revision 2
* Started shipping later during the lifecycle of NX
    * (Some/most?) FX cabs with NX still came with Revision 1
* Motherboard: GA-8I865GME-775-RH Rev 1.X, LGA775
* Sometimes referred to as "MK7"
* Sound chip not compatible with Exceed 2 and Zero

#### Compatible Games
* [Exceed 2](../game/16-exc2.md) (only Revision 1)
* [Zero](../game/17-zero.md) (only Revision 1)
* [NX](../game/18-nx.md)
* [PRO](../game/19-pro.md)
* [NX2](../game/20-nx2.md)
* [NXA](../game/21-nxa.md)
* [PRO 2](../game/22-pro2.md) (no 3D arrow skin)
* [Fiesta](../game/23-fst.md) (requires RAM upgrade to at least 512 MB)
* [FiestaEX](../game/24-fex.md) (requires RAM upgrade to at least 512 MB)
* [Fiesta 2](../game/26-f2.md) (requires RAM upgrade to at least 512 MB)
* [Prime](../game/27-pri.md) (requires RAM upgrade to at least 512 MB, SD mode only)

### MK 9

#### General Specifications
* Still USB PIUIO based IO interface

#### Revision 1
* Motherboard: GIGABYTE GA-945GZM-s2
* GPU: GeForce 7200GF, 512 MB

#### Revision 2
* Motherboard: Gigabyte GA-945GCM-S2L
* GPU: GeForce 8400GS

#### Revision 3
* Motherboard: Asrock G41M-S3
* GPU: GeForce 9300GS or Nvidia GT210

#### Compatible Games
* [PRO](../game/19-pro.md)
* [NX2](../game/20-nx2.md)
* [NXA](../game/21-nxa.md)
* [PRO 2](../game/22-pro2.md)
* [Fiesta](../game/23-fst.md)
* [FiestaEX](../game/24-fex.md)
* [Fiesta 2](../game/26-f2.md)
* [Prime](../game/27-pri.md)
* [Prime 2](../game/28-pri2.md)
* [XX](../game/29-xx.md)

### MK X
TBD