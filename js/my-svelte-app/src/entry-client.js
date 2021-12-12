import routes from './routes';
import {match} from '$src/lib/core';

(function(){
  let uri = Location.href.split('?')[0];

  let page = match(routes, uri)
  if (!page) {
    console.log("404 Not Found.");
    return;
  }

  page().then((page)=>{
    const loader = page.load || (()=>{return {};});
    const App = page.default;

    const app = new App({
      target:document.querySelector("#app"),
      hydrate: true,
    });

  }).catch((e) => {
    console.log(e)
  })



})()
