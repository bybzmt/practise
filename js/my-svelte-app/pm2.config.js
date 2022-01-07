
module.exports = {
    apps : [{
        "name": "test-demo",
        "script": "./dist/server-production.js",
        "instances" : "max",
        "exec_mode" : "cluster",
        "cwd" : "./",
        "restart_delay":2000,
        "autorestart": true,
        "watch" : "./dist",
    }],
    Deployment:{
        user:"www"
    }
}
