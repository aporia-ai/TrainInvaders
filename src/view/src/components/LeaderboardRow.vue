<template>
  <div class="leaderboard-row" :class="{ self, winner }">
    <div :title="position">
      <div v-if="position === 1" class="position-crown">&#128081;</div>
      <div v-else>{{ position }}</div>
    </div>
    <div>
      <div class="display-name-input-wrapper" v-if="showDisplayNameInput">
        <input
          ref="input"
          type="text"
          class="display-name-input"
          placeholder="."
          @input="$emit('displayNameChange', $event.target.value)"
        />
        <div class="display-name-input-placeholder">Your nickname</div>
      </div>
      <div v-else :title="displayName" class="display-name">{{ displayName }}</div>
    </div>
    <div :title="score">{{ score }}</div>
  </div>
</template>

<script>
export default {
  name: "LeaderboardRow",
  props: {
    showDisplayNameInput: Boolean,
    displayName: String,
    score: Number,
    position: Number,
    self: Boolean,
    winner: Boolean
  },
  mounted() {
    if (this.showDisplayNameInput) this.$refs.input.focus();
  }
};
</script>

<style lang="scss" scoped>
.leaderboard-row {
  display: flex;
  align-items: center;
  border: solid 1px grey;
  border-radius: 4px;
  padding: 9px 7px;

  &.self {
    box-shadow: 0px 5px 8px -3px #000000;
    background-color: black;
    color: white;
  }
  &.winner {
    background-color: wheat;
    color: black;
  }
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

.display-name {
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.display-name-input-wrapper {
  position: relative;
  width: 100%;
}

.display-name-input {
  width: 100%;
  text-align: left;
}

.display-name-input::placeholder {
  opacity: 0.1;
}

.display-name-input-placeholder {
  content: "Your nickname";
  position: absolute;
  bottom: 1px;
  left: 0;
  animation: blink 1s step-start 0s infinite;
  pointer-events: none;
}

.display-name-input:not(:placeholder-shown) + .display-name-input-placeholder {
  visibility: hidden;
}

.position-crown {
  font-size: 15px;
  line-height: 0;
  height: 4px;
}
</style>
