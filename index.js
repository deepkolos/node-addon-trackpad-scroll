const addon = require('./build/Release/addon');

console.log('Listening for trackpad events...');

addon.onScroll(event => {
  console.log('Trackpad Event:', event);
});
