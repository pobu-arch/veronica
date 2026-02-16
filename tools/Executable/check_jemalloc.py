#!/usr/bin/env python3
import os
import re
import glob
import platform
import ctypes
import ctypes.util
import subprocess

system = platform.system().lower()
arch = platform.machine().lower()

def linux_candidates():
    c = []

    # 0) 手工覆盖
    env_lib = os.environ.get("JEMALLOC_LIB")
    if env_lib:
        c.append(env_lib)

    # 1) ctypes 查找
    libname = ctypes.util.find_library("jemalloc")
    if libname:
        c.append(libname)

    # 2) ldconfig -p（最可靠）
    try:
        out = subprocess.check_output(["ldconfig", "-p"], text=True, stderr=subprocess.DEVNULL)
        for line in out.splitlines():
            if "jemalloc" not in line:
                continue
            m = re.search(r"=>\s+(\S+)", line)
            if m:
                c.append(m.group(1))
    except Exception:
        pass

    # 3) 常见目录
    dirs = [
        "/usr/lib/aarch64-linux-gnu",
        "/lib/aarch64-linux-gnu",
        "/usr/lib64",
        "/lib64",
        "/usr/lib",
        "/lib",
        "/usr/local/lib",
    ]
    names = ["libjemalloc.so.2", "libjemalloc.so", "libjemalloc.so.*"]
    for d in dirs:
        for n in names:
            c.extend(glob.glob(os.path.join(d, n)))

    # 4) 通用名兜底
    c.extend(["libjemalloc.so.2", "libjemalloc.so"])

    # 去重保序
    seen = set()
    out = []
    for x in c:
        if x and x not in seen:
            seen.add(x)
            out.append(x)
    return out

def darwin_candidates():
    c = []
    env_lib = os.environ.get("JEMALLOC_LIB")
    if env_lib:
        c.append(env_lib)

    libname = ctypes.util.find_library("jemalloc")
    if libname:
        c.append(libname)

    base_dirs = ["/opt/homebrew/lib", "/usr/local/lib", "/usr/lib"]
    for d in base_dirs:
        for n in ("libjemalloc.2.dylib", "libjemalloc.dylib"):
            p = os.path.join(d, n)
            if os.path.exists(p):
                c.append(p)

    c.extend(["libjemalloc.2.dylib", "libjemalloc.dylib"])
    return c

if system == "linux":
    candidates = linux_candidates()
elif system == "darwin":
    candidates = darwin_candidates()
else:
    raise SystemExit(f"unsupported system: {system}")

lib = None
loaded_input = None
errors = []

for p in candidates:
    try:
        lib = ctypes.CDLL(p)
        loaded_input = p
        break
    except OSError as e:
        errors.append(f"{p}: {e}")

if lib is None:
    msg = "\n".join(errors[-10:]) if errors else "no candidates"
    raise SystemExit(
        f"jemalloc not found (system={system}, arch={arch})\nlast tries:\n{msg}"
    )

if not hasattr(lib, "mallctl"):
    raise SystemExit(f"loaded but no mallctl: {loaded_input}")

mallctl = lib.mallctl
mallctl.argtypes = [
    ctypes.c_char_p,
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_size_t),
    ctypes.c_void_p,
    ctypes.c_size_t,
]
mallctl.restype = ctypes.c_int

ver_ptr = ctypes.c_char_p()
sz = ctypes.c_size_t(ctypes.sizeof(ctypes.c_void_p))
rc = mallctl(b"version", ctypes.byref(ver_ptr), ctypes.byref(sz), None, 0)
if rc != 0 or not ver_ptr.value:
    raise SystemExit(f"mallctl('version') failed, rc={rc}")

# dladdr 获取真实加载路径
class DlInfo(ctypes.Structure):
    _fields_ = [
        ("dli_fname", ctypes.c_char_p),
        ("dli_fbase", ctypes.c_void_p),
        ("dli_sname", ctypes.c_char_p),
        ("dli_saddr", ctypes.c_void_p),
    ]

loaded_path = os.path.realpath(loaded_input)
try:
    libsys = ctypes.CDLL(None)
    if hasattr(libsys, "dladdr"):
        dladdr = libsys.dladdr
        dladdr.argtypes = [ctypes.c_void_p, ctypes.POINTER(DlInfo)]
        dladdr.restype = ctypes.c_int
        info = DlInfo()
        ok = dladdr(ctypes.cast(lib.mallctl, ctypes.c_void_p), ctypes.byref(info))
        if ok and info.dli_fname:
            loaded_path = os.path.realpath(info.dli_fname.decode())
except Exception:
    pass

print(f"system: {system}")
print(f"arch: {arch}")
print(f"path: {loaded_path}")
print(f"version: {ver_ptr.value.decode()}")