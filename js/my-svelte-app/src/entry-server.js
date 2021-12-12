import fs from 'fs'
import path from 'path'
import routes from './routes';
import {match} from '$src/lib/core';

export async function render(req, res, template) {

  const uri = "/" + req.baseUrl.replace(/^\/+|\/+$/, '')

  let page = match(routes, uri)
  if (!page) {
    res.status(404).set({ 'Content-Type': 'text/html; charset=utf-8' }).end("404 Not Found.")
    console.log("GET", req.originalUrl, "404 Not Found")
    return;
  }

  console.log("GET", req.originalUrl)


  const app = await page();

  const loader = app.load || (()=>{return {};});
  const appRender = app.default.render;

  const data = await loader();

  let { head, html, css } = await appRender(data)

  if (css.code) {
    head+= "\n<style>\n"+css.code+"\n</style>\n";
  }

  // 5. 注入渲染后的应用程序 HTML 到模板中。
  const out = template
    .replace(`<!--ssr-head-->`, head)
    .replace(`<!--ssr-body-->`, html)


  // 6. 返回渲染后的 HTML。
  res.status(200).set({ 'Content-Type': 'text/html; charset=utf-8' }).end(out)
}
