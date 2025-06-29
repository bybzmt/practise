
export default {
    map: {
        "/": {
            page: () => import('./pages/index.svelte'),
            perm: null,
        },
        "/index2": {
            page: () => import('./pages/index2.svelte'),
            perm: null,
        },
    },
    error:{
        page: () => import('./pages/404.svelte'),
        perm: null,
    }
}
