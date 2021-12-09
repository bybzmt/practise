import { defineConfig } from 'vite'
import { svelte } from '@sveltejs/vite-plugin-svelte';
import postcssImport from 'postcss-import';
import tailwindcss from 'tailwindcss';
import postcssNested from 'postcss-nested';
import autoprefixer from 'autoprefixer';
import cssnano from 'cssnano';

import path from 'path';
//import { fileURLToPath } from 'url';
//const __dirname = path.dirname(fileURLToPath(import.meta.url));

export default defineConfig(({command, mode}) =>{
  console.log("command", command, "mode", mode)

  const dev = mode === 'development';

  let postcss_config = {
    plugins: [
      postcssImport(),
      postcssNested(),
      tailwindcss({
        purge:["src/**/*.svelte"]
      }),
      autoprefixer({
        cascade: true,
      }),
    ],
  }

  if (!dev) {
    postcss_config.plugins.push(
      cssnano({
        preset: 'default',
      })
    );
  }

  return {
    mode:"development",
    publicDir:'./public',
    root:'./src',
    build:{
      sourcemap:true,
    },
    resolve: {
      alias: {
        $src: path.resolve(__dirname, './src'),
        $icon: path.resolve(__dirname, './src/icons'),
      }
    },
    css:{
      postcss: postcss_config,
      preprocessorOptions:{},
    },
    plugins: [
      svelte({
        compilerOptions:{
          hydratable:true,
        },
        disableDependencyReinclusion:true,
        extensions:[".svelte"],
        //useVitePreprocess:true,
      }),
    ]
  }
})
