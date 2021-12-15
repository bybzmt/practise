import router, {setPage} from "./app.svelte"

import routes from "$src/routes"

export const App = router
export const setP = setPage

export function match(routes, uri) {
  let page = routes.map[uri];
  if (page) {
    return page
  }

  return null;
}

export function goto(uri) {
  let f = match(routes, uri)
  if (!f) {
    console.log("404")
    return
  }

  f().then((p) => {
      setPage(p.default)
  }).catch((e)=>{
    console.log(e)
  })
}

export function href(uri) {
  return ()=>{
    console.log("href", uri)

    goto(uri)
  }
}
