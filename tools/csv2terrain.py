#!/usr/bin/env python3
"""Convert a tile-grid CSV into a .terrain.json file for cosmos TerrainLoader."""

import argparse
import csv
import json
import sys

VALID_ROTATIONS = {
    0:   [1,0,0,0,  0,1,0,0,  0,0,1,0],
    90:  [0,0,1,0,  0,1,0,0,  -1,0,0,0],
    180: [-1,0,0,0, 0,1,0,0,  0,0,-1,0],
    270: [0,0,-1,0, 0,1,0,0,  1,0,0,0],
}


def make_transform(rotation_deg, x, y, z):
    rot = VALID_ROTATIONS[rotation_deg]
    return rot + [x, y, z, 1]


def parse_csv(path):
    tile_size = 3.0
    tiles = {}  # name -> uri (insertion ordered)
    grid_rows = []

    with open(path, newline="") as f:
        reader = csv.reader(f)
        in_grid = False
        for line_num, row in enumerate(reader, start=1):
            if not row or (row[0].startswith("#") and not in_grid):
                continue

            first = row[0].strip()

            if not in_grid:
                if first.lower() == "grid":
                    in_grid = True
                    continue
                if first.lower() == "tile_size":
                    if len(row) < 2:
                        print(f"error: line {line_num}: tile_size missing value", file=sys.stderr)
                        sys.exit(1)
                    tile_size = float(row[1].strip())
                elif first.lower() == "tile":
                    if len(row) < 3:
                        print(f"error: line {line_num}: tile requires name and path", file=sys.stderr)
                        sys.exit(1)
                    name = row[1].strip()
                    uri = row[2].strip()
                    tiles[name] = uri
                continue

            grid_rows.append((line_num, row))

    if not in_grid:
        print("error: no 'grid' marker row found", file=sys.stderr)
        sys.exit(1)

    if not tiles:
        print("error: no tile definitions found", file=sys.stderr)
        sys.exit(1)

    return tile_size, tiles, grid_rows


def build_terrain(tile_size, tiles, grid_rows):
    instances = []
    used_tiles = set()

    for row_idx, (line_num, row) in enumerate(grid_rows):
        for col_idx, cell in enumerate(row):
            cell = cell.strip()
            if not cell:
                continue

            parts = cell.split(":")
            name = parts[0]
            rotation = int(parts[1]) if len(parts) > 1 else 0

            if name not in tiles:
                print(f"error: line {line_num}, col {col_idx + 1}: unknown tile '{name}'", file=sys.stderr)
                sys.exit(1)

            if rotation not in VALID_ROTATIONS:
                print(f"error: line {line_num}, col {col_idx + 1}: invalid rotation {rotation} "
                      f"(must be 0, 90, 180, or 270)", file=sys.stderr)
                sys.exit(1)

            x = col_idx * tile_size
            z = -row_idx * tile_size
            transform = make_transform(rotation, x, 0, z)

            instances.append({"mesh": name, "transform": transform})
            used_tiles.add(name)

    meshes = [{"name": n, "uri": tiles[n]} for n in tiles if n in used_tiles]
    return {"meshes": meshes, "instances": instances}


def main():
    parser = argparse.ArgumentParser(description="Convert a tile-grid CSV to .terrain.json")
    parser.add_argument("csv", help="input CSV file")
    parser.add_argument("-o", "--output", help="output JSON file (default: stdout)")
    args = parser.parse_args()

    tile_size, tiles, grid_rows = parse_csv(args.csv)
    terrain = build_terrain(tile_size, tiles, grid_rows)

    output = json.dumps(terrain, indent=2) + "\n"

    if args.output:
        with open(args.output, "w") as f:
            f.write(output)
        print(f"wrote {len(terrain['instances'])} instances to {args.output}", file=sys.stderr)
    else:
        sys.stdout.write(output)


if __name__ == "__main__":
    main()
