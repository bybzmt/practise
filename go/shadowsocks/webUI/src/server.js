import { h, Component } from 'preact';

import styles from "./index.css";

export default class extends Component {
  constructor() {
    super();

    this.state = {
      Servers: [],
      edit: {
        Addr:"",
        Passwd:"",
        Cipher:"",
        Note:"",
        Enable:false,
        ID: "",
      }
    }
  }

  componentDidMount() {
    this.refresh()
  }

  refresh() {
    fetch("/api/serverConfigs").then(t=>t.json()).then(data=>{
      this.setState({Servers:data})
    });
  }

  set(x) {
      if (x) {
        this.setState({edit:{...this.state.edit, ...x}})
      } else {
        let t = {...this.state.edit};
        for (let i in t) {
          t[i] = ""
        }
        this.setState({edit:t})
      }
  }

  doSave() {
    var formData = new FormData();
    formData.append("Addr", this.state.edit.Addr)
    formData.append("Passwd", this.state.edit.Passwd)
    formData.append("Cipher", this.state.edit.Cipher)
    formData.append("Note", this.state.edit.Note)
    formData.append("Enable", this.state.edit.Enable ? "1":"")

    let url

    if (this.state.edit.ID == "") {
      url = "/api/serverConfigAdd";
    } else {
      url = "/api/serverConfigEdit";
      formData.append("ID", this.state.edit.ID)
    }

    fetch(url,{
      method: 'POST',
      body: formData,
    }).then(t=>t.text()).then(d=>{
      this.refresh()
      this.set()
    });
  }

  render(props, {Servers, edit}) {
    let c0 = e=>this.setState(this.state)


    let eid = r => () => this.set(r)

    let c1 = e=>this.set({Addr:e.target.value})
    let c2 = e=>this.set({Passwd:e.target.value})
    let c3 = e=>this.set({Cipher:e.target.value})
    let c4 = e=>this.set({Note:e.target.value})
    let c5 = e=>this.set({Enable:e.target.checked})


    let del = id=>{
      var formData = new FormData();
      formData.append("ID", id)

      fetch("/api/serverConfigDel",{
        method: 'POST',
        body: formData,
      }).then(t=>t.text()).then(d=>{
        this.refresh()
        this.set()
      });
    }

    return <table className={styles.table}>
    <tr>
      <th>Addr</th>
      <th>Password</th>
      <th>Cipher</th>
      <th>Note</th>
      <th>Enable</th>
      <th></th>
      </tr>
      { Servers.map(s => edit.ID != s.ID ? (
        <tr>
        <td>{s.Addr}</td>
        <td>{s.Passwd}</td>
        <td>{s.Cipher}</td>
        <td>{s.Note}</td>
        <td><input type="checkbox" checked={s.Enable} readonly="readonly" value="1" onchange={c0} /></td>
        <td>
          <span onclick={eid(s)}>edit</span> &nbsp;
          <span onclick={()=>del(s.ID)}>del</span>
        </td>
        </tr>
      ):(
        <tr>
        <td><input value={edit.Addr} onchange={c1} /></td>
        <td><input value={edit.Passwd} onchange={c2} /></td>
        <td><input value={edit.Cipher} onchange={c3} /></td>
        <td><input value={edit.Note} onchange={c4} /></td>
        <td><input type="checkbox" checked={edit.Enable} onchange={c5} value="1" /></td>
        <td>
          <span onclick={this.doSave.bind(this)}>Save</span> &nbsp;
          <span onclick={eid()}>cancel</span>
        </td>
        </tr>
    ))}
    {edit.ID == "" && (
      <tr>
      <td><input value={edit.Addr} onchange={c1} /></td>
      <td><input value={edit.Passwd} onchange={c2} /></td>
      <td><input value={edit.Cipher} onchange={c3} /></td>
      <td><input value={edit.Note} onchange={c4} /></td>
      <td><input type="checkbox" checked={edit.Enable} onchange={c5} value="1" /></td>
      <td><span onclick={this.doSave.bind(this)}>add</span></td>
      </tr>
    )}
      </table>;
  }
}
