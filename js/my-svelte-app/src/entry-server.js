
import App from './pages/index.svelte'


export async function render(req, url) {

  let d = App.render();

  return d
}
