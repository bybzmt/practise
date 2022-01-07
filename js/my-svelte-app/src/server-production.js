import fs from "fs"
import path from "path"
import express from "express"

import { render } from "./entry-server.js"

import { fileURLToPath } from 'url';
const __dirname = path.dirname(fileURLToPath(import.meta.url));

const app = express()

// 1. 读取 index.html
let template = fs.readFileSync(
    path.resolve(__dirname, './client/index.html'),
    'utf-8'
)

app.use(express.static(path.resolve(__dirname, './client')))

app.use('*', async (req, res) => {
    try {
        let url = req.originalUrl

        await render(req, res, template)
    } catch (e) {
        console.error(e.message)
        res.status(500).end(e.message)
    }
})

app.listen(3000)
