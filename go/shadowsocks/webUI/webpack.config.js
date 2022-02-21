const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');

let mode = process.env.NODE_ENV == 'development' ? "development" : "production";

console.log(mode);

module.exports = {
  entry: {
    index:'./src/index.js'
  },
  output: {
    path: path.resolve(__dirname, '../ui/dist'),
    filename: '[name].js'
  },
  mode: mode,
  devtool: "source-map",
  module: {
    rules:[
      {test: /\.(js|jsx)$/, use: ['babel-loader']},
      {test: /\.css$/, use: ['style-loader', {loader:'css-loader', options:{modules:true}}]},
      {test: /\.(png|jpe?g|gif)$/, use: ['file-loader']},
    ]
  },
  plugins: [new HtmlWebpackPlugin({
    title: "shadowsocks",
    meta: {viewport: 'width=device-width, initial-scale=1, shrink-to-fit=no'}
  })]
};
