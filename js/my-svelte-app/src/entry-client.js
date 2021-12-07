import App from './pages/index.svelte'

const app = new App({
  target:document.querySelector("#app"),
  hydrate: true,
});
