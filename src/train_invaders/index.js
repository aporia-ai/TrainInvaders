const getGameIframe = () => document.querySelector(`.game-iframe`);

// If Jupyter notebook
if (typeof window !== 'undefined' && 'Jupyter' in window) {
    const outputArea = this
    const cellElement = outputArea.element.parents('.cell');
    const cellIndex = Jupyter.notebook.get_cell_elements().index(cellElement);
    const cell = Jupyter.notebook.get_cell(cellIndex);
    const cellId = cell.cell_id

    // Expand output fully
    outputArea.collapse(); outputArea.expand();

    if (!cell.__gameInitiated) {
        cell.__isRunning = true
        // On cell execution stop
        Jupyter.notebook.events.on('finished_execute.CodeCell', function (evt, data) {
            if (data.cell.cell_id === cellId) {
                const gameIframe = getGameIframe()
                if (gameIframe) gameIframe.contentWindow.postMessage({ event: 'finishedTraining' }, '*')
                cell.__isRunning = false
            }
        });
        // On cell execution start
        Jupyter.notebook.events.on('execute.CodeCell', function (evt, data) {
            if (data.cell.cell_id === cellId) {
                cell.__isRunning = true
            }
        });
        window.addEventListener("message", event => {
            if (event.data.event === `gameTrainingState`) {
                if (!cell.__isRunning) {
                    const gameIframe = getGameIframe()
                    if (gameIframe) gameIframe.contentWindow.postMessage({ event: 'finishedTraining' }, '*')
                }
            }
        });
        cell.__gameInitiated = true
    }
}

if(!window.__isThereGameCloseListener) {
    function gameCloseHandler(event) {
        if (event.data.event === `gameClose`) {
            const gameIframe = getGameIframe()
            if(gameIframe) getGameIframe().remove()
            window.removeEventListener("message", gameCloseHandler)
            window.__isThereGameCloseListener = false
        }
    }
    window.addEventListener("message", gameCloseHandler);
    window.__isThereGameCloseListener = true
}

// If online, run the game
if(navigator.onLine) {
    getGameIframe().contentDocument.write(atob("$$GAME_HTML_BASE64$$"))
    getGameIframe().focus()
} else {
    getGameIframe().height = 25
    getGameIframe().contentDocument.write(`[TrainInvaders] No internet connection :(`)
}
