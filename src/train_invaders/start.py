import base64
from pathlib import Path
import sys
from types import FrameType
from typing import Any, Dict, Optional

from IPython.display import display, HTML, Javascript

train_invaders_dir = Path(__file__).parent
iframe_html = """
    <iframe
        class="game-iframe"
        height="667"
        scrolling="no"
        style="border:0;
        width: 100%;"
        frameborder="0"
    ></iframe>
"""

def _get_template(path: Path, mapping: Optional[Dict[str, Any]] = None) -> str:
    """Gets the file and injects the values of the keys from the mapping."""
    with open(path, mode="r") as template_file:
        file = template_file.read()
        if mapping is not None:
            for key in mapping:
                file = file.replace(f"$${key}$$", mapping[key])
        return file


def _call_tracer(frame: FrameType, event: str, arg: Any):
    """Hook fit / train / train_on_batch methods and run the game of their call."""
    if event == "call" and (
        frame.f_code.co_name == "fit"
        or frame.f_code.co_name == "train"
        or frame.f_code.co_name == "train_on_batch"
    ):

        # Inject iframe
        display(HTML(iframe_html))

        # Create main logic file and inject in it the base64 HTLM of the view
        with open(train_invaders_dir / "view.txt", mode="r") as view:
            script_str = _get_template(
                path=train_invaders_dir / "index.js",
                mapping={
                    "GAME_HTML_BASE64": view.read()
                },
            )

        # Inject the logic
        display(Javascript(script_str))


# Start tracing
sys.settrace(_call_tracer)

# Remove `stop` module cache
if "train_invaders.stop" in sys.modules:
    del sys.modules["train_invaders.stop"]
