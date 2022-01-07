import { goto, loadPage, initSession } from '$src/lib/core';
import { initEvent } from '$src/lib/core/event';
import { GraphQLInit } from '$src/lib/api'
import routes from '$src/routes'
import { writable } from 'svelte/store';
import cookie from 'cookie';


async function render(target) {

    let _url = new URL(location.href);

    let url = writable(_url);

    let session = writable({});

    let context = new Map()

    //cookie读取
    let cookies = cookie.parse(document.cookie)

    context.set('url', url)
    context.set('cookie', cookies)
    context.set('session', session)
    context.set('goto', goto(context))
    context.set('api', GraphQLInit(context))

    let sess_data;
    let init_data = window._init_data_;

    if (init_data) {
        context.set('init_props', init_data.props)

        sess_data = init_data.sess_data || {};
    } else {
        sess_data = await initSession(context);
    }

    session.set({ user: sess_data.user })

    if (sess_data.sid != cookies.sid) {
        cookies.sid = sess_data.sid
        document.cookie = cookie.serialize('sid', sess_data.sid, { sameSite: 'lax', path: "/" })
    }

    let app;

    function onload(page) {
        if (!page) {
            return;
        }

        if (page.redirect) {
            let goto = context.get('goto')
            goto(page.redirect)
            return;
        }

        if (page.headers) {
            for (let key in page.headers) {
                document.cookie = cookie.serialize(key, String(page.header[key]), { sameSite: 'lax', path: "/" })
            }
        }

        if (app instanceof page.render) {
            app.$set(page.props)
        } else {
            if (app) {
                app.$destroy()
            }

            app = new page.render({
                target,
                hydrate: true,
                props: page.props,
                context,
            });
        }
    }

    url.subscribe(url => {
        _url = url
        loadPage(context, routes, url).then(onload)
    });

    initEvent(context)
}


render(document.querySelector("#app"))
