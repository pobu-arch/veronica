#!/usr/bin/env python3
import os
import platform
import ctypes
import ctypes.util

system = platform.system().lower()
arch = platform.machine().lower()

# 1) 生成候选库路径（Linux + macOS）
candidates = []

if system == "linux":
    base_dir = {
        "x86_64": "/usr/lib/x86_64-linux-gnu",
        "amd64": "/usr/lib/x86_64-linux-gnu",
        "aarch64": "/usr/lib/aarch64-linux-gnu",
        "arm64": "/usr/lib/aarch64-linux-gnu",
    }.get(arch)

    if base_dir:
        for name in ("libjemalloc.so.2", "libjemalloc.so"):
            candidates.append(os.path.join(base_dir, name))

elif system == "darwin":
    # Apple Silicon 常见 Homebrew 路径: /opt/homebrew/lib
    # Intel Mac 常见 Homebrew 路径: /usr/local/lib
    base_dirs = []
    if arch in ("arm64", "aarch64"):
        base_dirs = ["/opt/homebrew/lib", "/usr/local/lib"]
    else:
        base_dirs = ["/usr/local/lib", "/opt/homebrew/lib"]

    for d in base_dirs:
        for name in ("libjemalloc.2.dylib", "libjemalloc.dylib"):
            candidates.append(os.path.join(d, name))

# 兜底：让动态链接器按系统默认路径查找
libname = ctypes.util.find_library("jemalloc")
if libname:
    candidates.append(libname)

# 再补一组通用 soname
candidates.extend(["libjemalloc.so.2", "libjemalloc.so", "libjemalloc.2.dylib", "libjemalloc.dylib"])

# 去重并加载
seen = set()
uniq_candidates = []
for c in candidates:
    if c and c not in seen:
        seen.add(c)
        uniq_candidates.append(c)

lib = None
loaded_input = None
last_err = None
for p in uniq_candidates:
    try:
        lib = ctypes.CDLL(p)
        loaded_input = p
        break
    except OSError as e:
        last_err = e

if lib is None:
    raise SystemExit(f"jemalloc not found (system={system}, arch={arch}). last_error={last_err}")

# 2) mallctl("version")
if not hasattr(lib, "mallctl"):
    raise SystemExit("loaded library has no mallctl symbol; not a usable jemalloc?")

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

version = ver_ptr.value.decode()

# 3) 取实际加载路径（dladdr）
class DlInfo(ctypes.Structure):
    _fields_ = [
        ("dli_fname", ctypes.c_char_p),
        ("dli_fbase", ctypes.c_void_p),
        ("dli_sname", ctypes.c_char_p),
        ("dli_saddr", ctypes.c_void_p),
    ]

loaded_path = os.path.realpath(loaded_input) if loaded_input else "<unknown>"

try:
    # macOS/Linux 都可尝试从主进程符号表取 dladdr
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
print(f"version: {version}")