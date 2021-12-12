

export function match(routes, uri) {
  let page = routes.map[uri];
  if (page) {
    return page
  }

  return null;
}
