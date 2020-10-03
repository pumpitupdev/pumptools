# Extracting song list from game
## NX2 and NXA
The song list is compiled into the game's executable as a big table with the following structure:
```c
struct SongList_t 
{
	int IndexNum;					// Index in the song list array
	int TrackNum;					// Song ID
	int szArtistName_kr;            // const char* to the null terminated string
	int szArtistName_en;            // const char* to the null terminated string
	int szSongName_kr;              // const char* to the null terminated string
	int szSongName_en;              // const char* to the null terminated string
	int BPM;
	int ChannelInfo;
	int Level[6];					// Array indices correspond to the following and the content is the difficulty level for that mode
									// See GetRandomTrack()
									// 0 - eStepNormal
									// 1 - eStepHard
									// 2 - eStepCrazy
									// 3 - eStepHalfDouble
									// 4 - eStepFreestyle
									// 5 - eStepNightmare
	bool bEnable_m;
	bool bHidden_m;
	bool bEnableDemoPlay;			// Allow playing in attract mode
	bool bCensor;					// Don't show song in game (configurable)
	bool bPad;
	bool bCensorLock;				// Permanently censored (not configurable)
	bool bEnable_play;
	bool bHidden_play;
	int TexID;
	bool bEnableMode_m[6];			// Modes where this song is allowed
	bool bEnableMode_play[6];
	short unknown1;
	short time;						// Time value used to calculate kcal and VO2
	short SongMileage;				// Cost to unlock this song
	short ModeMileage[3];			// Mileage required to play this song on the different modes
} __attribute__((packed));
```

Probably the easiest method to locate that table is to find a string that any of the entries points to using your
favorite decompiler. Song names are good candidates as most of them should be unique and easy to locate when your tool
can expose all strings of the binary. Follow any references to the string and you should easily find a chunk of that
that looks like this:
```text 
.data:08130FDF                 db    0
.data:08130FE0 ; int g_songListWithouBlazeEmotion[]
.data:08130FE0 g_songListWithouBlazeEmotion dd 0F02h   ; DATA XREF: HandleSongUnlocksMaybe:loc_806A5B5r
.data:08130FE0                                         ; HandleSongUnlocksMaybe+26Br ...
.data:08130FE4                 dd offset aACn          ; "8¥¦ 8ûì "
.data:08130FE8                 dd offset aYahpp        ; "YAHPP "
.data:08130FEC                 dd offset aRieX_1       ; "8¦Édà+ X.1 "
.data:08130FF0                 dd offset aCannonX_1    ; "Cannon X.1 "
.data:08130FF4                 dd offset a185          ; "185 "
.data:08130FF8                 dd 0
.data:08130FFC                 dd 3
.data:08131000                 dd 0Ah
.data:08131004                 dd 14h
.data:08131008                 dd 0Dh
.data:0813100C                 dd 16h
.data:08131010                 db    1
.data:08131011                 db    1
.data:08131012                 db    0
.data:08131013                 db    1
.data:08131014                 db    0
.data:08131015                 db    1
.data:08131016                 db    0
.data:08131017                 db    0
.data:08131018                 dd 0FFFFFFFFh
.data:0813101C                 db    1
.data:0813101D                 db    1
.data:0813101E                 db    1
.data:0813101F                 db    1
.data:08131020                 db    1
.data:08131021                 db    0
.data:08131022                 db    0
.data:08131023                 db    0
.data:08131024                 db    0
.data:08131025                 db    0
.data:08131026                 db    0
.data:08131027                 db    0
.data:08131028                 dw 79h
.data:0813102A                 dw 0
.data:0813102C                 dw 0
.data:0813102E                 dw 0
.data:08131030                 dw 0
.data:08131032                 dw 0
.data:08131034                 dw 0
.data:08131036                 dw 0
.data:08131038                 dw 2
.data:0813103A                 dw 0
.data:0813103C                 dd 0F03h
```

Find the beginning by going backwards and checking where the first valid entry with pointers to artist, title etc
start. Usually, the first entry is also the primary reference for the game to address the song list.

This should allow you to find the function `GetSongRealIndex`. From there, you can easily find out the total length
of the song list.
```c
int __cdecl GetSongRealIndex(int songNo)
{
  signed int v1; // ebx@1
  int v2; // ecx@2
  int v3; // edx@3
  int v4; // eax@4

  v1 = GetTotalTrackNum();                      // 324
  if ( v1 <= 0 )
    return -1;
  v2 = 0;
  if ( g_songIdBlazingEmotion[0] != songNo )
  {
    v2 = 0;
    v3 = 0;
    while ( 1 )
    {
      ++v2;
      if ( v2 == v1 )
        break;
      v4 = g_songListWithouBlazeEmotion[v3];
      v3 += 23;
      if ( v4 == songNo )
        return *(&g_songList[0].TrackNum + 23 * v2);
    }
    return -1;
  }
  return *(&g_songList[0].TrackNum + 23 * v2);
}
```

With that info, you can dump the list straight from the binary, e.g. using `dd`:
```shell script
dd if=./piu bs=1 skip=954240 count=31200 of=songlist.bin
```

Requires you to manually calculate the size by using the above struct as reference.

Note: This part does not cover the dumping of the referenced strings to build a full song list. This process was mainly
used to get a song index mapping table for mapping absolute indices of the song table array to the song IDs and vice
versa (for pumpnet).