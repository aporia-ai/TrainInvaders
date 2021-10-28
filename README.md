<h1 align="center">
  <br>
  <img src="https://raw.githubusercontent.com/aporia-ai/TrainInvaders/main/src/assets/logo.png" alt="TrainInvaders" width="200">
  <br>
    Train Invaders
    <a href="https://twitter.com/intent/tweet?text=Jupyter%20Notebook%20%2B%20Space%20Invaders%3F!%20Something%20fun%20to%20do%20while%20training%20your%20model%20%F0%9F%91%BE&url=https://github.com/aporia-ai/TrainInvaders&hashtags=MachineLearning,JupyterNotebook,DataScience,TrainInvaders" target="_blank">
        <img src="https://img.shields.io/badge/Twitter-1DA1F2?style=for-the-badge&logo=twitter&logoColor=white" width="70">
    </a>
</h1>

<h4 align="center">Jupyter Notebook + Space Invaders!?</h4>

<p align="center">
  <img src="https://img.shields.io/badge/python-+3.6-blue.svg"
         alt="Python version">
  <img src="https://img.shields.io/badge/contributions-welcome-orange.svg"
         alt="Python version">
  <img src="https://img.shields.io/badge/license-MIT-green.svg"
         alt="License">
</p>

<p align="center">
  <a href="#why">Why?</a> •
  <a href="#getting-started-">Getting started</a> •
  <a href="#how-it-works-%EF%B8%8F">How it works</a> •
  <a href="#faq-">FAQ</a> •
  <a href="#drawbacks-">Drawbacks</a> •
  <a href="#contribute-">Contribute</a> •
  <a href="#thanks-to-">Thanks to</a> •
  <a href="#you-may-also-%EF%B8%8F">You may also like...</a>
</p>

![Demo](https://raw.githubusercontent.com/aporia-ai/TrainInvaders/main/src/assets/demo.gif)

## Why❓
*Training a model can be a long long process!*

In the meantime, have a bit of fun with a **competitive space invaders game**.

See if you can get your name to the top of the *leaderboard*.

## Getting started 🏁
1. Install the game:
`!pip3 install train_invaders --upgrade`
2. Import the game in your notebook:
`import train_invaders.start`
3. Start training your model. The game will automatically pop up when the process begins.
4. **Play the game!** *You'll get notified when your training is finished*.
5. Want to stop the game from popping up when the model is being trained?
`import train_invaders.stop` will do the work.

## How it works ⚙️
**Tons of magic**... Just kidding :)

When importing the `start` module, its code will be executed.

The code utilizes python's `settrace` function which follows the functions' call stack.

When a function named `fit` `train` or `train_on_batch` is called - using Jupyter notebook's kernel, aka, `IPython`, a javascript view code will be injected inside the output block as an `iframe` to keep it completely **isolated from your code**.

When importing the `stop` module, the `settrace` function will be canceled and the function hooks will be removed.

## FAQ 🙋
### Will it interfere with the training process somehow?

NO. The game will start and be played **in parallel to the training** and will even *let you know when the training is finished*.

## Drawbacks 🥺
* Training stop indication is only in Jupyter Notebook. Want to get notified by email or text? Try [MLNotify](https://mlnotify.aporia.com/?utm_source=train-invaders&utm_medium=docs&utm_campaign=train-invaders)
* Authentication, and therefore, saving your score can only be done from `localhost` and port `8888 - 8891 / 8080 / 8081`

## Contribute 🤝
Have an awesome idea for a new feature? PRs are more than welcome!

1. Clone the project
2. Run `make build-game` to get a local and compiled copy of the game (if not exists)
2. Enter `src/view` directory and run `npm run serve` to run the local environment
2. Implement your ideas
3. Made changes in the game (C files)? Re-run `make build-game` from root dir and check them out
5. Enter root directory, run `make build`, `pip install . --upgrade` and test the changes in your notebook
6. PR us!

## Thanks to 🙏
[JanSiebert](https://github.com/JanSiebert/wasm-space-invaders) for the WebAssembly game.

[Cody Boisclair](https://github.com/codeman38) for the PressStart2P font.

[Vue](https://github.com/vuejs/vue) for the awesome FE framework.

## You may also ❤️
[Aporia](https://www.aporia.com/?utm_source=train-invaders&utm_medium=docs&utm_campaign=train-invaders) - Customized monitoring for your ML models.

[MLNotify](https://mlnotify.aporia.com/?utm_source=train-invaders&utm_medium=docs&utm_campaign=train-invaders) - Get notified when training is complete.

[MLOps Toys](https://mlops.toys/?utm_source=train-invaders&utm_medium=docs&utm_campaign=train-invaders) - A curated list of useful MLOps tools.
