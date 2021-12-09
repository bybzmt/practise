import page from '{%page%}';

const app = new page({
  target:document.querySelector("#app"),
  hydrate: true,
});
