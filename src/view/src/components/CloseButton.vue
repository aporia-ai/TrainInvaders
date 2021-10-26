<template>
  <button title="Close" class="close-button" @click="$emit('click')">X</button>
</template>

<script>
export default {
  name: "CloseButton"
};
</script>

<style lang="scss" scoped>
.close-button {
  position: absolute;
  top: 0;
  right: 0;
  background-color: red;
  color: white;
  width: 17px;
  height: 17px;
  font-size: 9px;
  display: inline-flex;
  justify-content: center;
  align-items: center;
  margin-top: 3px;
  padding-left: 1px;
  margin-right: 3px;
}
</style>
