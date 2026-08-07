#!/usr/bin/env python3
# Incrusta un payload (recursos + driver Vulkan por software) al final de un
# ejecutable de Windows para lograr una aplicacion de un solo archivo.
# Formato del payload (little-endian):
#   MAGIC "GETFGPAYL" (8 bytes) | u64 count | por entrada: u32 nameLen + name + u64 dataLen + data
#   Trailer: FOOTER "GTRAILER!" (9) | u64 offsetInicioPayload
import argparse
import os
import struct

MAGIC = b"GETFGPAYL"
FOOTER = b"GTRAILER!"


def build_payload(entries):
    payload = MAGIC + struct.pack("<Q", len(entries))
    for name, path in entries:
        with open(path, "rb") as f:
            data = f.read()
        name_b = name.encode("utf-8")
        payload += struct.pack("<I", len(name_b))
        payload += name_b
        payload += struct.pack("<Q", len(data))
        payload += data
    return payload


def main():
    ap = argparse.ArgumentParser(description="Empaqueta recursos dentro del .exe")
    ap.add_argument("exe", help="Ejecutable a modificar")
    ap.add_argument("--entry", action="append", nargs=2,
                    metavar=("NAME", "PATH"),
                    help="Nombre dentro del payload y ruta real del archivo")
    args = ap.parse_args()

    if not args.entry:
        print("No hay entradas para incrustar.", file=os.sys.stderr)
        return 1

    payload = build_payload(args.entry)
    with open(args.exe, "r+b") as f:
        f.seek(0, os.SEEK_END)
        start = f.tell()
        f.write(payload)
        f.write(FOOTER + struct.pack("<Q", start))

    print(f"Payload incrustado: {len(payload)} bytes en offset {start}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())