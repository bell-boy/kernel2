import fnmatch
import os
import threading
from typing import List
import subprocess
import gpt_image
from fs import open_fs
from pyfatfs.PyFat import PyFat
import argparse

COMPILE_FLAGS_PATH = "compile_flags.txt"

LDFLAGS = [
    "-nostdlib",
    "-static",
    "-z", "max-page-size=0x1000",
    "--gc-sections",
    "-T", "src/linker.lds"
]

TEST_TIMEOUT = 10  # seconds per test
KERNEL_TEST_MAGIC = "STARTING KERNEL INTEGRATION TEST"

def parse_args():
    parser = argparse.ArgumentParser(prog="buildtool")
    subparsers = parser.add_subparsers(dest="command", required=True)

    build_parser = subparsers.add_parser("build")
    build_parser.set_defaults(func=lambda args: build())

    qemu_parser = subparsers.add_parser("qemu")
    qemu_parser.set_defaults(func=lambda args: qemu())

    test_parser = subparsers.add_parser("test")
    test_parser.add_argument("name", help="Integration test name (without .cc extension)")
    test_parser.set_defaults(func=lambda args: test(args.name))

    test_all_parser = subparsers.add_parser("test-all")
    test_all_parser.set_defaults(func=lambda args: test_all())

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

def _create_object_files(source_files: List[str]) -> List[str]:
    object_files = []
    for file in source_files:
        filename, _ = os.path.splitext(file)
        output_path = os.path.join("build", filename + ".o")
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        result = subprocess.run(
            ["clang++", f"@{COMPILE_FLAGS_PATH}", "-c", file, "-o", output_path],
            capture_output=True, text=True
        )
        if result.returncode != 0:
            raise RuntimeError(result.stderr)
        object_files.append(output_path)
    return object_files

def _link_binary(o_files: List[str], output: str = "build/kernel.elf") -> str:
    os.makedirs(os.path.dirname(output), exist_ok=True)
    result = subprocess.run(
        ["ld.lld", *LDFLAGS, *o_files, "-o", output],
        capture_output=True, text=True
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr)
    return output

def _copy_to_fat_disk(disk, src_path, dest_path):
    with open(src_path, "rb") as src:
        with disk.openbin(dest_path, "w") as dst:
            dst.write(src.read())

def _create_disk_image(elf_path: str, img_path: str):
    os.makedirs(os.path.dirname(img_path) or ".", exist_ok=True)
    if os.path.exists(img_path):
        os.remove(img_path)
    disk = gpt_image.disk.Disk(img_path)
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
    pf.mkfs(img_path, fat_type=32, size=64 * 1024 * 1024)
    pf.close()
    with open_fs(f"fat://{img_path}?offset={offset}") as fat:
        fat.makedirs("EFI/BOOT", recreate=True)
        _copy_to_fat_disk(fat, "bin/BOOTAA64.EFI", "EFI/BOOT/BOOTAA64.EFI")
        _copy_to_fat_disk(fat, "bin/limine.conf", "EFI/BOOT/limine.conf")
        _copy_to_fat_disk(fat, elf_path, "EFI/BOOT/kernel")

def build():
    source_files = _find_source_files("src")
    o_files = _create_object_files(source_files)
    binary = _link_binary(o_files)
    _create_disk_image(binary, "build/kernel.img")

def qemu():
    build()
    subprocess.run([
        "qemu-system-aarch64",
        "-machine", "virt,acpi=off",
        "-cpu", "cortex-a72",
        "-smp", "1",
        "-m", "512M",
        "-d", "int,guest_errors",
        "-D", "qemu.log",
        "-bios", "bin/QEMU_EFI.fd",
        "-drive", "if=none,file=build/kernel.img,format=raw,id=hd0",
        "-device", "virtio-blk-device,drive=hd0",
        "-monitor", "none",
        "-display", "none",
        "-semihosting-config", "enable=on,target=native"
    ], text=True)

# main.cc provides the default main(); exclude it when building tests
# so each test file can supply its own main() instead.
_KERNEL_MAIN = os.path.normpath("src/main.cc")

def _build_test_image(name: str, test_file: str) -> str:
    """Compile test_file alongside src/ (minus kernel_main.cc), return disk image path."""
    src_files = [
        f for f in _find_source_files("src")
        if os.path.normpath(f) != _KERNEL_MAIN
    ]
    o_files = _create_object_files(src_files + [test_file])
    elf_path = f"build/integration_tests/{name}.elf"
    _link_binary(o_files, elf_path)
    img_path = f"build/integration_tests/{name}.img"
    _create_disk_image(elf_path, img_path)
    return img_path

def _run_test_image(img_path: str) -> tuple[bool, str]:
    """Boot a disk image in QEMU. Returns (timed_out, output)."""
    name = os.path.splitext(img_path)[0]
    proc = subprocess.Popen([
        "qemu-system-aarch64",
        "-machine", "virt,acpi=off",
        "-cpu", "cortex-a72",
        "-smp", "1",
        "-D", f"{name}.qlog",
        "-d", "int,guest_errors",
        "-m", "512M",
        "-bios", "bin/QEMU_EFI.fd",
        "-drive", f"if=none,file={img_path},format=raw,id=hd0",
        "-device", "virtio-blk-device,drive=hd0",
        "-monitor", "none",
        "-display", "none",
        "-semihosting-config", "enable=on,target=native"
    ], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

    chunks: list[str] = []

    def _read():
        # Reads until the pipe closes (process exits or is killed).
        for chunk in iter(lambda: proc.stdout.read(4096), ""):
            chunks.append(chunk)

    reader = threading.Thread(target=_read, daemon=True)
    reader.start()
    reader.join(timeout=TEST_TIMEOUT)

    timed_out = reader.is_alive()
    if proc.poll() is None:  # Still running — kill it.
        proc.kill()
    reader.join()
    proc.wait()

    return timed_out, "".join(chunks)

def _normalized_test_output(output: str) -> list[str]:
    marker = output.rfind(KERNEL_TEST_MAGIC)
    if marker != -1:
        line_end = output.find("\n", marker)
        output = "" if line_end == -1 else output[line_end + 1:]

    lines = [line.rstrip() for line in output.splitlines()]
    while lines and not lines[-1]:
        lines.pop()
    return lines

def _outputs_match(actual: str, expected: str) -> bool:
    return _normalized_test_output(actual) == _normalized_test_output(expected)

def _run_one_test(name: str) -> tuple[bool, str]:
    """Build and run a single test. Returns (passed, status)."""
    test_file = os.path.join("integration_tests", name + ".cc")
    ok_file   = os.path.join("integration_tests", name + ".ok")
    result_dir  = "build/integration_tests"
    result_file = os.path.join(result_dir, name + ".result")
    output_file = os.path.join(result_dir, name + ".output")

    if not os.path.exists(test_file):
        raise FileNotFoundError(f"Test not found: {test_file}")
    if not os.path.exists(ok_file):
        raise FileNotFoundError(f".ok file not found: {ok_file}")

    # Create the output directory before anything that can fail so that the
    # finally block can always write result/output files.
    os.makedirs(result_dir, exist_ok=True)

    output = ""
    status = "ERROR"
    try:
        print(f"  [build] {name}")
        img_path = _build_test_image(name, test_file)

        print(f"  [run]   {name}")
        timed_out, output = _run_test_image(img_path)

        if timed_out:
            status = "TIMEOUT"
        else:
            with open(ok_file) as f:
                expected = f.read()
            status = "PASS" if _outputs_match(output, expected) else "FAIL"

        if status == "FAIL":
            with open(ok_file) as f:
                expected = f.read()
            print("  --- expected ---")
            print(expected.rstrip())
            print("  --- actual ---")
            print("\n".join(_normalized_test_output(output)))
    finally:
        with open(output_file, "w") as f:
            f.write(output)
        with open(result_file, "w") as f:
            f.write(status + "\n")

    return status == "PASS", status

def test(name: str):
    passed, status = _run_one_test(name)
    print(f"[{status}] {name}")
    if not passed:
        raise SystemExit(1)

def test_all():
    if not os.path.isdir("integration_tests"):
        print("No integration_tests directory found.")
        return

    names = sorted(
        f[:-3] for f in os.listdir("integration_tests") if f.endswith(".cc")
    )
    if not names:
        print("No integration tests found.")
        return

    results: dict[str, str] = {}
    for name in names:
        try:
            _, status = _run_one_test(name)
            results[name] = status
        except (RuntimeError, FileNotFoundError) as e:
            print(f"  error: {e}")
            results[name] = "ERROR"
        print(f"[{results[name]}] {name}")

    print(f"\n{'=' * 40}")
    passed = sum(1 for s in results.values() if s == "PASS")
    print(f"{passed}/{len(results)} passed")
    for name, status in results.items():
        print(f"  [{status}] {name}")

    if any(s != "PASS" for s in results.values()):
        raise SystemExit(1)

def main():
    args = parse_args()
    args.func(args)

if __name__ == "__main__":
    main()
