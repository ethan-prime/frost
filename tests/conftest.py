import sys
from pathlib import Path

# add repo root (parent of tests/) to import path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))