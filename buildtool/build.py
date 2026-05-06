import fnmatch
import os
from typing import List
import subprocess
import gpt_image
from fs import open_fs
from pyfatfs.PyFat import PyFat
import argparse

CXXFLAGS = [
    "--target=aarch64-none-elf",
    "-Wall",
    "-Wextra",
    "-std=c++23",
    "-ffreestanding",
    "-fno-stack-protector",
    "-fno-stack-check",
    "-fno-lto",
    "-fno-PIC",
    "-fno-exceptions",
    "-fno-rtti",
    "-fno-use-cxa-atexit",
    "-ffunction-sections",
    "-fdata-sections",
    "-march=armv8-a",
    "-mgeneral-regs-only",
    "-mstrict-align",
    "-Iinclude", 
]

LDFLAGS = [
    "-nostdlib",
    "-static",
    "-z", "max-page-size=0x1000",
    "--gc-sections",
    "-T", "src/linker.lds"
]

def parse_args():
    parser = argparse.ArgumentParser(prog="buildtool")
    subparsers = parser.add_subparsers(dest="command", required=True)
    
    build_parser = subparsers.add_parser("build")
    build_parser.set_defaults(func=build)

    qemu_parser = subparsers.add_parser("qemu")
    qemu_parser.set_defaults(func=qemu)
    return parser.parse_args()

def _find_source_files(directory: str) -> List[str]:
    source_files = []
    for item in os.listdir(directory):
        path = os.path.join(directory, item)
        if os.path.isdir(path):
            source_files.extend(_find_source_files(path))
        elif fnmatch.fnmatch(item, "*.cc"):
            source_files.append(path)
    return source_files

def _create_object_files(source_files: List[str]):
    object_files = []
    for file in source_files:
        filename, _ = os.path.splitext(file)
        output_path = os.path.join("build", filename + ".o")
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        cmds = ["clang++", *CXXFLAGS, "-c", file, "-o", output_path]
        result = subprocess.run(cmds, capture_output=True, text=True)
        if result.returncode != 0:
            raise RuntimeError(f"{result.stderr}")
        object_files.append(output_path)
    return object_files

def _link_binary(o_files: List[str]):
    os.makedirs("build", exist_ok=True)
    result = subprocess.run(["ld.lld",*LDFLAGS, *o_files, "-o", "build/kernel.elf"], text=True, capture_output=True)
    if result.returncode != 0:
        raise RuntimeError(f"{result.stderr}")
    return "build/kernel.elf"

def _copy_to_fat_disk(disk, src_path, dest_path):
    with open(src_path, "rb") as src:
        with disk.openbin(dest_path, "w") as dst:
            dst.write(src.read())

def build():
    # find source files
    source_files = _find_source_files("src")
    # create object files
    o_files = _create_object_files(source_files)
    # link binary
    binary = _link_binary(o_files) 
    # create image
    if os.path.exists("build/kernel.img"):
        os.remove("build/kernel.img")
    disk = gpt_image.disk.Disk("build/kernel.img")
    disk.create(128 * 1024 * 1024)
    boot_partition = gpt_image.partition.Partition(
            "boot",
            64 * 1024 * 1024,
            gpt_image.partition.PartitionType.EFI_SYSTEM_PARTITION.value
    )
    disk.table.partitions.add(boot_partition)
    disk.commit()
    offset = disk.table.partitions.entries[0].first_lba * disk.sector_size
    pf = PyFat(offset=offset)
    pf.mkfs("build/kernel.img", fat_type=32, size=64 * 1024 * 1024)
    pf.close()
    # add files to boot parition
    with open_fs(f"fat://build/kernel.img?offset={offset}") as fat:
        fat.makedirs("EFI/BOOT", recreate=True)
        _copy_to_fat_disk(fat, "bin/BOOTAA64.EFI", "EFI/BOOT/BOOTAA64.EFI")
        _copy_to_fat_disk(fat, "bin/limine.conf", "EFI/BOOT/limine.conf")
        _copy_to_fat_disk(fat, binary, "EFI/BOOT/kernel")

def qemu():
    build()
    subprocess.run([
        "qemu-system-aarch64",
        "-M", "virt",
        "-cpu", "cortex-a57",
        "-m", "512M",
        "-bios", "bin/QEMU_EFI.fd",
        "-drive", "if=none,file=build/kernel.img,format=raw,id=hd0",
        "-device", "virtio-blk-device,drive=hd0",
        "-serial", "stdio",
        "-monitor", "none",
        "-display", "none",
        "-semihosting-config", "enable=on,target=native"
    ], check=True)

def main():
    args = parse_args()
    args.func()

if __name__ == "__main__":
    main()
