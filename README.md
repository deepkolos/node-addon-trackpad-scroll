# node-addon-trackpad-scroll

一个用于在 macOS 上监听触控板滚动事件的 Node.js C++ 插件。

## 安装

```bash
npm install
```

## 使用

```javascript
const addon = require('node-addon-trackpad-scroll');

addon.onScroll(event => {
  console.log('Event:', event);
});
```

## 构建

```bash
npm run build
```