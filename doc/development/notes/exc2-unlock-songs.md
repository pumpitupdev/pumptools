# Notes about song unlocks in Exceed 2
Executable of v1.02, build 7th December 2004.

A bunch of copy-pastes from IDA checking which songs can be unlocked permanently.

## Internal song list and state structure
```
00000000 SongList        struc ; (sizeof=0x48, mappedto_187) ; XREF: .data:songList/r
00000000 trackNum        dd ?
00000004 ptrArtistNameKr dd ?
00000008 ptrArtistNameEn dd ?
0000000C ptrSongNameKr   dd ?                    ; XREF: sub_805186E+7A/r
0000000C                                         ; sub_805186E+B4/r ...
00000010 ptrSongNameEn   dd ?
00000014 ptrBpm          dd ?
00000018 channelInfo     dd ?
0000001C level           dd 6 dup(?)
00000034 enableSong      db ?
00000035 hiddenSong      db ?
00000036 enableDemoPlay  db ?
00000037 censor          db ?
00000038 pad             db ?
00000039 censorLock      db ?
0000003A enable_play     db ?
0000003B hidden_play     db ?
0000003C enableMode      db 6 dup(?)             ; string(C)
00000042 enableMode_play db 6 dup(?)             ; string(C)
00000048 SongList        ends
```

## Internal song list
```
.data:080890E4 ; SongList songList
.data:080890E4 songList        dd 101h                 ; trackNum
.data:080890E4                                         ; DATA XREF: sub_8062D22+38o
.data:080890E4                                         ; GetSongRealIndex+33o ...
.data:080890E4                 dd 807BC28h             ; ptrArtistNameKr
.data:080890E4                 dd 807BC2Dh             ; ptrArtistNameEn
.data:080890E4                 dd 807BC33h             ; ptrSongNameKr
.data:080890E4                 dd 0                    ; ptrSongNameEn
.data:080890E4                 dd 0                    ; ptrBpm
.data:080890E4                 dd 40624000h            ; channelInfo
.data:080890E4                 dd 0, 0FFFFFFFFh, 6, 0Ch, 7, 0FFFFFFFFh; level
.data:080890E4                 db 1                    ; enableSong
.data:080890E4                 db 0                    ; hiddenSong
.data:080890E4                 db 1                    ; enableDemoPlay
.data:080890E4                 db 0                    ; censor
.data:080890E4                 db 0FFh                 ; pad
.data:080890E4                 db 0FFh                 ; censorLock
.data:080890E4                 db 0FFh                 ; enable_play
.data:080890E4                 db 0FFh                 ; hidden_play
.data:080890E4                 db 0,0,0,0,0,0          ; enableMode
.data:080890E4                 db 0,0,1,0,0,0          ; enableMode_play
.data:080890E4                 dd 102h                 ; trackNum
.data:080890E4                 dd 807BC28h             ; ptrArtistNameKr
.data:080890E4                 dd 807BC2Dh             ; ptrArtistNameEn
.data:080890E4                 dd 807BC44h             ; ptrSongNameKr
.data:080890E4                 dd 0                    ; ptrSongNameEn
.data:080890E4                 dd 0                    ; ptrBpm
.data:080890E4                 dd 40668000h            ; channelInfo
.data:080890E4                 dd 0, 0FFFFFFFFh, 6, 0Ch, 7, 0Fh; level
.data:080890E4                 db 1                    ; enableSong
.data:080890E4                 db 0                    ; hiddenSong
.data:080890E4                 db 1                    ; enableDemoPlay
.data:080890E4                 db 0                    ; censor
.data:080890E4                 db 0FFh                 ; pad
.data:080890E4                 db 0FFh                 ; censorLock
.data:080890E4                 db 0FFh                 ; enable_play
.data:080890E4                 db 0FFh                 ; hidden_play
.data:080890E4                 db 0,0,0,0,0,0          ; enableMode
.data:080890E4                 db 0,0,2,0,0,0          ; enableMode_play
...
```

## Settings struct first few fields
```
00000000 settings_struct struc ; (sizeof=0x162B, mappedto_186)
00000000 ff_area         db 1307 dup(?)          ; string(C)
0000051B unlocks_maybe   db 16 dup(?)            ; string(C)
0000052B blacklist       db 139 dup(?)           ; string(C)
000005B6 unkn2           db 117 dup(?)           ; string(C)
0000062B remain          db 4096 dup(?)          ; string(C)
0000162B settings_struct ends
```

## Function that checks for song unlocks
Located at 0x0806307E

```
int __usercall unlock_song@<eax>(int a1@<esi>, _DWORD *arg_0, _DWORD *songid)
{
  int v3; // eax@1
  int v4; // ecx@1
  signed int v5; // edi@3
  int v6; // eax@6
  signed int v7; // eax@16
  signed int v8; // eax@23
  signed int v9; // eax@30
  signed int v10; // eax@37
  signed int v11; // eax@42
  int v13; // [sp+0h] [bp-18h]@3
  char result_unlocked_song; // [sp+7h] [bp-11h]@1

  result_unlocked_song = 0;
  v3 = sub_8061C62(dword_80BED54, "PLAY");
  v4 = v3 - 200696;
  if ( !v3 )
    v4 = 0;
  v5 = 0;
  v13 = v4;
  do
  {
    if ( v5 )
    {
      if ( v5 != 1 )
        goto LABEL_11;
      if ( !(dword_80BECD4 & 2) )
        goto LABEL_43;
      v6 = sub_8056E76(v13, 1);
    }
    else
    {
      if ( !(dword_80BECD4 & 1) )
        goto LABEL_43;
      v6 = sub_8056E76(v13, 0);
    }
    a1 = v6;
LABEL_11:
    if ( currentStateCount > 2
      && !stationCurrentlySelected              // 0 = arcade station
      && *(&songList.trackNum + 18 * sub_8062DC6(dword_80BECE4)) == 0xB13// ill give you all my love
      && byte_80BECF5 & 2
      && *(_DWORD *)(a1 + 456) <= 10 )
    {
      v7 = GetSongRealIndex(0xB19);             // canon-d
      *arg_0 = 0;
      result_unlocked_song = 1;
      *songid = 0xB19;                          // canon-d
      songList.enableMode[72 * v7 + 4] = 0;     // +4 mode offset = nightmare
    }
    if ( currentStateCount == 1 )
    {
      if ( stationCurrentlySelected == 1        // 1 = remix station
        && *(&songList.trackNum + 18 * sub_8062DC6(dword_80BECDC)) == 0xB31// diva's remix
        && *(_DWORD *)(a1 + 400) <= 20
        && !*(_DWORD *)(a1 + 448)
        && *(float *)(a1 + 368) >= 1.0 )
      {
        v8 = GetSongRealIndex(0xB54);           // 2nd hidden remix
        *arg_0 = 3;
        result_unlocked_song = 1;
        *songid = 0xB54;                        // 2nd hidden remix
        songList.enableMode[72 * v8 + 4] = 0;   // +4 mode offset = nightmare
      }
      if ( currentStateCount == 1 )
      {
        if ( stationCurrentlySelected == 1
          && *(&songList.trackNum + 18 * sub_8062DC6(dword_80BECDC)) == 0xB30// deux remix
          && *(_DWORD *)(a1 + 400) <= 20
          && !*(_DWORD *)(a1 + 448)
          && *(float *)(a1 + 368) >= 1.0 )
        {
          v9 = GetSongRealIndex(0xB29);         // banya classic remix
          *arg_0 = 3;
          result_unlocked_song = 1;
          *songid = 0xB29;                      // banya classic remix
          songList.enableMode[72 * v9 + 4] = 0; // +4 mode offset = nightmare
        }
        if ( currentStateCount == 1 )
        {
          if ( stationCurrentlySelected == 1
            && *(&songList.trackNum + 18 * sub_8062DC6(dword_80BECDC)) == 0xB29// banya classic remix
            && *(_DWORD *)(a1 + 400) <= 20
            && !*(_DWORD *)(a1 + 448)
            && *(float *)(a1 + 368) >= 1.0 )
          {
            v10 = GetSongRealIndex(0xB51);      // dignity full remix
            *arg_0 = 3;
            result_unlocked_song = 1;
            *songid = 0xB51;                    // dignity full remix
            songList.enableMode[72 * v10 + 2] = 0;// +2 mode offset = crazy
          }
          if ( currentStateCount == 1
            && stationCurrentlySelected == 1
            && *(&songList.trackNum + 18 * sub_8062DC6(dword_80BECDC)) == 0xB28// treme vook of the war
            && byte_80BECED & 2 )
          {
            *(&songList.hiddenSong + 72 * GetSongRealIndex(0xB58)) = 0;// disable this song as a hidden song
            v11 = GetSongRealIndex(0xB58);      // treme of the vook war another
            *arg_0 = 3;
            result_unlocked_song = 1;
            *(&songList.enableSong + 72 * v11) = 1;
            *songid = 0xB58;
          }
        }
      }
    }
LABEL_43:
    ++v5;
  }
  while ( v5 <= 1 );
  if ( result_unlocked_song )
    sub_8074CA8(9, 0);
  if ( settings->unlocks_maybe[10] == 1 && stationCurrentlySelected == 1 )
  {
    *(&songList.hiddenSong + 0x48 * GetSongRealIndex(0xB57)) = 0;// disable this song as a hidden song
    *(&songList.enableSong + 0x48 * GetSongRealIndex(0xB57)) = 1;
    if ( !result_unlocked_song )
    {
      *arg_0 = 3;
      *songid = 0xB57;                          // canon-d full mix
    }
    result_unlocked_song = 1;
  }
  return (unsigned __int8)result_unlocked_song;
}
```