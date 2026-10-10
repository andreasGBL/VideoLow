import argparse
import os
import struct
import tempfile
import zlib
from pathlib import Path


def compress_executable(source: Path, level: int) -> Path:
    data = source.read_bytes()
    if len(data) > 0xFFFFFFFF:
        raise ValueError(f"File is too large for Qt qCompress format: {source}")

    compressed = struct.pack(">I", len(data)) + zlib.compress(data, level)
    destination = source.with_name(source.name + ".qcomp")
    temporary_path = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="wb", dir=source.parent, prefix=destination.name + ".", suffix=".tmp", delete=False
        ) as temporary:
            temporary_path = Path(temporary.name)
            temporary.write(compressed)
        os.replace(temporary_path, destination)
    finally:
        if temporary_path is not None and temporary_path.exists():
            temporary_path.unlink()

    return destination


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Create Qt qCompress-compatible sidecars for FFmpeg executables."
    )
    parser.add_argument("files", nargs="*", type=Path, help="Executables to compress")
    parser.add_argument("--level", type=int, choices=range(10), default=9)
    arguments = parser.parse_args()

    base_directory = Path(__file__).resolve().parent
    sources = arguments.files or [Path("ffmpeg.exe"), Path("ffprobe.exe")]
    try:
        for source in sources:
            if not source.is_absolute():
                source = base_directory / source
            destination = compress_executable(source, arguments.level)
            print(f"{source} -> {destination} ({destination.stat().st_size:,} bytes)")
    except (OSError, ValueError) as error:
        parser.error(str(error))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())