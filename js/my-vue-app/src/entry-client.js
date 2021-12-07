import { createApp } from 'vue'
import { createRouter, createWebHistory } from 'vue-router'
import { createHead } from '@vueuse/head'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import App from './App.vue'

import routes from './routes.js'

const router = createRouter({
  history: createWebHistory(),
  routes,
})

// 5. 创建并挂载根实例
const app = createApp(App)
const head = createHead()
app.use(head)
app.use(router)
app.use(ElementPlus)

app.mount('#app')
