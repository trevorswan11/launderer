import subprocess
import sys
from pathlib import Path

EXTENSIONS = {".cpp", ".hpp", ".c", ".h"}
EXCLUDE_DIRS = {"build", ".git", "managed_components"}

files_to_format = []

# Recursive search
for path in Path.cwd().rglob('*'):
    if any(part in EXCLUDE_DIRS for part in path.parts):
        continue
        
    if path.suffix in EXTENSIONS:
        files_to_format.append(str(path))

if not files_to_format:
    exit(0)

try:
    subprocess.run(["clang-format", "-i"] + files_to_format, check=True)
except FileNotFoundError:
    print("Error: 'clang-format' not found in your PATH.", file=sys.stderr)
except subprocess.CalledProcessError as e:
    print(f"Error during formatting: {e}", file=sys.stderr)

