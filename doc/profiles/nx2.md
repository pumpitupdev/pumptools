# nx2save.bin

## Generalized layout

| File offset (hex) |   Size | Region                                                                     |
| ----------------- | -----: | -------------------------------------------------------------------------- |
| `0000-000B`       |     12 | Review player ID, ASCII                                                    |
| `000C-000F`       |      4 | Review mileage, signed little-endian `int32`                               |
| `0010-0013`       |      4 | Reward count                                                               |
| `0014-0017`       |      4 | WorldMax count                                                             |
| `0018-001B`       |      4 | Play count                                                                 |
| `001C-009B`       |    128 | Current WorldMax land, ASCII                                               |
| `009C-011B`       |    128 | Current WorldMax mission, ASCII                                            |
| `011C-011F`       |      4 | Calories (`kcal`)                                                          |
| `0120-0123`       |      4 | VO2                                                                        |
| `0124-0127`       |      4 | Adler-32 checksum                                                          |
| `0128-0167`       |     64 | USB serial, ASCII                                                          |
| `0168-016F`       |      8 | Timestamp: year `int16`, month/day/hour/min `uint8`, milliseconds `uint16` |
| `0170-0173`       |      4 | Avatar ID                                                                  |
| `0174-0177`       |      4 | Rank                                                                       |
| `0178-017B`       |      4 | Country ID                                                                 |
| `017C-0187`       |     12 | Stats player ID, ASCII                                                     |
| `0188-018B`       |      4 | Mileage                                                                    |
| `018C-018F`       |      4 | Play count                                                                 |
| `0190-0193`       |      4 | Calories                                                                   |
| `0194-0197`       |      4 | VO2                                                                        |
| `0198-019B`       |      4 | WorldMax map position                                                      |
| `019C-019F`       |      4 | Reward count                                                               |
| `01A0-01A3`       |      4 | WorldMax count                                                             |
| `01A4-02A3`       |    256 | Song unlock flags, one byte per song                                       |
| `02A4-06A3`       |  1,024 | WorldMax mission unlock flags, one byte per mission                        |
| `06A4-16A3`       |  4,096 | WorldMax high scores, 1,024 signed `int32`s                                |
| `16A4-26A3`       |  4,096 | WorldMax challenge values, 1,024 signed `int32`s                           |
| `26A4-26B3`       |     16 | WorldMax barricade flags                                                   |
| `26B4-2723`       |    128 | WorldMax event flags                                                       |
| `2724-272B`       |      8 | WorldMax warp flags                                                        |
| `272C-27AB`       |    128 | Current WorldMax land, ASCII                                               |
| `27AC-282B`       |    128 | Current WorldMax mission, ASCII                                            |
| `282C-783B`       | 20,480 | Song score table: `256 songs × 5 modes × 16 bytes`                         |

For `nx2save.bin`, the checksum covers bytes `0128-783B` after decryption.
The checksum field itself (`0124-0127`) and the review region (`0000-0123`)
are excluded.

For each song score entry:

```text
offset = 0x282C + 16 * (song_index * 5 + mode_index)

+0x00: score       int32 little-endian
+0x04: player_id   12-byte ASCII string
```

## Score entries offsets

### Arcade Station

| Artist                 | Song                         | NOTE            | Offset | Offset # |
| ---------------------- | ---------------------------- | --------------- | ------ | -------: |
| Banya Production       | Money                        |                 | ``     |        0 |
| Banya Production       | Guitar Man                   |                 | ``     |        1 |
| Banya Production       | Jam O Beat                   |                 | ``     |        2 |
| Banya Production       | Beat The Ghost               |                 | ``     |        3 |
| Banya Production       | Monkey Fingers 2             |                 | ``     |        4 |
| Banya Production       | Caprice of Otada             |                 | ``     |        5 |
| Banya Production       | Higgledy Piggledy            |                 | ``     |        6 |
| Sonic Dimension        | Groovin' Motion              |                 | ``     |        7 |
| OscillatorX            | Dance All Night              |                 | ``     |        8 |
| Elpis                  | Dance Vibrations             |                 | ``     |        9 |
| Sonic Dimension        | Chopstix                     |                 | ``     |       10 |
| Zig Zag                | Energizer                    |                 | ``     |       11 |
| YAHPP                  | Faster Z                     |                 | ``     |       12 |
| YAHPP                  | Solitary 1.5                 |                 | ``     |       13 |
| YAHPP                  | Pumptris Quattro             |                 | ``     |       14 |
| Wonder Girls           | Tell Me                      | KOREA EXCLUSIVE | ``     |       15 |
| Taiji Boys             | It's My Business             |                 | ``     |       16 |
| Taiji Boys             | Come Back Home               |                 | ``     |       17 |
| Taiji Seo              | Feel The Soul                |                 | ``     |       18 |
| Joanne                 | Shiny Day                    |                 | ``     |       19 |
| Gilgun                 | A. U Ready?                  |                 | ``     |       20 |
| Mina                   | Get My Phone Call            |                 | ``     |       21 |
| 015B                   | Very Old Couples             |                 | ``     |       22 |
| Bada                   | V. I. P.                     |                 | ``     |       23 |
| Baechigi               | Nice to Meet You             |                 | ``     |       24 |
| No Brain               | You Fall in Me               |                 | ``     |       25 |
| Crying Nut             | Astral Song                  |                 | ``     |       26 |
| May                    | Compunction                  |                 | ``     |       27 |
| Pia                    | A Maelstorm                  |                 | ``     |       28 |
| HEaDTriP               | Beat # No.4                  |                 | ``     |       29 |
| Hot Potato             | No Despair                   |                 | ``     |       30 |
| Dynamic duo            | Go Back                      |                 | ``     |       31 |
| Epik High              | Fly                          |                 | ``     |       32 |
| DJ DOC                 | One Night                    |                 | ``     |       33 |
| JinYoungHyun VS Yahpp  | U Inside My Dim Memory       |                 | ``     |       34 |
| Jiny                   | Free!                        |                 | ``     |       35 |
| Jongkook Kim           | Lovely                       |                 | ``     |       36 |
| Urburbu Project        | Terminal Depository          |                 | ``     |       37 |
| N.E.X.T                | For You                      |                 | ``     |       38 |
| Apple Jam              | Snow Dream                   |                 | ``     |       39 |
| May                    | Handsome Character That Pass |                 | ``     |       40 |
| Jinyoung Hyun          | Hyunjinyoung Go jinyoung Go  |                 | ``     |       41 |
| Hyanglim Park VS YAHPP | My Brother Is Street Singer  |                 | ``     |       42 |
| Typhoon                | So                           |                 | ``     |       43 |
| Cho PD & B.E.G         | Hold The Line                |                 | ``     |       44 |
| Drunken Tiger          | Emergency!                   |                 | ``     |       45 |
| Cho PD                 | My Friend                    |                 | ``     |       46 |
| Eugene                 | Wuthering Heights            |                 | ``     |       47 |
| Lazy Bone              | Do It Yourself               |                 | ``     |       48 |
| Turtles                | What's Going On?             |                 | ``     |       49 |
| Louis                  | Chung Hwa Ban Jeom           |                 | ``     |       50 |
| Spooky Banana          | Mr. Fire Fighter             |                 | ``     |       51 |
| T.O                    | Footprints                   |                 | ``     |       52 |
| Yoonjung Jang          | Oh My!                       |                 | ``     |       53 |
| JLT                    | Enter The Dragon             |                 | ``     |       54 |
| Perry                  | Storm                        |                 | ``     |       55 |
| Mina                   | Turn Around                  |                 | ``     |       56 |
| LEXY                   | Greenhorn                    |                 | ``     |       57 |
| 1Tym                   | HOT                          |                 | ``     |       58 |
| SOM2                   | Pray                         |                 | ``     |       59 |
| Jiwon Eun              | Dead Drunkenness in Melody   |                 | ``     |       60 |
| SOM2                   | Deja Vu                      |                 | ``     |       61 |
| D.O                    | Harangue                     |                 | ``     |       62 |
| D.O                    | Typhoon                      |                 | ``     |       63 |
| Wax                    | I'll Give You All My Love    |                 | ``     |       64 |
| Crash                  | Dignity                      |                 | ``     |       65 |
| Crash                  | What Do You Really Want?     |                 | ``     |       66 |
| U;NEE                  | Go                           |                 | ``     |       67 |
| Crying Nut             | Circus Magic                 |                 | ``     |       68 |
| DUKE                   | Starian                      |                 | ``     |       69 |
| U-Too                  | Mistake                      |                 | ``     |       70 |
| N.E.X.T                | Lazenca Save Us              |                 | ``     |       71 |
| DEUX                   | Come Back To Me              |                 | ``     |       72 |
| Novasonic              | Slam                         |                 | ``     |       73 |
| Tashannie              | Don't Bother Me              |                 | ``     |       74 |
| DEUX                   | We Are                       |                 | ``     |       75 |
| Sechs Kies             | Com'Back                     |                 | ``     |       76 |
| Fin.K.L                | Forever Love                 |                 | ``     |       77 |
| CLON                   | Funky Tonight                |                 | ``     |       78 |
| Novasonic              | Another Truth                |                 | ``     |       79 |
| DJ Dookie              | Hybs                         |                 | ``     |       80 |
| Andrew Kim             | Throw'em                     |                 | ``     |       81 |
| Sam-I-Am               | Haley                        |                 | ``     |       82 |
| Gyfted                 | We Goin' Fly Remix           |                 | ``     |       83 |
| Manresa                | Le Code De Bonne Conduite    |                 | ``     |       84 |
| Kaoma                  | Essa Maneria                 |                 | ``     |       85 |
| Los ninos de sara      | La Cubanita                  |                 | ``     |       86 |
| ROD                    | Shake It Up                  |                 | ``     |       87 |
| Victoria               | Power of Dream               |                 | ``     |       88 |
| Victoria               | Watch Out                    |                 | ``     |       89 |
| General Grant          | Soca Make Yuh Ram Ram        |                 | ``     |       90 |
| BanYa                  | Final Audition               |                 | ``     |       91 |
| BanYa                  | Extravaganza                 |                 | ``     |       92 |
| BanYa                  | Final Audition 2             |                 | ``     |       93 |
| BanYa                  | Naissance                    |                 | ``     |       94 |
| BanYa                  | Turkey March                 |                 | ``     |       95 |
| BanYa                  | She Likes Pizza              |                 | ``     |       96 |
| BanYa                  | Solitary                     |                 | ``     |       97 |
| BanYa                  | Mr. Larpus                   |                 | ``     |       98 |
| BanYa                  | N                            |                 | ``     |       99 |
| BanYa                  | Beethoven Virus              |                 | ``     |      100 |
| BanYa                  | Dr. M                        |                 | ``     |      101 |
| BanYa                  | Love is a Danger Zone        |                 | ``     |      102 |
| BanYa                  | Point Break                  |                 | ``     |      103 |
| BanYa                  | Street Show Down             |                 | ``     |      104 |
| BanYa                  | Winter                       |                 | ``     |      105 |
| BanYa                  | Will-O-The-Wisp              |                 | ``     |      106 |
| BanYa                  | Miss S' Story                |                 | ``     |      107 |
| BanYa                  | Vook                         |                 | ``     |      108 |
| BanYa                  | Bee                          |                 | ``     |      109 |
| BanYa                  | Beat of The War              |                 | ``     |      110 |
| BanYa                  | Come To Me                   |                 | ``     |      111 |
| BanYa                  | Final Audition Ep. 1         |                 | ``     |      112 |
| BanYa                  | Final Audition 3             |                 | ``     |      113 |
| BanYa                  | Naissance 2                  |                 | ``     |      114 |
| BanYa                  | Monkey Fingers               |                 | ``     |      115 |
| BanYa                  | X Treme                      |                 | ``     |      116 |
| BanYa                  | Get Up!                      |                 | ``     |      117 |
| BanYa                  | J Bong                       |                 | ``     |      118 |
| BanYa                  | Hi Bi                        |                 | ``     |      119 |
| BanYa                  | Solitary 2                   |                 | ``     |      120 |
| BanYa                  | Cannon D                     |                 | ``     |      121 |
| BanYa                  | Beat of The War 2            |                 | ``     |      122 |
| BanYa                  | Moonlight                    |                 | ``     |      123 |
| BanYa                  | Witch Doctor                 |                 | ``     |      124 |
| BanYa                  | Love is a Danger Zone pt. 2  |                 | ``     |      125 |
| BanYa                  | Phantom                      |                 | ``     |      126 |
| YAHPP                  | Witch Doctor #1              |                 | ``     |      127 |
| YAHPP                  | Arch of Dakrness             |                 | ``     |      128 |
| YAHPP                  | Chimera                      |                 | ``     |      129 |
| Banya Production       | Do You Know That-Old School  |                 | ``     |      130 |
| Banya Production       | Bullfighter's Song           |                 | ``     |      131 |
| YAHPP                  | Final Audition Ep. 2-1       |                 | ``     |      132 |
| YAHPP                  | Final Audition Ep. 2-2       |                 | ``     |      133 |

### Special Zone

#### Full Songs

| Artist                | Song                    | NOTE    | Offset | Offset # |
| --------------------- | ----------------------- | ------- | ------ | -------: |
| Seotaji and Boys      | It's My Business        | UNLOCK  | ``     |      134 |
| 015B                  | Very Old Couples        |         | ``     |      135 |
| Taiji Boys            | Come Back Home          |         | ``     |      136 |
| Seo Taji              | Feel The Soul           | UNLOCK  | ``     |      137 |
| Bada                  | V. I. P.                |         | ``     |      138 |
| Epik High             | Fly                     | UNLOCK  | ``     |      139 |
|                       |                         | UNKNOWN | ``     |      140 |
| JinYoungHyun VS Yahpp | U Inside My Dim Memory  |         | ``     |      141 |
| YAHPP                 | Fire                    |         | ``     |      142 |
| Jongkook Kim          | Lovely                  |         | ``     |      143 |
| BanYa                 | Love is a Danger Zone 2 |         | ``     |      144 |
| YAHPP                 | Beat of The War 2       |         | ``     |      145 |
| Cho PD                | Hold The Line           |         | ``     |      146 |
| YAHPP                 | Canon D Fullmix         |         | ``     |      147 |
| Crash                 | Dignity Fullmix         |         | ``     |      148 |


#### Remix

| Artist                             | Song                                 | NOTE   | Offset | Offset # |
| ---------------------------------- | ------------------------------------ | ------ | ------ | -------: |
| Banya Production                   | Banya-P Guitar Remix                 |        | ``     |      149 |
| Banya Production                   | GuitarMan Fullver.                   |        | ``     |      150 |
| Banya Production                   | Money Fingers                        | UNLOCK | ``     |      151 |
| Banya Production                   | Monkey Fingers Fullver.              | UNLOCK | ``     |      152 |
| U;NEE/Duke/D.O                     | NX2 K-Pop Remix 1                    | UNLOCK | ``     |      153 |
| Jiwon Eun/Dynamic Duo              | NX2 K-Hiphop Remix                   | UNLOCK | ``     |      154 |
| Crying Nut                         | Astral Song Fullver.                 |        | ``     |      155 |
| Mina/Baechigi                      | NX2 K-Pop Remix 2                    |        | ``     |      156 |
| Hot Potato                         | No Despair Fullver.                  |        | ``     |      157 |
| D.O/Som2                           | NX2 K-Pop Remix 3                    | UNLOCK | ``     |      158 |
| Joaane/Bada                        | NX2 Diva Remix                       | UNLOCK | ``     |      159 |
| DJ-DOC/Epik High                   | NX K-Pop Dance Remix                 |        | ``     |      160 |
| Tashannie/CLON                     | Groove Party                         |        | ``     |      161 |
| Kisteen/Scoop                      | Pop House Remix                      |        | ``     |      162 |
| YAHPP                              | WI-EX-DOC-VA                         |        | ``     |      163 |
| YAHPP                              | Bemera                               |        | ``     |      164 |
| Banya Production                   | Banya-P Classic Remix                |        | ``     |      165 |
| BanYa                              | Love is a Danger Zone 2 Try to B.P.M |        | ``     |      166 |
| Drunken Tiger/JTL/Perry/Eun Ji Won | ZERO K-Hiphop Remix                  |        | ``     |      167 |
| Novasonic/Crash                    | NOVARASH Remix                       |        | ``     |      168 |
| BanYa                              | Tream Vook of The War                |        | ``     |      169 |
| DEUX                               | Deux Remix                           |        | ``     |      170 |
| Turbo                              | Turbo Remix                          |        | ``     |      171 |
| CLON/Sechs Kies                    | 2nd Hidden Remix                     |        | ``     |      172 |
| Drunken Tiger/Honey Family         | Drunken Family Remix                 |        | ``     |      173 |
| BanYa                              | BanYa Hiphop Remix                   |        | ``     |      174 |
| BanYa                              | BanYa Classic Remix                  |        | ``     |      175 |
| BanYa                              | EXTRA BanYa Remix                    |        | ``     |      176 |


#### Another Step

| Artist           | Song                         | NOTE                                | Offset | Offset # |
| ---------------- | ---------------------------- | ----------------------------------- | ------ | -------: |
| BanYa            | Solitary                     |                                     | ``     |      177 |
| Jiny             | Free!                        |                                     | ``     |      178 |
| Apple Jam        | Snow Dream                   |                                     | ``     |      179 |
| May              | Handsome Character That Pass |                                     | ``     |      180 |
| Typhoon          | So                           | UNLOCK                              | ``     |      181 |
| BanYa            | Turkey March                 | UNLOCK                              | ``     |      182 |
| BanYa            | Come to Me                   | UNLOCK                              | ``     |      183 |
| Banya Production | Ugly Dee                     | UNLOCK                              | ``     |      184 |
| Banya Production | Caprice of Otada             | UNLOCK                              | ``     |      185 |
| YAHPP            | Fire (?game?)                | UNLOCK                              | ``     |      186 |
| Gill Gun         | A. U Ready?                  | UNLOCK                              | ``     |      187 |
| YAHPP            | Faster Z                     | UNLOCK                              | ``     |      188 |
| YAHPP            | Pumptris Quattro             | UNLOCK                              | ``     |      189 |
| Seotaji and Boys | It's My Business             | UNLOCK                              | ``     |      190 |
| No Brain         | Fall in Me                   | UNLOCK                              | ``     |      191 |
| Banya Production | Guitar Man                   | UNLOCK                              | ``     |      192 |
| Banya Production | Higgledy Piggledy            | UNLOCK                              | ``     |      193 |
| Elpis            | Dance Vibrations             | UNLOCK                              | ``     |      194 |
| BanYa            | Extravaganza                 |                                     | ``     |      195 |
| BanYa            | Love is a Danger Zone pt. 2  |                                     | ``     |      196 |
| BanYa            | Naissance 2                  |                                     | ``     |      197 |
|                  |                              | UNKNOWN                             | ``     |      198 |
| BanYa            | Love is a Danger Zone        |                                     | ``     |      199 |
| BanYa            | Witch Doctor                 |                                     | ``     |      200 |
| Lazy Bone        | Do It Yourself               |                                     | ``     |      201 |
| BanYa            | Mr. Larpus                   |                                     | ``     |      202 |
| BanYa            | Vook                         |                                     | ``     |      203 |
| Novasonic        | Another Truth                |                                     | ``     |      204 |
| BanYa            | Final Audition 2             |                                     | ``     |      205 |
| BanYa            | Final Audition               |                                     | ``     |      206 |
| BanYa            | Final Audition 3             |                                     | ``     |      207 |
| YAHPP            | Chimera                      |                                     | ``     |      208 |
| BanYa            | Beethoven Virus              |                                     | ``     |      209 |
| BanYa            | Bee                          | zde se neco pokazilo (o jedno vic?) | ``     |      210 |
| BanYa            | Final Audition Ep. 1         |                                     | ``     |      211 |
| YAHPP            | Final Audition Ep. 2-1       |                                     | ``     |      212 |
| YAHPP            | Final Audition Ep. 2-2       |                                     | ``     |      213 |
| YAHPP            | Witch Doctor #1              |                                     | ``     |      214 |
| BanYa            | Dr M.                        |                                     | ``     |      215 |
| N.E.X.T          | For You                      |                                     | ``     |      216 |
| BanYa            | Phantom                      |                                     | ``     |      217 |
| BanYa            | Hi Bi                        |                                     | ``     |      218 |
| SOM2             | Deja Vu                      |                                     | ``     |      219 |
| Banya Production | Gun Rock                     |                                     | ``     |      220 |
|                  |                              |                                     | ``     |  221-255 |


# nx2rank.bin

The decrypted rank file is exactly `12,296` bytes (`0x3008`). Unlike
`nx2save.bin`, the entire file is encrypted by the NX2 byte transform.

## Generalized layout

| File offset (hex) |   Size | Region                                                         |
| ----------------- | -----: | -------------------------------------------------------------- |
| `0000-0003`       |      4 | Adler-32 checksum, unsigned little-endian `uint32`             |
| `0004-0007`       |      4 | Number of rankings, unsigned little-endian `uint32`            |
| `0008-3007`       | 12,288 | Ranking entries: `128` ranking slots x `3` stages x `32` bytes |

Each ranking entry has this layout:

| Entry offset | Size | Field                                             |
| ------------ | ---: | ------------------------------------------------- |
| `+0x00`      |    4 | Game mode, signed little-endian `int32`           |
| `+0x04`      |    4 | Play order, signed little-endian `int32`          |
| `+0x08`      |    4 | Play score, signed little-endian `int32`          |
| `+0x0C`      |    4 | Grade, signed little-endian `int32`               |
| `+0x10`      |    4 | Mileage, signed little-endian `int32`             |
| `+0x14`      |    4 | Play time, IEEE-754 little-endian `float`         |
| `+0x18`      |    4 | Calories (`kcal`), IEEE-754 little-endian `float` |
| `+0x1C`      |    4 | VO2, IEEE-754 little-endian `float`               |

To locate an entry, use zero-based `rank_index` and `stage_index` values:

```text
entry_offset = 0x0008 + 0x20 * (rank_index * 3 + stage_index)
```

Valid indices are `0..127` for `rank_index` and `0..2` for `stage_index`.
The checksum covers bytes `0004-3007`, excluding the checksum field itself.
