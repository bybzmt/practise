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

    let api_base;
    let use_cssnano = false;

    if (mode === 'production') {
        api_base = JSON.stringify("http://api.test")
        use_cssnano = true;
    } else if (mode === 'test') {
        api_base = JSON.stringify("http://api.test")
    } else {
        api_base = JSON.stringify("http://api.lan")
    }

    let postcss_config = {
        plugins: [
            postcssImport(),
            postcssNested(),
            tailwindcss({
                mode: 'jit',
                enabled: true,
                purge:["src/**/*.svelte"]
            }),
            autoprefixer({
                cascade: true,
            }),
        ],
    }

    if (use_cssnano) {
        postcss_config.plugins.push(
            cssnano({
                preset: 'default',
            })
        );
    }

    return {
        publicDir: path.resolve(__dirname, './static'),
        //http://cdn.xx.com/xxx/
        base:'/',
        root:'./src',
        define: {
            API_BASE: api_base,
        },
        build: {
            emptyOutDir:false,
            sourcemap:true,
            cssCodeSplit:false,
        },
        resolve: {
            alias: {
                $src: path.resolve(__dirname, './src'),
                $lib: path.resolve(__dirname, './src/lib'),
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
        ],
        server:{
            host:"0.0.0.0"
        }
    }
})
