import os
import re
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parent
RELEASE_DIR = ROOT / "build" / "Release"
EXCLUDED_FILES = {
    "ffmpeg.exe",
    "ffprobe.exe",
    "VideoLow_DEBUG.exe",
}
EXCLUDED_SUFFIXES = {".exp", ".ilk", ".lib", ".pdb"}
REQUIRED_FILES = {
    "VideoLow.exe",
    "VideoLow_AMD.exe",
    "VideoLow_Nvidia.exe",
    "ffmpeg.exe.qcomp",
    "ffprobe.exe.qcomp",
    "Qt5Core.dll",
    "Qt5Widgets.dll",
    "platforms/qwindows.dll",
}


def run_build() -> None:
    environment = os.environ.copy()
    environment["CI"] = "1"
    subprocess.run(
        ["cmd.exe", "/d", "/c", "call", str(ROOT / "build-all.bat")],
        cwd=ROOT,
        stdin=subprocess.DEVNULL,
        env=environment,
        check=True,
    )


def release_files() -> list[Path]:
    if not RELEASE_DIR.is_dir():
        raise FileNotFoundError(f"Release output directory not found: {RELEASE_DIR}")

    files = []
    for path in RELEASE_DIR.rglob("*"):
        if not path.is_file():
            continue
        relative_path = path.relative_to(RELEASE_DIR)
        if relative_path.as_posix() in EXCLUDED_FILES:
            continue
        if path.suffix.lower() in EXCLUDED_SUFFIXES:
            continue
        files.append(path)

    included = {path.relative_to(RELEASE_DIR).as_posix() for path in files}
    missing = REQUIRED_FILES - included
    if missing:
        raise FileNotFoundError(f"Release output is missing required files: {', '.join(sorted(missing))}")
    return files


def create_archive(version: str) -> Path:
    archive_path = ROOT / f"VideoLow.{version}.Windows.x64.zip"
    temporary_path = None
    try:
        with tempfile.NamedTemporaryFile(
            dir=ROOT, prefix=archive_path.name + ".", suffix=".tmp", delete=False
        ) as temporary:
            temporary_path = Path(temporary.name)

        with zipfile.ZipFile(temporary_path, "w", allowZip64=True) as archive:
            for path in release_files():
                relative_path = path.relative_to(RELEASE_DIR)
                compression = zipfile.ZIP_STORED if path.suffix == ".qcomp" else zipfile.ZIP_DEFLATED
                archive.write(path, relative_path.as_posix(), compress_type=compression, compresslevel=9)
            archive.write(ROOT / "LICENSE", "LICENSE", compress_type=zipfile.ZIP_DEFLATED)

        os.replace(temporary_path, archive_path)
    finally:
        if temporary_path is not None and temporary_path.exists():
            temporary_path.unlink()

    return archive_path


def main() -> int:
    version = (ROOT / "version.txt").read_text(encoding="utf-8").strip()
    if not re.fullmatch(r"\d+(?:\.\d+)+", version):
        print(f"Invalid version in version.txt: {version!r}", file=sys.stderr)
        return 1

    try:
        run_build()
        archive_path = create_archive(version)
    except (OSError, subprocess.CalledProcessError, zipfile.BadZipFile) as error:
        print(f"Release packaging failed: {error}", file=sys.stderr)
        return 1

    print(f"Created {archive_path} ({archive_path.stat().st_size:,} bytes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())