import { writable } from 'svelte/store';

const uri = writable('/');

function goto(n)
{
  history.pushState({}, "", n)
  uri.update(()=>n)
}

function href(n)
{
  return ()=>{
    goto(n)
  }
}

export {uri, goto, href}
