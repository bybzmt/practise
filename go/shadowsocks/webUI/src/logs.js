import { h, Component } from 'preact';

import styles from "./index.css";

export default class extends Component {
  constructor() {
    super();

    this.state = {
      logs: []
    };
  }

  componentDidMount() {
    let aa = this.props.mode ? () => {
      let url = "/api/log?length=1000";

      fetch(url).then(t=>t.json()).then(data=>{
          this.setState({logs:data})
      })
    } : ()=>{
      let url = "/api/log?type=1&length=1000";

      fetch(url).then(t=>t.json()).then(data=>{
          this.setState({logs:data})
      })
    };

    aa();

    this.timer = setInterval(aa, 500);
  }

  componentWillUnmount() {
    clearInterval(this.timer);
  }

  render(props, {logs}) {
    return <table className={styles.table} >
      { logs.map(log => <tr>
        <td>{ log.ID }</td>
        <td>{ (new Date(log.Now)).toLocaleString() }</td>
        <td>{ log.Proxy ? "Proxy":"Direct" }</td>
        <td>{ log.From }</td>
        <td>{ log.To }</td>
        <td>{ log.Msg }</td>
        </tr>
      ) }
      </table>;
  }
}
