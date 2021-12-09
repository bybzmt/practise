
export default {
  map:{
    "/": () => import('./pages/index.svelte'),
    "/index2": () => import('./pages/index2.svelte'),
  },
  matchs:[
    {
      reg: /\.*/,
      page: () => import('./pages/404.svelte'),
    },
  ]
}
