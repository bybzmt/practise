import fs from 'fs'
import path from 'path'

import routes from './routes.js'

export async function render(req, res, template) {

  const url = "/" + req.originalUrl.replace(/^\/+|\/+$/, '')

  console.log("GET", url)

  let page = routes.map[url];
  if (!page) {
    res.status(404).set({ 'Content-Type': 'text/html; charset=utf-8' }).end("404 Not Found.")
    return
  }

  const app = await page();

  const loader = app.load || (()=>{return {};});
  const render = app.default.render;

  const data = await loader();

  let { head, html, css } = await render(data)

  if (css.code) {
    head+= "\n<style>\n"+css.code+"\n</style>\n";
  }

  let js = fs.readFileSync(path.resolve('./src/client-tpl.js'), 'utf-8')
  js = js.replace("{%page%}", "/pages/index.svelte")
  js = `<script type="module">\n${js}\n</script>`


  // 5. 注入渲染后的应用程序 HTML 到模板中。
  const out = template
    .replace(`<!--ssr-head-->`, head)
    .replace(`<!--ssr-body-->`, html)
    .replace(`<!--ssr-script-->`, js)


  // 6. 返回渲染后的 HTML。
  res.status(200).set({ 'Content-Type': 'text/html; charset=utf-8' }).end(out)
}
