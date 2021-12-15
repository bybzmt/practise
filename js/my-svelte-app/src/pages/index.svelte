<script context="module">
export const load = async () => {
    console.log("load1");
    return {
        props: { }
    };
};
</script>
<script>
import {href} from '$src/lib/core';
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
</main>

<style>
main{
    color:red;
}
</style>
