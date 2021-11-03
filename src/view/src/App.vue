<template>
  <div id="app" class="app">
    <div v-if="shouldGiveTrainingIndication" class="training-status">
      <div v-if="isTrainingFinished" class="training-status-finished">
        Training is finished
        <img src="./assets/success.png" alt="Success icon" class="success-icon" />
      </div>
      <div v-else>Training...</div>
    </div>
    <CloseButton @click="closeGame" />
    <div class="inner">
      <div class="canvas-wrapper">
        <canvas id="canvas" class="canvas" ref="canvas" width="128" height="128"></canvas>
        <div class="aporia-credit">
          Brought to you with
          <img src="./assets/heart.png" alt="Heart icon" class="heart-icon" /> By
          <a
            href="https://www.aporia.com?utm_source=train-invaders&utm_medium=inhouse-application&utm_campaign=train-invaders"
            target="_blank"
            class="aporia-logo-wrapper"
          >
            <img src="./assets/aporia-logo.png" alt="Aporia logo" class="aporia-logo" />
          </a>
        </div>
      </div>
    </div>
    <div class="leaderboard-wrapper">
      <Leaderboard
        v-if="isLeaderboardVisible"
        :selfScore="gameState.score"
        @close="setLeaderboardVisibility(false)"
      />
    </div>
  </div>
</template>

<script>
import gameBase64 from '!raw-loader!./assets/game.txt';

import CloseButton from './components/CloseButton.vue';
import Leaderboard from './components/Leaderboard.vue';

export default {
  components: { Leaderboard, CloseButton },
  mounted() {
    this.shouldGiveTrainingIndication = typeof window !== 'undefined' && 'Jupyter' in parent.window;

    this.initGame();

    window.onmessage = (event) => {
      if (event.data.event === 'finishedTraining') {
        this.isTrainingFinished = true;
      }
    };

    // Ask parent to notify me if training is finished
    parent.postMessage({ event: 'gameTrainingState' }, '*');

    firebase.initializeApp(this.firebaseConfig);
  },
  data() {
    return {
      isTrainingFinished: false,
      shouldGiveTrainingIndication: false,
      canvasSize: 128,
      gameState: {
        scene: null,
        score: 0,
      },
      keyTranslationTable: {
        // Directions
        ArrowLeft: 1, // key-left
        ArrowUp: 2, // key-up
        ArrowRight: 4, // key-right
        ArrowDown: 8, // key-down

        // A
        Space: 16, // space maps to "A"
        Enter: 16, // enter maps to "A"
        KeyX: 16, // X maps to "A"

        // B
        KeyA: 32, // A maps to "B"
        KeyS: 32, // S maps to "B"

        // Start
        Escape: 64, // ESC maps to "START"
        KeyQ: 64, // Q maps to "START"

        // Select
        KeyW: 128, // W maps to "SELECT"
      },
      sceneTranslationTable: {
        1: 'MAINMENU',
        2: 'WAVE',
        3: 'BOSS',
        4: 'GAMEOVER',
      },
      firebaseConfig: {
        apiKey: 'AIzaSyDXuwZy2DpcKm87I6SEd-F95_yKsMAiXC4',
        authDomain: 'train-invaders.firebaseapp.com',
        projectId: 'train-invaders',
        storageBucket: 'train-invaders.appspot.com',
        messagingSenderId: '80001383742',
        appId: '1:80001383742:web:3810a38e12f198eb21406f',
      },
      isLeaderboardVisible: false,
    };
  },
  methods: {
    async createWebAssembly(importObject) {
      const decoded = atob(gameBase64);
      const bytes = new Uint8Array(decoded.length);
      for (let i = 0; i < decoded.length; i++) {
        bytes[i] = decoded.charCodeAt(i);
      }
      return WebAssembly.instantiate(bytes.buffer, importObject);
    },
    controllerUpdater(inputRef, memory) {
      let localKeyState = 0;
      const remoteKeyState = new Uint8ClampedArray(memory.buffer, inputRef, 1);

      document.addEventListener('keydown', (e) => {
        const path = e.path || (e.composedPath && e.composedPath());
        if (path.some((element) => element.tagName === 'INPUT')) return;
        if (e.code in this.keyTranslationTable) {
          e.preventDefault();
          localKeyState |= this.keyTranslationTable[e.code];
        }
      });

      document.addEventListener('keyup', (e) => {
        const path = e.path || (e.composedPath && e.composedPath());
        if (path.some((element) => element.tagName === 'INPUT')) return;

        if (e.code in this.keyTranslationTable) {
          e.preventDefault();
          localKeyState &= ~this.keyTranslationTable[e.code];
        }
      });

      return () => {
        remoteKeyState[0] = localKeyState;
      };
    },
    screenUpdater(screenRef, memory) {
      const pixelArray = new Uint8ClampedArray(
        memory.buffer,
        screenRef,
        4 * this.canvasSize * this.canvasSize,
      );
      const imgData = new ImageData(pixelArray, this.canvasSize, this.canvasSize);
      const canvas = this.$refs.canvas.getContext('2d');
      canvas.globalCompositeOperation = 'copy';

      return () => {
        canvas.putImageData(imgData, 0, 0);
      };
    },
    timeUpdater(timeRef, memory) {
      const state = new Uint32Array(memory.buffer, timeRef, 4);
      const initial_time = +new Date();
      return () => {
        state[0] = +new Date() - initial_time;
      };
    },
    initRandom(seedRef, memory) {
      const state = new Uint32Array(memory.buffer, seedRef, 4);
      state[0] = Math.ceil(Math.random() * Math.pow(2, 32));
    },
    /* NOTICE: This method is pass to C language */
    getGameState(scene, score) {
      this.gameState = {
        scene: this.sceneTranslationTable[scene],
        score,
      };
    },
    async initGame() {
      const app = await this.createWebAssembly({
        env: {
          get_game_state: this.getGameState,
        },
      });
      const { exports } = app.instance;
      exports._init();
      const screenRef = exports._get_screen_ref();
      const inputRef = exports._get_input_ref();
      const timeRef = exports._get_time_ref();
      const seedRef = exports._get_seed_ref();
      this.initRandom(seedRef, exports.memory);
      const updateController = this.controllerUpdater(inputRef, exports.memory);
      const updateScreen = this.screenUpdater(screenRef, exports.memory);
      const updateTime = this.timeUpdater(timeRef, exports.memory);
      const updateGame = exports._update;
      let skipNextFrame = false;
      function update() {
        skipNextFrame = !skipNextFrame;
        if (skipNextFrame) {
          updateController();
          updateTime();
          updateGame();
          updateScreen();
        }
        window.requestAnimationFrame(update);
      }
      update();
    },
    closeGame() {
      parent.postMessage({ event: 'gameClose' }, '*');
    },
    setLeaderboardVisibility(isVisible) {
      // No internet connection
      if (!navigator.onLine && isVisible) return;
      this.isLeaderboardVisible = isVisible;
    },
  },
  watch: {
    'gameState.scene': {
      async handler(scene, prevScene) {
        if (scene === 'GAMEOVER') {
          this.setLeaderboardVisibility(true);
        } else if (scene === 'WAVE' || scene === 'BOSS') {
          this.setLeaderboardVisibility(false);
        }
      },
    },
  },
};
</script>

<style lang="scss" scoped>
.app {
  background-image: url("./assets/bg.png");
  padding: 92px 15px 57px 15px;
  position: relative;
  background-position: top;
  background-repeat: no-repeat;
  background-color: black;
}

.training-status {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  text-align: center;
  color: white;
  margin-top: 5px;
}

.training-status-finished {
  animation: blink 1s step-start 0s 4;
}

.success-icon {
  width: 18px;
  transform: translateZ(0);
}

.inner {
  position: relative;
  display: flex;
  justify-content: center;
}

.canvas-wrapper {
  position: relative;
}

.canvas {
  image-rendering: optimizeSpeed;
  image-rendering: -moz-crisp-edges;
  image-rendering: -webkit-optimize-contrast;
  image-rendering: optimize-contrast;
  image-rendering: pixelated;
  -ms-interpolation-mode: nearest-neighbor;
  width: 512px;
  height: 512px;
  border: solid 3px white;
}

.heart-icon {
  width: 19px;
  transform: translateZ(0);
}

.aporia-credit {
  position: absolute;
  top: 100%;
  right: 0;
  width: 100%;
  text-align: center;
  font-size: 10px;
  color: white;
  margin-top: 15px;
}

.aporia-logo-wrapper {
  vertical-align: middle;
}

.aporia-logo {
  width: 87px;
  transform: translateZ(0);
}

.leaderboard-wrapper {
  pointer-events: none;
}

@media only screen and (max-width: 800px) {
  .leaderboard-wrapper {
    position: absolute;
    top: 0;
    right: 0;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
}
</style>
