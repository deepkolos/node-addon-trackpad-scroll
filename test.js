const addon = require('./index.js');

console.log('Listening for trackpad events...');

addon.onScroll(event => {
  console.log('Event:', event);
});
