
export default [
  {
    path: '/',
    component: () => import('./pages/aa.jsx'),
    name: 'index',
  },
  {
    path: '/about',
    component: () => import('./pages/bb.jsx'),
    name: 'about',
  },
  {
    path: '/:catchAll(.*)',
    name: 'not-found',
    component: () => import('./pages/404.vue'),
  },
]
