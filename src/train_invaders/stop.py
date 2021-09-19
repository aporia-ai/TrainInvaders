import sys

# Stop settrace of `start` module
sys.settrace(None)

# Remove `start` module cache
if "train_invaders.start" in sys.modules:
    del sys.modules["train_invaders.start"]
