<script>
export let page = null;
export let props = {};
</script>

{#if page}
  <svelte:component this={page} {...props} />
{:else}
  404 Not Found.
{/if}
