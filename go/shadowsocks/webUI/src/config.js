import { h, Component } from 'preact';
import styles from "./index.css";

export default class extends Component {
  constructor() {
    super();

    this.state = {
      Addr: "",
      Timeout: 0,
      IdleTimeout: 0,
      Proxy: false,
      LDNS: "",
      RDNS: "",
    }
  }

  componentDidMount() {
    fetch("/api/clientConfig").then(t => t.json()).then(data => {
      this.setState(data)
    });
  }



  doSave() {
    var formData = new FormData();
    formData.append("Addr", this.state.Addr)
    formData.append("Timeout", this.state.Timeout)
    formData.append("IdleTimeout", this.state.IdleTimeout)
    formData.append("Proxy", this.state.Proxy ? "1" : "")
    formData.append("LDNS", this.state.LDNS)
    formData.append("RDNS", this.state.RDNS)

    fetch("/api/clientConfigSave", {
      method: 'POST',
      body: formData,
    }).then(t => t.json()).then(d => {
      this.setState(d)
      fetch("/api/restart")
    });
  }

  render(props, state) {
    let c1 = e => this.setState({ Addr: e.target.value })
    let c2 = e => this.setState({ Timeout: e.target.value });
    let c3 = e => this.setState({ IdleTimeout: e.target.value });
    let c4 = e => { this.setState({ Proxy: !state.Proxy }); };
    let c5 = e => this.setState({ LDNS: e.target.value });
    let c6 = e => this.setState({ RDNS: e.target.value });

    return <form>
      <div className={styles.tr}>
        <span className={styles.td}>Addr:</span> &nbsp; <input value={state.Addr} onchange={c1} />
      </div>
      <div className={styles.tr}>
        <span className={styles.td}>Timeout:</span> &nbsp; <input value={state.Timeout} onchange={c2} />
      </div>
      <div className={styles.tr}>
        <span className={styles.td}>IdleTimeout:</span> &nbsp; <input value={state.IdleTimeout} onchange={c3} />
      </div>
      <div className={styles.tr}>
        <span className={styles.td}>Default Action:</span> &nbsp;

        <label>
          <input type="radio" name="proxy" checked={state.Proxy} onchange={c4} />
          Proxy
        </label>

        <label>
          <input type="radio" name="proxy" checked={!state.Proxy} onchange={c4} />
          Direct
        </label>
      </div>
      <div className={styles.tr}>
        <span className={styles.td}>DNS(on Direct):</span> &nbsp; <textarea value={state.LDNS} onchange={c5} />
      </div>
      <div className={styles.tr}>
        <span className={styles.td}>RDNS(on Proxy):</span> &nbsp; <textarea value={state.RDNS} onchange={c6} />
      </div>
      <div>
        <input type="button" value="save & restart" onclick={this.doSave.bind(this)} />
      </div>
    </form>;
  }
}
