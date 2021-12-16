import {loadPage} from '$src/lib/core';
import {uri} from '$src/lib/core/nav';
import App from '$src/lib/core/app.svelte';
import routes from '$src/routes'


function render() {

  let url = new URL(location.href);

  let path = url.pathname;

  console.log(path);

  uri.set(path)

  loadPage(routes, path).then((d)=>{
    const props = d ? d : {page:null, props:{}}

    const app = new App({
      target:document.querySelector("#app"),
      hydrate: true,
      props,
    });

    uri.subscribe(value => {
      console.log("goto:", value);
      loadPage(routes, value).then((d) => {
        const props = d ? d : {page:null, props:{}}
        app.$set(props)
      })
    });
  })

  window.addEventListener('popstate', (event) => {
    let url = new URL(location.href);
    uri.set(url.pathname);
  });

}


render()
