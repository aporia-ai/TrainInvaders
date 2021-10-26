const HtmlWebpackPlugin = require('html-webpack-plugin');
const HtmlWebpackInlineSourcePlugin = require('html-webpack-inline-source-plugin');

const URL_LOADER_LIMIT = 1000000; // 1000 kb

module.exports = {
  lintOnSave: false,
  css: {
    extract: false,
  },
  filenameHashing: false,
  chainWebpack: (config) => {
    // Show emojies
    config.optimization.minimizer("terser").tap(args => {
      const options = args[0]
      const terserOptions = options.terserOptions
      terserOptions.output = terserOptions.output || {}
      terserOptions.output.ascii_only = true
      return args
    })

    // Images to base64
    config.module
      .rule('images')
      .use('url-loader')
      .loader('url-loader')
      .tap((options) => {
        options.limit = URL_LOADER_LIMIT;
        return options;
      });

    // Fonts to base64
    config.module
      .rule('fonts')
      .test(/\.(woff2?|eot|ttf|otf)(\?.*)?$/i)
      .use('url-loader')
      .loader('url-loader')
      .tap((options) => {
        options.limit = URL_LOADER_LIMIT;
        return options;
      });
  },
  configureWebpack: {
    optimization: {
      splitChunks: false,
    },
    plugins: [
      new HtmlWebpackPlugin({
        title: 'TrainInvaders',
        template: 'public/index.html', // this is important - a template file to use for insertion
        inlineSource: '.(js|css)$', // embed all javascript and css inline
      }),
      new HtmlWebpackInlineSourcePlugin(HtmlWebpackPlugin),
    ],
  },
};
