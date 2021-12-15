import routes from './routes';
import {App, match, setP} from '$src/lib/core';

(function(){
  let url = new URL(location.href);

  let uri = url.pathname;
  console.log(uri);

  let page = match(routes, uri)
  if (!page) {
    console.log("404 Not Found.");
    return;
  }

  page().then((page)=>{

    const loader = page.load || (()=>{return {};});

    setP(page.default)

    const app = new App({
      target:document.querySelector("#app"),
      hydrate: true,
    });
  }).catch((e) => {
    console.log(e)
  })



})()
