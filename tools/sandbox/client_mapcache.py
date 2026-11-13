#!/usr/bin/env python3
# [RO Sandbox] Build server map data from the client's own map files.
#
#   python3 tools/sandbox/client_mapcache.py <client folder> <map> [<map>...] [--check]
#
# Reads data\<map>.gat and .rsw from the client (its data folder first, then
# its GRFs in DATA.INI order) and converts the cells the way rAthena's
# mapcache tool does. With --check, only reports how many cells differ from
# the map the server loads now. Without it, writes db/import/map_cache.dat
# with those maps (other maps already in that file are kept).
# The generated cache holds walkability data only, no Gravity art.
# Standard library only.

import os
import re
import struct
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import grf  # noqa: E402  (tools/sandbox/grf.py)

IMPORT_CACHE = "db/import/map_cache.dat"
SERVER_CACHES = [IMPORT_CACHE, "db/pre-re/map_cache.dat", "db/map_cache.dat"]
RSW_NO_WATER = 1000000
NAMES = {0: "walkable", 1: "wall", 2: "?", 3: "water", 4: "?", 5: "cliff", 6: "?"}


def read_cache(path):
    """{name: (xs, ys, raw compressed entry, cells)}"""
    maps = {}
    if not os.path.exists(path):
        return maps
    d = open(path, "rb").read()
    _, count = struct.unpack_from("<IH", d, 0)
    pos = 8
    for _ in range(count):
        name = d[pos:pos + 12].split(b"\0")[0].decode()
        xs, ys, ln = struct.unpack_from("<hhi", d, pos + 12)
        maps[name] = (xs, ys, d[pos:pos + 20 + ln], zlib.decompress(d[pos + 20:pos + 20 + ln]))
        pos += 20 + ln
    return maps


def client_files(client, names):
    """{grf path: bytes} from the client's data folder, then its GRFs in DATA.INI order."""
    found = {}
    for n in names:
        loose = os.path.join(client, *n.split("\\"))
        if os.path.exists(loose):
            found[n] = open(loose, "rb").read()
    ini = os.path.join(client, "DATA.INI")
    if not os.path.exists(ini):
        sys.exit(f"{ini} not found: is this the client folder?")
    grfs = []
    for line in open(ini, encoding="latin-1"):
        m = re.match(r"\s*(\d+)\s*=\s*(\S.*\.grf)\s*$", line, re.I)
        if m:
            grfs.append((int(m.group(1)), m.group(2)))
    for _, g in sorted(grfs):
        path = os.path.join(client, g)
        left = [n for n in names if n not in found]
        if left and os.path.exists(path):
            for n, data in grf.read_files(path, left).items():
                found[n] = data
                print(f"  {n} from {g}")
    return found


def water_level(rsw):
    if rsw is None or not rsw.startswith(b"GRSW"):
        return RSW_NO_WATER
    version = (rsw[4] << 8) | rsw[5]
    if version < 0x104 or version > 0x205:
        sys.exit(f"RSW version {version:#06x} is not supported")
    off = 171 if version >= 0x205 else 167 if version >= 0x202 else 166
    return int(struct.unpack_from("<f", rsw, off)[0])


def convert(gat, rsw):
    """(xs, ys, cells) like rAthena's mapcache read_map()."""
    xs, ys = struct.unpack_from("<II", gat, 6)
    water = water_level(rsw)
    cells = bytearray(xs * ys)
    off = 14
    for i in range(xs * ys):
        height = struct.unpack_from("<f", gat, off)[0]
        kind = struct.unpack_from("<I", gat, off + 16)[0]
        off += 20
        if kind == 0 and water != RSW_NO_WATER and height > water:
            kind = 3
        cells[i] = kind
    return xs, ys, bytes(cells)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if len(args) < 2:
        sys.exit("usage: client_mapcache.py <client folder> <map> [<map>...] [--check]")
    check = "--check" in sys.argv
    client, maps = args[0], args[1:]

    files = client_files(client, [f"data\\{m}.{ext}" for m in maps for ext in ("gat", "rsw")])
    server = {}
    for path in reversed(SERVER_CACHES):  # the server reads the import cache first
        server.update(read_cache(path))

    built = {}
    for m in maps:
        gat = files.get(f"data\\{m}.gat")
        if gat is None:
            sys.exit(f"data\\{m}.gat not found in the client")
        xs, ys, cells = convert(gat, files.get(f"data\\{m}.rsw"))
        built[m] = (xs, ys, cells)
        if m not in server:
            print(f"{m}: client {xs}x{ys}; the server has no such map")
            continue
        sxs, sys_, _, scells = server[m]
        if (sxs, sys_) != (xs, ys):
            print(f"{m}: client {xs}x{ys}, server {sxs}x{sys_}: different maps")
            continue
        walk = lambda c: c in (0, 3)
        diff = [i for i in range(xs * ys) if walk(cells[i]) != walk(scells[i])]
        print(f"{m}: {xs}x{ys}, {len(diff)} cells differ in walkability "
              f"({sum(1 for i in diff if walk(cells[i]))} walkable on the client but blocked on the server)")
        for i in diff[:5]:
            print(f"    {i % xs},{i // xs}: client {NAMES.get(cells[i], cells[i])}, "
                  f"server {NAMES.get(scells[i], scells[i])}")

    if check:
        return
    out = {n: v[2] for n, v in read_cache(IMPORT_CACHE).items()}
    for m, (xs, ys, cells) in built.items():
        data = zlib.compress(cells)
        out[m] = struct.pack("<12shhi", m.encode(), xs, ys, len(data)) + data
    body = b"".join(out.values())
    with open(IMPORT_CACHE, "wb") as f:
        f.write(struct.pack("<IHxx", 8 + len(body), len(out)) + body)
    print(f"Wrote {IMPORT_CACHE}: {', '.join(out)}")


if __name__ == "__main__":
    main()
