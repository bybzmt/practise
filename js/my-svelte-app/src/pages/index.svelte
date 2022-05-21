<script context="module">
    export const load = async () => {
        console.log("load1");
        return {
            props: {
                a: Math.random(),
            },
        };
    };
</script>

<script>
    export let a = 0;
</script>

<svelte:head>
    <title>index</title>
</svelte:head>

<main>
    index
    <br />
    <br />
    <a href="/">index</a>
    <br />
    <a href="/index2">index2</a>
    <br />
    <a href="/404">404</a>
</main>
{a}

<style>
    main {
        color: red;
		background:url("$lib/icons/zondicons/add-outline.svg") no-repeat;
    }
</style>
