<template>
  <div class="leaderboard">
    <div class="inner">
      <div class="title">Leaderboard</div>
      <form @submit.prevent="!user.uid ? signIn() : ''">
        <div class="header">
          <div></div>
          <div>Nickname</div>
          <div>Score</div>
        </div>
        <div class="rows">
          <div v-if="leaderboard[0] && leaderboard[0].position !== 1">...</div>
          <LeaderboardRow
            v-for="leaderboardUser in leaderboard"
            :key="leaderboardUser.uid"
            :displayName="leaderboardUser.self && readOnly ? 'YOU' : leaderboardUser.displayName"
            :score="leaderboardUser.score"
            :position="leaderboardUser.position"
            @displayNameChange="user.displayName = $event"
            :showDisplayNameInput="leaderboardUser.self && !leaderboardUser.uid && !readOnly"
            :self="leaderboardUser.self"
            :winner="isUserAWinner(leaderboardUser.position)"
          />
          <div
            v-if="
              leaderboard[leaderboard.length - 1] && !leaderboard[leaderboard.length - 1].isLast
            "
          >
            ...
          </div>
        </div>
        <div class="current-score">Current score: {{ user.score }}</div>
        <div class="encouragement" v-if="encouragementText">{{ encouragementText }}</div>
        <div v-if="!readOnly" class="buttons-wrapper">
          <button v-if="!user.uid" class="button" type="submit">
            Save & Login
          </button>
          <button v-else class="button" @click="logout" type="button">Logout</button>
        </div>
      </form>
    </div>
    <CloseButton class="close-button" @click="$emit('close')" />
    <Spinner v-if="isLoading"></Spinner>
  </div>
</template>

<script>
import Spinner from "./Spinner.vue";
import CloseButton from "./CloseButton.vue";
import LeaderboardRow from "./LeaderboardRow.vue";

export default {
  name: "Leaderboard",
  components: { LeaderboardRow, CloseButton, Spinner },
  props: {
    selfScore: Number
  },
  data() {
    return {
      database: null,
      googleClientId: "80001383742-orkfn42mavpm40an54o517hnhrslbn76.apps.googleusercontent.com",
      GoogleAuth: null,
      GoogleUser: null,
      firebaseUser: null,
      user: {
        uid: "",
        displayName: "",
        score: 0
      },
      leaderboard: [],
      isLoading: true,
      errorTimeout: null,
      readOnly: false
    };
  },
  computed: {
    encouragementText() {
      const selfFromLeaderboard = this.leaderboard.find(user => user.self);
      if (!selfFromLeaderboard) return "";
      // Logged in
      if (selfFromLeaderboard.uid) {
        // Current score is better than personal best
        if (this.user.score > selfFromLeaderboard.score) {
          return this.getTextByPosition(
            selfFromLeaderboard.position,
            "YOU ARE SUPERB",
            "So close! Great improvement",
            "Nice improvement!"
          );
        }
        if (this.user.score == selfFromLeaderboard.score) {
          // Current score is as the personal best
          return this.getTextByPosition(
            selfFromLeaderboard.position,
            "YOU ARE SUPERB",
            "So close! Next time you'll definitely succeed",
            "Nice!"
          );
        }
        // Current score lower than personal best
        return this.getTextByPosition(
          selfFromLeaderboard.position,
          "No worries! you are still THE BEST",
          "No worries! you are still (almost) the best",
          "No worries! Practice makes you perfect"
        );
      }
      // Not logged in
      // Can log in
      if (!this.readOnly) {
        return this.getTextByPosition(
          selfFromLeaderboard.position,
          "WOW! Hurry up and save your score",
          "So close! Save your score and try again",
          "Nice job! Save your score and try again"
        );
      }
      // Can't log in
      return this.getTextByPosition(selfFromLeaderboard.position, "WOW!", "So close!", "Nice job!");
    }
  },
  methods: {
    async signIn() {
      this.isLoading = true;
      try {
        this.GoogleUser = await this.GoogleAuth.signIn({
          prompt: "select_account",
          scope: "profile email"
        });
        if (this.GoogleUser) await this.getFirebaseUser();
        const oldDisplayName = this.user.displayName;
        if (this.firebaseUser) {
          await this.getUser();
          if (!this.user.displayName) {
            const generatedNickname = `Champ-${Math.random()
              .toString(36)
              .substring(7)}`;
            await this.setUserDisplayName(oldDisplayName || generatedNickname);
            this.user.displayName = oldDisplayName;
          }
          await this.updateScoreInLeaderboard();
          await this.getLeaderboard();
        }
      } finally {
        this.isLoading = false;
      }
    },
    async getFirebaseUser() {
      return new Promise(async res => {
        const unsubscribe = firebase.auth().onAuthStateChanged(async firebaseUser => {
          unsubscribe();
          if (!this.isUserEqual(firebaseUser)) {
            const credential = firebase.auth.GoogleAuthProvider.credential(
              this.GoogleUser.getAuthResponse().id_token
            );
            await firebase.auth().signInWithCredential(credential);
          }
          this.firebaseUser = firebase.auth().currentUser;
          res("");
        });
      });
    },
    isUserEqual(firebaseUser) {
      if (firebaseUser) {
        const { providerData } = firebaseUser;
        for (let i = 0; i < providerData.length; i++) {
          if (
            providerData[i].providerId === firebase.auth.GoogleAuthProvider.PROVIDER_ID &&
            providerData[i].uid === this.GoogleUser.getBasicProfile().getId()
          ) {
            // We don't need to reauth the Firebase connection.
            return true;
          }
        }
      }
      return false;
    },
    async getGoogleAuth() {
      return new Promise((res, rej) => {
        gapi.load("auth2", () => {
          let GoogleAuth = gapi.auth2.getAuthInstance();
          if (!GoogleAuth) {
            GoogleAuth = gapi.auth2.init({
              client_id: this.googleClientId,
              ux_mode: "popup"
            });
          }
          GoogleAuth.then(async () => {
            this.GoogleAuth = GoogleAuth;
            res("");
          }).catch(e => rej(e));
        });
      });
    },
    getGoogleUser() {
      const isSignedIn = this.GoogleAuth.isSignedIn.get();
      if (isSignedIn) this.GoogleUser = this.GoogleAuth.currentUser.get();
    },
    async setUserDisplayName(name) {
      const ref = await this.database.collection("userMetadata").doc(this.user.uid);
      await ref.set({
        uid: this.user.uid,
        displayName: name
      });
    },
    async getUserDisplayName(uid) {
      if (!uid) return "";
      const ref = this.database.collection("userMetadata").doc(uid);
      const row = await ref.get();
      return row.exists ? row.data().displayName : "";
    },
    async getUser() {
      const { uid } = this.firebaseUser;
      this.user.displayName = await this.getUserDisplayName(uid);
      this.user.uid = uid;
    },
    async logout() {
      this.isLoading = true;
      try {
        await this.GoogleAuth.signOut();
        await firebase.auth().signOut();
        this.GoogleUser = null;
        this.firebaseUser = null;
        this.user = Object.assign(this.user, {
          displayName: "",
          uid: ""
        });
        await this.getLeaderboard();
      } finally {
        this.isLoading = false;
      }
    },
    async updateScoreInLeaderboard() {
      const ref = this.database.collection("leaderboard").doc(this.user.uid);
      const row = await ref.get();
      if (!row.exists) {
        await ref.set({
          score: this.user.score,
          uid: this.user.uid
        });
      } else {
        const { score } = row.data();
        if (score < this.user.score) {
          await ref.update({
            score: this.user.score
          });
        }
      }
    },
    async getLeaderboard() {
      const ref = await this.database.collection("leaderboard");
      const _users = await ref.orderBy("score", "desc").get();
      const users = [];
      _users.forEach(user => {
        if (user.exists) users.push(user.data());
      });
      let selfIndex = users.findIndex(user => user.uid === this.user.uid);
      if (selfIndex < 0) {
        let indexOfUserBelow = users.findIndex(user => user.score <= this.user.score);
        if (indexOfUserBelow < 0) indexOfUserBelow = users.length;
        users.splice(indexOfUserBelow, 0, {
          uid: this.user.uid,
          score: this.user.score
        });
        selfIndex = users.findIndex(user => user.uid === this.user.uid);
      }
      users.forEach((user, userIndex) => (user.position = userIndex + 1));
      users[selfIndex].self = true;
      users[users.length - 1].isLast = true;
      let leaderboard = [
        users[selfIndex - 3],
        users[selfIndex - 2],
        users[selfIndex - 1],
        users[selfIndex],
        users[selfIndex + 1],
        users[selfIndex + 2],
        users[selfIndex + 3]
      ];
      leaderboard = leaderboard.filter(user => user);

      const promises = leaderboard.map(user => this.getUserDisplayName(user.uid));
      const usersDisplayNames = await Promise.all(promises);
      leaderboard.forEach((user, userIndex) => (user.displayName = usersDisplayNames[userIndex]));

      this.leaderboard = leaderboard;
    },
    showError(errorMessage) {
      this.errorMessage = errorMessage;
      clearTimeout(this.errorTimeout);
      this.errorTimeout = setTime(() => {
        this.errorMessage = "";
      }, 3_000);
    },
    isUserAWinner(position) {
      return position <= 2;
    },
    isUserAlmostAWinner(position) {
      return position > 2 && position <= 5;
    },
    getTextByPosition(position, winnerText, almostWinnerText, regularScoreText) {
      if (this.isUserAWinner(position)) {
        return winnerText;
      }
      if (this.isUserAlmostAWinner(position)) {
        return almostWinnerText;
      }
      // Regular score
      return regularScoreText;
    }
  },
  async mounted() {
    const isLocalhost = window.location.hostname === "localhost";
    const isPortAllowed = ["8888", "8889", "8890", "8891", "8080", "8081"].some(
      port => port === window.location.port
    );
    const isCookiesEnabled = navigator.cookieEnabled;
    this.readOnly = !(isLocalhost && isPortAllowed) || !isCookiesEnabled;
    this.user.score = this.selfScore;
    this.database = firebase.firestore();
    if (!this.readOnly) {
      try {
        await this.getGoogleAuth();
      } catch (e) {
        if (e && e.details === "Cookies are not enabled in current environment.") {
          this.readOnly = true;
        } else throw e;
      }
      if (this.GoogleAuth) this.getGoogleUser();
      if (this.GoogleUser) await this.getFirebaseUser();
      if (this.firebaseUser) await this.getUser();
      if (this.user.uid) {
        await this.updateScoreInLeaderboard();
      }
    }
    await this.getLeaderboard();
    this.isLoading = false;
  }
};
</script>

<style lang="scss" scoped>
.leaderboard {
  padding: 21px 12px;
  border-radius: 2px;
  width: 100%;
  max-width: 257px;
  background-color: white;
  position: absolute;
  top: 10px;
  left: 10px;
  font-size: 8px;
  pointer-events: all;
}

.inner {
  position: relative;
}

.title {
  text-align: center;
  margin-bottom: 20px;
  font-size: 15px;
  font-weight: bold;
}

.rows {
  margin-bottom: 15px;
}

.rows > *:not(:last-child) {
  margin-bottom: 7px;
}

.header {
  display: flex;
  align-items: center;
  margin-bottom: 7px;

  & > * {
    width: 0;
    flex-grow: 1;
    text-align: center;
  }

  & > *:nth-child(2) {
    flex-grow: 3;
  }

  & > *:not(:last-child) {
    margin-right: 5px;
  }
}

.current-score {
  text-align: center;
}

.encouragement {
  text-align: center;
  margin-top: 6px;
}

.buttons-wrapper {
  text-align: center;
  margin-top: 15px;
}

.button {
  background-color: black;
  color: white;
  padding: 4px 8px;
  border-radius: 6px;
}

@media only screen and (max-width: 800px) {
  .leaderboard {
    width: 272px;
    max-width: none;
    position: relative;
  }
}
</style>
