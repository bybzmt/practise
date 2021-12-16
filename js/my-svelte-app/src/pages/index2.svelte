<script context="module">
</script>

<svelte:head>
  <title>Dashboard</title>
</svelte:head>

<script>
import {href} from '$src/lib/core/nav';

export let a=0

</script>

<main>
index2
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
