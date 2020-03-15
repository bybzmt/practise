import { h, render, Component } from 'preact';
import { Router, Link } from 'preact-router';
import Match from 'preact-router/match';
import {createHashHistory} from 'history';

import styles from './index.css';

import State from "./state";
import Logs from "./logs";
import Config from "./config";
import Server from "./server";
import Rules from "./rules";

class Clock extends Component {
    constructor() {
        super();
    }

    componentDidMount() {
    }

    componentWillUnmount() {
    }

    render(props, {tab}) {
      return <div>
        <nav className={styles.nav}>
          <Link href="/" >State</Link>
          <Link href="/logs" >Logs</Link>
          <Link href="/config" >Client</Link>
          <Link href="/server" >Server</Link>
          <Link href="/rules" >Rules</Link>
        </nav>
        <Router history={createHashHistory()}>
          <Match path="/" native  key="/">
          {({ matches, path, url }) => (
            <div key="/">
              <State />
              <Logs />
            </div>
          )}
          </Match>
          <Match path="/logs"  key="/logs">
          {({ matches, path, url }) => (
            <div>
              <State />
              <Logs mode={1} />
            </div>
          )}
          </Match>
          <Config path="/config" />
          <Server path="/server" />
          <Rules path="/rules" />
        </Router>
      </div>;
    }
}

// render an instance of Clock into <body>:
render(<Clock />, document.body);
