import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueJsx from '@vitejs/plugin-vue-jsx'

export default defineConfig(({command, mode}) =>{
  console.log("command", command, "mode", mode)

  return {
    mode:"development",
    build:{
      sourcemap:true
    },
    plugins: [
      vue(),
      vueJsx(),
    ]
  }
})
