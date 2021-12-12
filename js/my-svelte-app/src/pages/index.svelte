<script context="module">
export const load = async () => {
    console.log("load1");
    return {
        props: { }
    };
};
</script>

<svelte:head>
  <title>Dashboard</title>
</svelte:head>

<main>
index
<br />
<a href="/">index</a>
<br />
<a href="/index2">index2</a>
</main>

<style>
main{
    color:red;
}
</style>
