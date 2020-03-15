import { h, Component } from 'preact';

import styles from "./index.css";

export default class extends Component {
  constructor() {
    super();

    this.state.Rules = []

    this.state.edit = {
        ID: "",
        Note:"",
        Proxy:false,
        Items:"",
        Enable:false,
    }
  }

  componentDidMount() {
    this.refresh()
  }

  refresh() {
    fetch("/api/rules").then(t=>t.json()).then(data=>{
      let t = {Rules:data}
      if (data.length > 0) {
        t.edit = data[0]
      }
      this.setState(t)
    });
  }

  del(id) {
    var formData = new FormData();
    formData.append("ID", id)

    fetch("/api/ruleDel",{
      method: 'POST',
      body: formData,
    }).then(t=>t.text()).then(d=>{
      this.refresh()
    });
  }



  doSave() {
    var formData = new FormData();
    formData.append("Items", this.state.edit.Items)
    formData.append("Note", this.state.edit.Note)
    formData.append("Proxy", this.state.edit.Proxy ? "1":"")
    formData.append("Enable", this.state.edit.Enable ? "1":"")

    let url

    if (this.state.edit.ID === "") {
      url = "/api/ruleAdd";
    } else {
      url = "/api/ruleEdit";
      formData.append("ID", this.state.edit.ID)
    }

    fetch(url,{
      method: 'POST',
      body: formData,
    }).then(t=>t.text()).then(d=>{
      this.refresh()
    });
  }

  render(props, {Rules, edit}) {

    let set = x => {
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

    let c1 = e=>set({Note:e.target.value})
    let c2 = e=>set({Proxy: !edit.Proxy})
    let c3 = e=>set({Enable:e.target.checked})
    let c4 = e=>set({Items:e.target.value})

    return <div className={styles.rules}>
      <div className={styles.rulesNav}>
      {Rules.map(r=>(r.ID===this.state.edit.ID) ?
        <span><b>{r.Note}</b></span>
       :
        <span onclick={()=>set(r)}>{r.Note}</span>
      )}
      &nbsp;
    <span onclick={()=>set()}>Add</span>
      </div>
      <div>
      <div className={styles.tr}>
      <span className={styles.td}>Note</span>
      <input value={edit.Note} onchange={c1} />
      </div>
      <div className={styles.tr}>
      <span className={styles.td}>Action:</span>
        <label>
        <input type="radio" name="proxy" checked={edit.Proxy} onchange={c2} /> Proxy
        </label>
        <label>
        <input type="radio" name="proxy" checked={!edit.Proxy} onchange={c2} /> Direct
        </label>
      </div>
      <div className={styles.tr}>
      <span className={styles.td}>Enable</span>
      <input type="checkbox" checked={edit.Enable} onchange={c3} />
      </div>
      <div>
      <textarea className={styles.td} value={edit.Items} onchange={c4} />
      </div>
      <div>
      <input type="button" value="Save" onclick={()=>this.doSave()} /> &nbsp;
    {edit.ID !== "" &&
        <input type="button" value="Del" onclick={()=>this.del(edit.ID)} />
    }
      </div>
      </div>
      </div>

  }
}
