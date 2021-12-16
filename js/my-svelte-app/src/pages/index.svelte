<script context="module">
export const load = async () => {
    console.log("load1");
    return {
      a:Math.random()
    };
};
</script>
<script>
import {href} from '$src/lib/core/nav';

export let a=0

</script>

<svelte:head>
  <title>Dashboard</title>
</svelte:head>

<main>
index
<br />
<a on:click={href("/")}>index</a>
<br />
<a on:click={href("/index2")}>index2</a>
<br />
<a on:click={href("/404")}>404</a>
</main>
{a}

<style>
main{
    color:red;
}
</style>
