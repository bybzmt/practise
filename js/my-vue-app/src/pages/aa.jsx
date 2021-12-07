import { ref } from 'vue'


export default {
    data() {
        return {
            count:0
        }
    },

    render() {
        const inc = () => {
            this.count++;
        };

        return (
            <h1 onClick={inc}>AA:{this.count}</h1>
        )
    }
}


