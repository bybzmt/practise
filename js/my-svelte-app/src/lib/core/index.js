
export function match(routes, uri) {
  let page = routes.map[uri];
  if (page) {
    return page
  }

  return null;
}

export async function loadPage(routes, uri) {
  let page = routes.map[uri];
  if (page) {
    let p = await page()

    const loader = p.load || (()=>{return {};});

    let x = await loader()

    return {
      page: p.default,
      props: x,
    }
  }

  return null;
}
