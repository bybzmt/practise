import { goto, loadPage, initSession } from '$src/lib/core';
import { initEvent } from '$src/lib/core/event';
import { GraphQLInit } from '$src/lib/api'
import routes from '$src/routes'
import { writable } from 'svelte/store';
import cookie from 'cookie';


export async function render(req, res, template) {

    let _url = req.protocol + "://" + req.headers.host + req.originalUrl;

    _url = new URL(_url);

    let url = writable(_url);

    let session = writable({});

    let context = new Map()

    //cookie读取
    let cookies = cookie.parse(req.headers.cookie || "")

    context.set('url', url)
    context.set('cookie', cookies)
    context.set('session', session)
    context.set('goto', goto(context))
    context.set('api', GraphQLInit(context))

    //session初始化
    let sess_data = await initSession(context);

    if (sess_data.sid != cookies.sid) {
        //cookie.serialize('sid', sid, { sameSite: 'lax', path: "/" })
        res.cookie('sid', sess_data.sid, { sameSite: 'lax', path: "/" })
    }

    session.set({ user: sess_data.user })

    let page = await loadPage(context, routes, _url)

    if (!page) {
        res.status(404).set({ 'Content-Type': 'text/html; charset=utf-8' }).end("404 Not Found.")
        console.log(req.method, req.originalUrl, "404 Not Found")
        return;
    }

    if (page.redirect) {
        let href = (new URL(page.redirect, _url)).href;
        res.status(302).set({ 'location': href }).end("redirect: " + href)
        console.log(req.method, req.originalUrl, 302, "redirect:", href)
        return;
    }

    let headers = { 'content-type': 'text/html; charset=utf-8', ...(page.headers || {}) }

    let { head, html, css } = page.render.render(page.props, { context })

    if (css.code) {
        head += "\n<style>\n" + css.code + "\n</style>\n";
    }

    let init_data = {
        sess_data,
        props: page.props,
    }

    // 5. 注入渲染后的应用程序 HTML 到模板中。
    const out = template
        .replace(`<!--ssr-head-->`, head)
        .replace(`<!--ssr-body-->`, html)
        .replace(`<!--ssr-script-->`, "<script>\nwindow._init_data_=" + JSON.stringify(init_data) + "\n</script>")

    // 6. 返回渲染后的 HTML。
    res.status(page.status || 200).set(headers).end(out)

    console.log(req.method, req.originalUrl, page.status || 200)
}
