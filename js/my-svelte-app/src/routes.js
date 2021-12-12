
export default {
  map:{
    "/": () => import('./pages/index.svelte'),
    "/index2": () => import('./pages/index2.svelte'),
  },
  e404: () => import('./pages/404.svelte'),
  preg:[
    {
      pattern: /\.*/,
      page: () => import('./pages/404.svelte'),
    },
  ]
}
