# Pad command codes notes
Raw data and notes about pad command codes which were extracted directly from the games because I could not find a 
complete and 100% accurate list per game.

All codes extracted from the MK3 games are based on the Linux port executables.

How did I find these? I used the `ac_sound_play_effect` with the second parameter set to the value `3` as my entry
point. This plays the sound effect that you can hear when you entered the modifier code correctly. After I have found
the right call, it's next to some code like this (taken from 03_3rd):
```
...
  if ( modFlags & 1 )
  {
    if ( modFlagsSpeed1P & 1 )
      result = func_2a2dc(0, 1);
    if ( modFlagsSpeed1P & 2 )
      result = func_2a2dc(0, 2);
    if ( modFlagsSpeed1P & 4 )
      result = func_2a2dc(0, 4);
    if ( modFlagsSpeed1P & 8 )
      result = func_2a2dc(0, 8);
    if ( modFlagsSpeed1P & 0x10 )
      result = func_2a2dc(0, 16);
  }
  if ( modFlags & 2 )
  {
    if ( modFlagsSpeed2P & 1 )
      result = func_2a2dc(1, 1);
    if ( modFlagsSpeed2P & 2 )
      result = func_2a2dc(1, 2);
    if ( modFlagsSpeed2P & 4 )
      result = func_2a2dc(1, 4);
    if ( modFlagsSpeed2P & 8 )
      result = func_2a2dc(1, 8);
    if ( modFlagsSpeed2P & 0x10 )
      result = func_2a2dc(1, 16);
  }
  if ( modFlagsSpeed1P & 0x1F )
  {
    checkIfModifierSet2();
    result = ac_sound_play_effect(0, 3u, 2u);
  }
  if ( modFlagsSpeed2P & 0x1F )
  {
    checkIfModifierSet();
    result = ac_sound_play_effect(0, 3u, 2u);
  }
  return result;
}
```

I already renamed the function that is called prior the `ac_sound_play_effect` to `checkIfModifierSet` here. That
function checks the last eight pad inputs from a list against a list of command codes. Once a code matches, flags are
set accordingly.

Excerpt, again from 03_3rd:
```
signed int checkIfModifierSet()
{
  const char *v0; // edx@1
  int v1; // ebp@1
  signed int v2; // ebx@1
  const char *v3; // edx@19
  unsigned int v4; // ebx@19
  signed int result; // eax@41
  signed int v6; // [sp+0h] [bp-1Ch]@1

  v0 = (const char *)&codeList; // THIS is the code list
  v1 = *(_DWORD *)&modFlags;
  v2 = 0;
  v6 = *(_DWORD *)&data_3299a4;
  while ( memcmp((const char *)&data_44e9e8, v0, 8) )// iterate command list and currently post 8 inputs
  {
    ++v2;
    v0 += 8;                                    // each modifier code is 8 commands
    if ( v2 >= 13 )                             // beyond command list size
      goto jump_2a17f;
  }
  switch ( v2 )                                 // found command id
  {
    case 0:
      *(_DWORD *)&data_3299a4 = 0;
      *(_DWORD *)&modFlags = *(_WORD *)&modFlags & 0xFFF;
      break;
```

## 03_3rd
```
1 = DL
2 = DR
4 = C
8 = UL
16 = UR

-- cancel
.data:0810833A                  db    4  
.data:0810833B                 db    4
.data:0810833C                 db    4
.data:0810833D                 db    4

.data:0810833E                 db    4
.data:0810833F                 db    4
.data:08108340                 db    4
.data:08108341                 db    4
--------------------------- vanish
.data:08108342                 db    4 center
.data:08108343                 db    4 center
.data:08108344                 db  10h up right
.data:08108345                 db    8 up left
.data:08108346                 db    4 center
.data:08108347                 db    2 down right
.data:08108348                 db    1 down left
.data:08108349                 db    4 center
----------------- mirror
.data:0810834A                 db    4
.data:0810834B                 db    4
.data:0810834C                 db    1
.data:0810834D                 db    4
.data:0810834E                 db  10h
.data:0810834F                 db    2
.data:08108350                 db    4
.data:08108351                 db    8
---- 8X
.data:08108352                 db    4
.data:08108353                 db    4
.data:08108354                 db    1
.data:08108355                 db    4
.data:08108356                 db    2
.data:08108357                 db    4
.data:08108358                 db    4
.data:08108359                 db    4
- 4 X
.data:0810835A                 db    4
.data:0810835B                 db    4
.data:0810835C                 db    2
.data:0810835D                 db    4
.data:0810835E                 db    4
.data:0810835F                 db    8
.data:08108360                 db    4
.data:08108361                 db    4
2X
.data:08108362                 db    4
.data:08108363                 db    4
.data:08108364                 db    8
.data:08108365                 db    4
.data:08108366                 db    4
.data:08108367                 db    4
.data:08108368                 db  10h
.data:08108369                 db    4
random step
.data:0810836A                 db    4
.data:0810836B                 db    1
.data:0810836C                 db    4
.data:0810836D                 db    2
.data:0810836E                 db    4
.data:0810836F                 db  10h
.data:08108370                 db    4
.data:08108371                 db    8
nonstep
.data:08108372                 db    8
.data:08108373                 db  10h
.data:08108374                 db    4
.data:08108375                 db    8
.data:08108376                 db  10h
.data:08108377                 db    4
.data:08108378                 db    8
.data:08108379                 db  10h
synchro
.data:0810837A                 db    4
.data:0810837B                 db    4
.data:0810837C                 db    8
.data:0810837D                 db  10h
.data:0810837E                 db    8
.data:0810837F                 db  10h
.data:08108380                 db    4
.data:08108381                 db    4
couple
.data:08108382                 db    4
.data:08108383                 db    4
.data:08108384                 db  10h
.data:08108385                 db    8
.data:08108386                 db  10h
.data:08108387                 db    8
.data:08108388                 db    4
.data:08108389                 db    4
union
.data:0810838A                 db    1
.data:0810838B                 db    2
.data:0810838C                 db    4
.data:0810838D                 db    8
.data:0810838E                 db    2
.data:0810838F                 db    1
.data:08108390                 db    4
.data:08108391                 db  10h
random vanish
.data:08108392                 db    8
.data:08108393                 db  10h
.data:08108394                 db    4
.data:08108395                 db    1
.data:08108396                 db    2
.data:08108397                 db    4
.data:08108398                 db    8
.data:08108399                 db  10h
random speed
.data:0810839A                 db    8
.data:0810839B                 db  10h
.data:0810839C                 db    2
.data:0810839D                 db    4
.data:0810839E                 db    1
.data:0810839F                 db    4
.data:081083A0                 db    2
.data:081083A1                 db    1

```

Looking further into the code, there seems to be another command list with three entries right after that and it's
checked in the same function. This time, the number of inputs are 10 instead of 8:
```
  v3 = (const char *)&data_108d84; // THIS is the code list
  v4 = 0;
  while ( memcmp(data_44e9f8, v3, 10) )
  {
    ++v4;
    v3 += 10;
    if ( (signed int)v4 >= 3 )
      goto jump_2a289;
  }
  if ( v4 < 1 )
  {
    if ( !v4 && data_44ea30 == 2 )
      data_108bad |= 0x80u;
  }
  else if ( v4 <= 1 )
  {
    if ( data_108bad & 4 )
    {
      *(_WORD *)&modFlags |= 0x820u;
      qmemcpy(data_44e488, &data_388e00[20 * (4 * data_108ba8 + 52)], 0x50u);
    }
  }
  else if ( v4 == 2
         && (data_108bad & 2 && data_108ba8 >= 2 || data_108bad & 4 && !(modFlags & 0x20) && data_108ba8 == 1) )
  {
    data_108bae |= 1u;
  }
```

Code list:
```
1 = DL
2 = DR
4 = C
8 = UL
16 = UR

.data:081083A2 data_108d84     db    2                 ; DATA XREF: func_29ce8:jump_29e70o
.data:081083A2                                         ; checkIfModifierSet:jump_2a17fo
.data:081083A3                 db    1
.data:081083A4                 db    8
.data:081083A5                 db  10h
.data:081083A6                 db  18h
.data:081083A7                 db  20h
.data:081083A8                 db  12h
.data:081083A9                 db  11h
.data:081083AA                 db  10h
.data:081083AB                 db  18h

.data:081083AC                 db  12h
.data:081083AD                 db  18h
.data:081083AE                 db  10h
.data:081083AF                 db    1
.data:081083B0                 db    8
.data:081083B1                 db    2
.data:081083B2                 db  11h
.data:081083B3                 db  20h
.data:081083B4                 db  11h
.data:081083B5                 db  10h

.data:081083B6                 db    8
.data:081083B7                 db    2
.data:081083B8                 db  18h
.data:081083B9                 db  12h
.data:081083BA                 db    1
.data:081083BB                 db  10h
.data:081083BC                 db  11h
.data:081083BD                 db  20h
.data:081083BE                 db    2
.data:081083BF                 db  18h
```

However, the code list seems to have inputs with multiple bit fields set, e.g. 0x12, 0x11. Untested.

## 04_3se
List of previous version extended by 3 more codes (last ones in the list).

```
1 = DL
2 = DR
4 = C
8 = UL
16 = UR

data_12ae8c     db    4                 ; DATA XREF: func_2a8c0+23o
.data:08123643                                         ; func_2ab98+23o
.data:08123644                 db    4
.data:08123645                 db    4
.data:08123646                 db    4
.data:08123647                 db    4
.data:08123648                 db    4
.data:08123649                 db    4
.data:0812364A                 db    4

.data:0812364B                 db    4
.data:0812364C                 db    4
.data:0812364D                 db  10h
.data:0812364E                 db    8
.data:0812364F                 db    4
.data:08123650                 db    2
.data:08123651                 db    1
.data:08123652                 db    4

.data:08123653                 db    4
.data:08123654                 db    4
.data:08123655                 db    1
.data:08123656                 db    4
.data:08123657                 db  10h
.data:08123658                 db    2
.data:08123659                 db    4
.data:0812365A                 db    8

.data:0812365B                 db    4
.data:0812365C                 db    4
.data:0812365D                 db    1
.data:0812365E                 db    4
.data:0812365F                 db    2
.data:08123660                 db    4
.data:08123661                 db    4
.data:08123662                 db    4

.data:08123663                 db    4
.data:08123664                 db    4
.data:08123665                 db    2
.data:08123666                 db    4
.data:08123667                 db    4
.data:08123668                 db    8
.data:08123669                 db    4
.data:0812366A                 db    4

.data:0812366B                 db    4
.data:0812366C                 db    4
.data:0812366D                 db    8
.data:0812366E                 db    4
.data:0812366F                 db    4
.data:08123670                 db    4
.data:08123671                 db  10h
.data:08123672                 db    4

.data:08123673                 db    4
.data:08123674                 db    1
.data:08123675                 db    4
.data:08123676                 db    2
.data:08123677                 db    4
.data:08123678                 db  10h
.data:08123679                 db    4
.data:0812367A                 db    8

.data:0812367B                 db    8
.data:0812367C                 db  10h
.data:0812367D                 db    4
.data:0812367E                 db    8
.data:0812367F                 db  10h
.data:08123680                 db    4
.data:08123681                 db    8
.data:08123682                 db  10h

.data:08123683                 db    4
.data:08123684                 db    4
.data:08123685                 db    8
.data:08123686                 db  10h
.data:08123687                 db    8
.data:08123688                 db  10h
.data:08123689                 db    4
.data:0812368A                 db    4

.data:0812368B                 db    4
.data:0812368C                 db    4
.data:0812368D                 db  10h
.data:0812368E                 db    8
.data:0812368F                 db  10h
.data:08123690                 db    8
.data:08123691                 db    4
.data:08123692                 db    4

.data:08123693                 db    1
.data:08123694                 db    2
.data:08123695                 db    4
.data:08123696                 db    8
.data:08123697                 db    2
.data:08123698                 db    1
.data:08123699                 db    4
.data:0812369A                 db  10h

.data:0812369B                 db    8
.data:0812369C                 db  10h
.data:0812369D                 db    4
.data:0812369E                 db    1
.data:0812369F                 db    2
.data:081236A0                 db    4
.data:081236A1                 db    8
.data:081236A2                 db  10h

.data:081236A3                 db    8
.data:081236A4                 db  10h
.data:081236A5                 db    2
.data:081236A6                 db    4
.data:081236A7                 db    1
.data:081236A8                 db    4
.data:081236A9                 db    2
.data:081236AA                 db    1
--- remix double
.data:081236AB                 db    1
.data:081236AC                 db    8
.data:081236AD                 db    1
.data:081236AE                 db    4
.data:081236AF                 db  10h
.data:081236B0                 db    2
.data:081236B1                 db  10h
.data:081236B2                 db    4
---- crazy mode
.data:081236B3                 db    4
.data:081236B4                 db    1
.data:081236B5                 db    4
.data:081236B6                 db    2
.data:081236B7                 db    4
.data:081236B8                 db    1
.data:081236B9                 db    4
.data:081236BA                 db    2
--- all select code? doesn't make sense and doesn't seem to work, probably unused
.data:081236BB                 db  10h
.data:081236BC                 db    4
.data:081236BD                 db    1
.data:081236BE                 db  10h
.data:081236BF                 db    8
.data:081236C0                 db    4
.data:081236C1 data_12af0a     db    2                 ; DATA XREF: func_2c380+139r
.data:081236C1                                         ; func_2c51c+559r
.data:081236C2                 db    8
```