import { createSSRApp } from 'vue'
import { renderToString } from '@vue/server-renderer'
import { createRouter, createMemoryHistory } from 'vue-router'
import { createHead, renderHeadToString } from '@vueuse/head'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'

import App from './App.vue'
import routes from './routes.js'

export async function render(url) {
  const app = createSSRApp(App)
  const head = createHead()

  const router = createRouter({
    history: createMemoryHistory(),
    routes,
  })

  app.use(head)
  app.use(ElementPlus)
  app.use(router)

  router.push(url)

  await router.isReady()

  const ctx={}

  const body = await renderToString(app, ctx)

  const { headTags, htmlAttrs, bodyAttrs } = renderHeadToString(head)

  return { headTags, htmlAttrs, bodyAttrs, body }
}
