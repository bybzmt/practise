import Msg from './msg.svelte'
import { get as store_get } from 'svelte/store';
import cookie from 'cookie';

function match(routes, url) {
  let uri = url.pathname

  let page = routes.map[uri];
  if (page) {
    return page
  }

    if (routes.error) return routes.error

  return null
}

async function loadPage(context, routes, url) {
  let res = match(routes, url)

  if (!res) {
    return {
      status: 404,
      render: Msg,
      props: { msg: "404 Not Found." }
    }
  }

  let _sess = context.get('session')
  let api = context.get('api')
  let session = store_get(_sess)

  if (res.perm) {
    if (!session.user ||
      (session.user.id != 1 && !session.user.perms.find(x => x == res.perm))
    ) {
      if (!session.user) {
        return {
          redirect: "/login",
        }
      } else {
        return {
          render: Msg,
          props: { msg: "您没有权限" }
        }
      }
    }
  }

  let p = await res.page()

  let input = {
    url,
    query: url.searchParams,
    session,
    api,
  }

  let init_props = context.get('init_props')
  let resp;

  if (init_props) {
    context.set('init_props', null)
    resp = { props: init_props }
  } else {
    resp = p.load ? await p.load(input) : {}
  }

  if (resp instanceof Error) {
    return {
      status: 404,
      render: Msg,
      props: { msg: resp.message }
    }
  }

  return {
    status: resp.status || 200,
    redirect: resp.redirect,
    headers: resp.headers,
    render: p.default,
    props: resp.props,
  }
}

function goto(context) {
  return function (href) {
    let url = context.get('url');

    url.update((_url) => {
      let x = new URL(href, _url);
      history.pushState({}, "", x.href)
      return x
    })
  }
}


async function initSession(context) {
    return {}

  //session初始化
  let query = `
{
  admin{
    sid
    currentUser{
      id
      name
      perms:permissions
    }
  }
}
  `;

  let api = context.get("api")

  let resp = await api(query, {})

  return {
    sid: resp.data.admin.sid,
    user: resp.data.admin.currentUser,
  }
}

export { goto, match, loadPage, initSession }
