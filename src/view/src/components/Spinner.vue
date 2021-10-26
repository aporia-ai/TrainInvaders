<template>
  <div class="spinner">
    <div class="icon"></div>
  </div>
</template>

<script>
export default {
  name: 'Spinner',
};
</script>

<style lang="scss" scoped>
.spinner {
  background-color: rgba(0, 0, 0, 0.7);
  border-radius: inherit;
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
}

.icon {
  border: 6px solid #f3f3f3;
  border-top: 5px solid wheat;
  border-radius: 50%;
  width: 35px;
  height: 35px;
  animation: spin 2s linear infinite;
}
</style>
