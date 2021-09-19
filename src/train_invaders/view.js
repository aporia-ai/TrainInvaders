const Spinner = Vue.component('Spinner', {
    template: `
        <div class="spinner">
            <div class="spinner-icon"></div>
        </div>
    `
})

const CloseButton = Vue.component('CloseButton', {
    template: `
        <button title="Close" class="close-button" @click="$emit('click')">X</button>
    `
})

const LeaderboardRow = Vue.component('LeaderboardRow', {
    props: {
        showDisplayNameInput: Boolean,
        displayName: String,
        score: Number,
        position: Number,
    },
    template: `
        <div class="leaderboard-row">
            <div :title="position">
                <div v-if="position === 1" class="leaderboard-row-position-crown">&#128081;</div>
                <div v-else>{{ position }}</div>
            </div>
            <div>
                <div class="leaderboard-row-display-name-input-wrapper" v-if="showDisplayNameInput" >
                    <input type="text" class="leaderboard-row-display-name-input" placeholder="." @input="$emit('displayNameChange', $event.target.value)">
                    <div class="leaderboard-row-display-name-input-placeholder">Your nickname</div>
                </div>
                <div v-else :title="displayName" class="leaderboard-row-display-name">{{ displayName }}</div>
            </div>
            <div :title="score">{{ score }}</div>
        </div>
    `
})

const Leaderboard = Vue.component('Leaderboard', {
    components: { LeaderboardRow, CloseButton, Spinner },
    props: {
        selfScore: Number,
    },
    data() {
        return {
            database: null,
            googleClientId: '80001383742-orkfn42mavpm40an54o517hnhrslbn76.apps.googleusercontent.com',
            GoogleAuth: null,
            GoogleUser: null,
            firebaseUser: null,
            user: {
                uid: '',
                displayName: '',
                score: 0,
            },
            leaderboard: [],
            isLoading: true,
            errorTimeout: null,
            readOnly: false,
        }
    },
    computed: {
        encouragementText() {
            const selfFromLeaderboard = this.leaderboard.find(user => user.self)
            if(!selfFromLeaderboard) return ''
            // Logged in
            if(selfFromLeaderboard.uid) {
                // Current score is better than personal best
                if(this.user.score > selfFromLeaderboard.score) {
                    return this.getTextByPosition(
                        selfFromLeaderboard.position,
                        'YOU ARE SUPERB',
                        'So close! Great improvement',
                        'Nice improvement!'
                    )
                } else if(this.user.score == selfFromLeaderboard.score) { // Current score is as the personal best
                    return this.getTextByPosition(
                        selfFromLeaderboard.position,
                        'YOU ARE SUPERB',
                        `So close! Next time you'll definitely succeed`,
                        'Nice!'
                    )
                } else { // Current score lower than personal best
                    return this.getTextByPosition(
                        selfFromLeaderboard.position,
                        'No worries! you are still THE BEST',
                        `No worries! you are still (almost) the best`,
                        'No worries! Practice makes you perfect'
                    )
                }
            } else { // Not logged in
                // Can log in
                if(!this.readOnly) {
                    return this.getTextByPosition(
                        selfFromLeaderboard.position,
                        'WOW! Hurry up and save your score',
                        'So close! Save your score and try again',
                        'Nice job! Save your score and try again'
                    )
                } else { // Can't log in
                    return this.getTextByPosition(
                        selfFromLeaderboard.position,
                        'WOW!',
                        'So close!',
                        'Nice job!'
                    )
                }
            }
        },
    },
    methods: {
        async signIn() {
            this.isLoading = true
            try {
                this.GoogleUser = await this.GoogleAuth.signIn({
                    prompt: 'select_account',
                    scope: 'profile email',
                })
                if (this.GoogleUser) await this.getFirebaseUser()
                const oldDisplayName = this.user.displayName
                if (this.firebaseUser) {
                    await this.getUser()
                    if (!this.user.displayName) {
                        const generatedNickname = `Champ-${Math.random().toString(36).substring(7)}`
                        await this.setUserDisplayName(oldDisplayName || generatedNickname)
                        this.user.displayName = oldDisplayName
                    }
                    await this.updateScoreInLeaderboard()
                    await this.getLeaderboard()
                }
            } finally {
                this.isLoading = false
            }
        },
        async getFirebaseUser() {
            return new Promise(async res => {
                const unsubscribe = firebase.auth().onAuthStateChanged(async (firebaseUser) => {
                    unsubscribe();
                    if (!this.isUserEqual(firebaseUser)) {
                        const credential = firebase.auth.GoogleAuthProvider.credential(
                            this.GoogleUser.getAuthResponse().id_token);
                        await firebase.auth().signInWithCredential(credential)
                    }
                    this.firebaseUser = firebase.auth().currentUser
                    res('')
                });
            })
        },
        isUserEqual(firebaseUser) {
            if (firebaseUser) {
                var providerData = firebaseUser.providerData;
                for (var i = 0; i < providerData.length; i++) {
                    if (providerData[i].providerId === firebase.auth.GoogleAuthProvider.PROVIDER_ID &&
                        providerData[i].uid === this.GoogleUser.getBasicProfile().getId()) {
                        // We don't need to reauth the Firebase connection.
                        return true;
                    }
                }
            }
            return false;
        },
        async getGoogleAuth() {
            return new Promise(res => {
                gapi.load('auth2', () => {
                    let GoogleAuth = gapi.auth2.getAuthInstance()
                    if (!GoogleAuth) {
                        GoogleAuth = gapi.auth2.init({
                            client_id: this.googleClientId,
                            ux_mode: 'popup',
                        })
                    }
                    GoogleAuth.then(async () => {
                        this.GoogleAuth = GoogleAuth
                        res('')
                    })
                });
            })
        },
        getGoogleUser() {
            const isSignedIn = this.GoogleAuth.isSignedIn.get()
            if (isSignedIn) this.GoogleUser = this.GoogleAuth.currentUser.get()
        },
        async setUserDisplayName(name) {
            const ref = await this.database.collection('userMetadata').doc(this.user.uid);
            await ref.set({
                uid: this.user.uid,
                displayName: name,
            })
        },
        async getUserDisplayName(uid) {
            if (!uid) return ''
            const ref = this.database.collection('userMetadata').doc(uid);
            const row = await ref.get()
            return row.exists ? row.data().displayName : ''
        },
        async getUser() {
            const { uid } = this.firebaseUser
            this.user.displayName = await this.getUserDisplayName(uid)
            this.user.uid = uid
        },
        async logout() {
            this.isLoading = true
            try {
                await this.GoogleAuth.signOut()
                await firebase.auth().signOut()
                this.GoogleUser = null
                this.firebaseUser = null
                this.user = Object.assign(this.user, {
                    displayName: '',
                    uid: '',
                })
                await this.getLeaderboard()
            } finally {
                this.isLoading = false
            }
        },
        async updateScoreInLeaderboard() {
            const ref = this.database.collection(`leaderboard`).doc(this.user.uid)
            const row = await ref.get()
            if (!row.exists) {
                await ref.set({
                    score: this.user.score,
                    uid: this.user.uid,
                })
            } else {
                const { score } = row.data()
                if (score < this.user.score) {
                    await ref.update({
                        score: this.user.score
                    })
                }
            }
        },
        async getLeaderboard() {
            const ref = await this.database.collection(`leaderboard`)
            const _users = await ref.orderBy('score', 'desc').get()
            const users = []
            _users.forEach(user => { if (user.exists) users.push(user.data()) })
            let selfIndex = users.findIndex(user => user.uid === this.user.uid)
            if (selfIndex < 0) {
                let indexOfUserBelow = users.findIndex(user => user.score <= this.user.score)
                if (indexOfUserBelow < 0) indexOfUserBelow = users.length
                users.splice(indexOfUserBelow, 0, {
                    uid: this.user.uid,
                    score: this.user.score,
                })
                selfIndex = users.findIndex(user => user.uid === this.user.uid)
            }
            users.forEach((user, userIndex) => user.position = userIndex + 1)
            users[selfIndex].self = true
            users[users.length - 1].isLast = true
            let leaderboard = [
                users[selfIndex - 3],
                users[selfIndex - 2],
                users[selfIndex - 1],
                users[selfIndex],
                users[selfIndex + 1],
                users[selfIndex + 2],
                users[selfIndex + 3],
            ]
            leaderboard = leaderboard.filter(user => user)


            const promises = leaderboard.map(user => this.getUserDisplayName(user.uid))
            const usersDisplayNames = await Promise.all(promises)
            leaderboard.forEach((user, userIndex) => user.displayName = usersDisplayNames[userIndex])

            this.leaderboard = leaderboard
        },
        showError(errorMessage) {
            this.errorMessage = errorMessage
            clearTimeout(this.errorTimeout)
            this.errorTimeout = setTime(() => {
                this.errorMessage = ''
            }, 3_000)
        },
        isUserAWinner(position) {
            return position <= 2
        },
        isUserAlmostAWinner(position) {
            return position > 2 && position <= 5
        },
        getTextByPosition(position, winnerText, almostWinnerText, regularScoreText) {
            if(this.isUserAWinner(position)) {
                return winnerText
            } else if(this.isUserAlmostAWinner(position)) {
                return almostWinnerText
            } else { // Regular score
                return regularScoreText
            }
        }
    },
    async mounted() {
        this.readOnly = !(
            window.location.hostname === 'localhost'
            && ['8888', '8889', '8890', '8891', '8080', '8081'].some(port => port === window.location.port)
        )
        this.user.score = this.selfScore
        this.database = firebase.firestore();
        if(!this.readOnly) {
            await this.getGoogleAuth()
            if (this.GoogleAuth) this.getGoogleUser()
            if (this.GoogleUser) await this.getFirebaseUser()
            if (this.firebaseUser) await this.getUser()
            if (this.user.uid) {
                await this.updateScoreInLeaderboard()
            }
        }
        await this.getLeaderboard()
        this.isLoading = false
    },
    template: `
        <div class="leaderboard">
            <div class="leaderboard-inner">
                <div class="leaderboard-title">Leaderboard</div>
                <div>
                    <div class="leaderboard-header">
                        <div></div>
                        <div>Nickname</div>
                        <div>Score</div>
                    </div>
                    <div class="leaderboard-rows">
                        <div v-if="leaderboard[0] && leaderboard[0].position !== 1">...</div>
                        <LeaderboardRow
                            v-for="leaderboardUser in leaderboard"
                            :key="leaderboardUser.uid"
                            :displayName="(leaderboardUser.self && readOnly) ? 'YOU' : leaderboardUser.displayName"
                            :score="leaderboardUser.score"
                            :position="leaderboardUser.position"
                            @displayNameChange="user.displayName = $event"
                            :showDisplayNameInput="leaderboardUser.self && !leaderboardUser.uid && !readOnly"
                            :class="{
                                self: leaderboardUser.self,
                                winner: isUserAWinner(leaderboardUser.position),
                            }"
                        />
                        <div v-if="
                            leaderboard[leaderboard.length - 1] && !leaderboard[leaderboard.length - 1].isLast
                        ">...</div>
                    </div>
                    <div class="leaderboard-current-score">Current score: {{ user.score }}</div>
                    <div class="leaderboard-encouragement" v-if="encouragementText">{{ encouragementText }}</div>
                    <div v-if="!readOnly" class="leaderboard-buttons-wrapper">
                        <button v-if="!user.uid" class="leaderboard-button" @click="signIn">
                            Save & Login
                        </button>
                        <button v-else class="leaderboard-button" @click="logout">Logout</button>
                    </div>
                </div>
            </div>
            <CloseButton class="leaderboard-close-button" @click="$emit('close')" />
            <Spinner v-if="isLoading"></Spinner>
        </div>
    `
})

new Vue({
    el: "#app",
    components: { Leaderboard },
    mounted() {

        this.shouldGiveTrainingIndication = typeof window !== 'undefined' && 'Jupyter' in parent.window

        this.initGame()

        window.onmessage = (event) => {
            if (event.data.event === "finishedTraining") {
                this.isTrainingFinished = true
            }
        };

        // Ask parent to notify me if training is finished
        parent.postMessage({ event: `gameTrainingState` }, "*");

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
                37: 1, // key-left
                38: 2, // key-up
                39: 4, // key-right
                40: 8, // key-down

                // A
                32: 16, // space maps to "A"
                13: 16, // enter maps to "A"
                88: 16, // X maps to "A"

                // B
                65: 32, // A maps to "B"
                83: 32, // S maps to "B"

                // Start
                27: 64, // ESC maps to "START"
                81: 64, // Q maps to "START"

                // Select
                87: 128, // W maps to "SELECT"
            },
            sceneTranslationTable: {
                1: "MAINMENU",
                2: "WAVE",
                3: "BOSS",
                4: "GAMEOVER",
            },
            firebaseConfig: {
                apiKey: "AIzaSyDXuwZy2DpcKm87I6SEd-F95_yKsMAiXC4",
                authDomain: "train-invaders.firebaseapp.com",
                projectId: "train-invaders",
                storageBucket: "train-invaders.appspot.com",
                messagingSenderId: "80001383742",
                appId: "1:80001383742:web:3810a38e12f198eb21406f"
            },
            isLeaderboardVisible: false,
        }
    },
    methods: {
        async createWebAssembly(importObject) {
            const decoded = atob("$$GAME_BINARY_BASE64$$");
            const bytes = new Uint8Array(decoded.length);
            for (var i = 0; i < decoded.length; i++) {
                bytes[i] = decoded.charCodeAt(i);
            }
            return WebAssembly.instantiate(bytes.buffer, importObject);
        },
        controllerUpdater(inputRef, memory) {
            let localKeyState = 0;
            let remoteKeyState = new Uint8ClampedArray(memory.buffer, inputRef, 1);

            document.addEventListener("keydown", (e) => {
                if (e.path.some(element => element.tagName === 'INPUT')) return

                if (e.keyCode in this.keyTranslationTable) {
                    e.preventDefault();
                    localKeyState |= this.keyTranslationTable[e.keyCode];
                }
            });

            document.addEventListener("keyup", (e) => {
                if (e.path.some(element => element.tagName === 'INPUT')) return

                if (e.keyCode in this.keyTranslationTable) {
                    e.preventDefault();
                    localKeyState &= ~this.keyTranslationTable[e.keyCode];
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
                4 * this.canvasSize * this.canvasSize
            );
            const imgData = new ImageData(pixelArray, this.canvasSize, this.canvasSize);
            const canvas = this.$refs.canvas.getContext("2d");
            canvas.globalCompositeOperation = "copy";

            return () => {
                canvas.putImageData(imgData, 0, 0);
            };
        },
        timeUpdater(timeRef, memory) {
            let state = new Uint32Array(memory.buffer, timeRef, 4);
            const initial_time = +new Date();
            return () => {
                state[0] = +new Date() - initial_time;
            };
        },
        initRandom(seedRef, memory) {
            let state = new Uint32Array(memory.buffer, seedRef, 4);
            state[0] = Math.ceil(Math.random() * Math.pow(2, 32));
        },
        /* NOTICE: This method is pass to C language */
        getGameState(scene, score) {
            this.gameState = {
                scene: this.sceneTranslationTable[scene],
                score,
            }
        },
        async initGame() {
            const app = await this.createWebAssembly({
                env: {
                    get_game_state: this.getGameState,
                },
            });
            const exports = app.instance.exports;
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
            parent.postMessage({ event: `gameClose` }, "*");
        },
        setLeaderboardVisibility(isVisible) {
            // No internet connection
            if(!navigator.onLine && isVisible) return
            this.isLeaderboardVisible = isVisible
        }
    },
    watch: {
        'gameState.scene': {
            async handler(scene, prevScene) {
                if (scene === 'GAMEOVER') {
                    this.setLeaderboardVisibility(true)
                } else if (scene === 'WAVE' || scene === 'BOSS') {
                    this.setLeaderboardVisibility(false)
                }
            },
        }
    },
    template: `
        <div id="app">
            <div v-if="shouldGiveTrainingIndication" class="training-status">
                <div v-if="isTrainingFinished" class="training-status-finished">
                    Training is finished <img src="data:image/gif;base64,$$GAME_SUCCESS_ICON$$" alt="Success icon" class="success-icon">
                </div>
                <div v-else>Training...</div>
            </div>
            <CloseButton @click="closeGame" />
            <div class="inner">
                <div class="canvas-wrapper">
                    <canvas id="canvas" ref="canvas" width="128" height="128"></canvas>
                    <div class="aporia-credit">
                        Brought to you with <img src="data:image/gif;base64,$$GAME_HEART_ICON$$" alt="Heart icon" class="heart-icon"> By
                        <a href="https://www.aporia.com/?utm_source=train-invaders&utm_medium=game&utm_campaign=train-invaders" target="_blank" class="aporia-logo-wrapper">
                            <img src="data:image/gif;base64,$$GAME_APORIA_LOGO$$" alt="Aporia logo" class="aporia-logo">
                        </a>
                    </div>
                </div>
            </div>
            <div class="leaderboard-wrapper">
                <Leaderboard v-if="isLeaderboardVisible" :selfScore="gameState.score" @close="setLeaderboardVisibility(false)" />
            </div>
        </div>
    `,
});
