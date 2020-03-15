import { h, Component } from 'preact';

import styles from "./index.css";

export default class extends Component {
    constructor() {
        super();

        this.state = {
          ConnNum: 0,
          Incoming:"",
          Outgoing: "",
        };
    }

    componentDidMount() {
        this.timer = setInterval(() => {
          fetch("/api/state").then(t=>t.json()).then(data=>{
            this.setState(data)
          });
        }, 500);
    }

    componentWillUnmount() {
        clearInterval(this.timer);
    }

    render(props, state) {
        return <div className={styles.state}>
          <span>ConnNum: { state.ConnNum }</span> &nbsp;
          <span>Incoming: { state.Incoming }</span> &nbsp;
          <span>Outgoing: { state.Outgoing }</span>
        </div>;
    }
}
