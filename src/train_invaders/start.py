import base64
from pathlib import Path
import sys
from types import FrameType
from typing import Any, Dict, Optional

from IPython.display import display, HTML, Javascript

train_invaders_dir = Path(__file__).parent
assets_dir = train_invaders_dir / "assets"
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


def _get_base64_file(path: Path) -> str:
    with open(path, mode="rb") as asset:
        return base64.b64encode(asset.read()).decode("utf-8")


def _get_template(path: Path, mapping: Optional[Dict[str, Any]] = None) -> str:
    with open(path, mode="r") as template_file:
        file = template_file.read()
        if mapping is not None:
            for key in mapping:
                file = file.replace(f"$${key}$$", mapping[key])
        return file


def _call_tracer(frame: FrameType, event: str, arg: Any):
    # Hook fit/train.. methods calls
    if event == "call" and (
        frame.f_code.co_name == "fit"
        or frame.f_code.co_name == "train"
        or frame.f_code.co_name == "train_on_batch"
    ):

        view_script = _get_template(
            path=train_invaders_dir / "view.js",
            mapping={
                "GAME_BINARY_BASE64": _get_base64_file(train_invaders_dir / "game.wasm"),
                "GAME_HEART_ICON": _get_base64_file(assets_dir / "heart.png"),
                "GAME_APORIA_LOGO": _get_base64_file(assets_dir / "aporia-logo.png"),
                "GAME_SUCCESS_ICON": _get_base64_file(assets_dir / "success.png"),
            },
        )

        game_html = _get_template(
            path=train_invaders_dir / "index.html",
            mapping={
                "GAME_VIEW_SCRIPT": view_script,
                "GAME_VIEW_CSS": _get_template(train_invaders_dir / "styles.css"),
                "GAME_BACKGROUND_BASE64": _get_base64_file(assets_dir / "bg.png"),
            },
        )

        display(HTML(iframe_html))

        script_str = _get_template(
            path=train_invaders_dir / "index.js",
            mapping={
                "GAME_HTML_BASE64": base64.b64encode(game_html.encode("utf-8")).decode("utf-8")
            },
        )

        display(Javascript(script_str))


# Start tracing
sys.settrace(_call_tracer)

# Remove `stop` module cache
if "train_invaders.stop" in sys.modules:
    del sys.modules["train_invaders.stop"]
