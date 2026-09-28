#!/usr/bin/env python3
"""Set NXA profile player IDs and mileage in an encrypted nxasave.bin file."""

import argparse
import struct
import zlib
from pathlib import Path

SAVE_SIZE = 90868
REVIEW_SIZE = 0x144
CHECKSUM_OFFSET = REVIEW_SIZE
STATS_DATA_OFFSET = CHECKSUM_OFFSET + 4
REVIEW_PLAYER_ID_OFFSET = 0x0000
REVIEW_MILEAGE_OFFSET = 0x000C
STATS_PLAYER_ID_OFFSET = 0x019C
STATS_MILEAGE_OFFSET = 0x01A8
PLAYER_ID_SIZE = 12
DEFAULT_MILEAGE = 9999


def decrypt_stats(data: bytearray) -> None:
    """Decrypt bytes 0x0144 through the end, matching the C implementation."""
    stats = data[CHECKSUM_OFFSET:]
    for index in range(len(stats) - 1, 0, -1):
        adjustment = (index * 1234567) >> 8
        stats[index] = ((stats[index] ^ stats[index - 1]) + adjustment) & 0xFF
    data[CHECKSUM_OFFSET:] = stats


def encrypt_stats(data: bytearray) -> None:
    """Encrypt bytes 0x0144 through the end, matching the C implementation."""
    stats = data[CHECKSUM_OFFSET:]
    for index in range(1, len(stats)):
        adjustment = (index * 1234567) >> 8
        stats[index] = (((stats[index] - adjustment) & 0xFF) ^ stats[index - 1]) & 0xFF
    data[CHECKSUM_OFFSET:] = stats


def fixed_string(value: str, size: int) -> bytes:
    encoded = value.encode("ascii")
    if len(encoded) > size:
        raise ValueError(f"player ID must be at most {size} ASCII bytes")
    return encoded + b"\0" * (size - len(encoded))


def update_player_ids(data: bytearray, player_id: bytes) -> None:
    data[REVIEW_PLAYER_ID_OFFSET : REVIEW_PLAYER_ID_OFFSET + PLAYER_ID_SIZE] = player_id
    data[STATS_PLAYER_ID_OFFSET : STATS_PLAYER_ID_OFFSET + PLAYER_ID_SIZE] = player_id


def update_mileage(data: bytearray, mileage: int) -> None:
    struct.pack_into("<I", data, REVIEW_MILEAGE_OFFSET, mileage)
    struct.pack_into("<i", data, STATS_MILEAGE_OFFSET, mileage)


def main() -> None:
    parser = argparse.ArgumentParser(
        description=(
            "Decrypt nxasave.bin, set profile player IDs and mileage, "
            "recalculate Adler-32, and encrypt it."
        )
    )
    parser.add_argument("input", type=Path, help="encrypted nxasave.bin")
    parser.add_argument("output", type=Path, help="encrypted output file")
    parser.add_argument(
        "player_id",
        nargs="?",
        help="optional ASCII player ID stored in the review and stats profile headers",
    )
    parser.add_argument(
        "--mileage",
        type=int,
        default=DEFAULT_MILEAGE,
        help=f"review/stats mileage value (default: {DEFAULT_MILEAGE})",
    )
    args = parser.parse_args()

    if args.mileage < 0:
        raise SystemExit("error: mileage must be a non-negative integer")
    if args.mileage > 0x7FFFFFFF:
        raise SystemExit("error: mileage must fit in a signed 32-bit integer")

    data = bytearray(args.input.read_bytes())
    if len(data) != SAVE_SIZE:
        raise SystemExit(
            f"error: expected {SAVE_SIZE} bytes, got {len(data)} bytes in {args.input}"
        )

    decrypt_stats(data)
    if args.player_id is not None:
        player_id = fixed_string(args.player_id, PLAYER_ID_SIZE)
        update_player_ids(data, player_id)
    update_mileage(data, args.mileage)
    checksum = zlib.adler32(data[STATS_DATA_OFFSET:]) & 0xFFFFFFFF
    struct.pack_into("<I", data, CHECKSUM_OFFSET, checksum)
    encrypt_stats(data)
    args.output.write_bytes(data)

    player_summary = args.player_id if args.player_id is not None else "unchanged"
    print(
        f"wrote {args.output}: checksum 0x{checksum:08X}; "
        f"player {player_summary}; mileage {args.mileage}"
    )


if __name__ == "__main__":
    main()