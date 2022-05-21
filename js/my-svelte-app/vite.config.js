import { defineConfig } from 'vite'
import { svelte } from '@sveltejs/vite-plugin-svelte';
import postcssImport from 'postcss-import';
import tailwindcss from 'tailwindcss';
import postcssNested from 'postcss-nested';
import autoprefixer from 'autoprefixer';
import cssnano from 'cssnano';
import path from 'path';
import postcss_url from "postcss-url";

//import { fileURLToPath } from 'url';
//const __dirname = path.dirname(fileURLToPath(import.meta.url));

let dir_src = path.resolve(__dirname, './src');
let dir_lib = path.resolve(__dirname, './src/lib');
let dir_icon = path.resolve(__dirname, './src/icons');

/**
* 自动将<img src="" />
* 处理成经vite编译的图片
* 识别符: "@import xxxx"
*/
let img_auto_import = (input) => {

	let files = new Map();
	let prefix = "asset_" + Math.floor(Math.random() * 10000 * 10000) + "_";

	let content = input.content;

	content = content.replace(/('|")\/@import\/([^'"\n\r]+)\1/g, (txt, t, src) => {
		let var_name;
		if (!files.has(src)) {
			var_name = prefix + files.size;
			files.set(src, var_name);
		} else {
			var_name = files.get(src)
		}
		let out = "{" + var_name + "}"
		return out;
	});

	if (files.size > 0) {
		let codes = [];
		files.forEach((var_name, src) => {
			let code = 'import ' + var_name + ' from "' + src + '?url";';
			codes.push(code);
		});
		codes = codes.join("\n");

		let find = false;
		content = content.replace(/<script>/, (txt) => {
			find = true;
			return txt + "\n" + codes + "\n";
		});

		if (!find) {
			content = "<script>\n" + codes + "\n</script>\n" + content;
		}
	}

	//console.log(input.filename);
	//console.log(content);

	return {
		code: content,
	}
};

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
                content:["src/**/*.svelte"]
            }),
            autoprefixer({
                cascade: true,
            }),
			postcss_url([
				{ filter: '$lib/**/*', url: 'copy', basePath:dir_lib, assetsPath:'img', useHash:true, },
				//{ filter: '**/*', url: (asset) => `https://cdn.url/${asset.url}` },
			]),
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
                $src: dir_src,
                $lib: dir_lib,
                $icon: dir_icon,
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
                preprocess: {
                    markup: img_auto_import,
				}
            }),
        ],
        server:{
            host:"0.0.0.0"
        }
    }
})
