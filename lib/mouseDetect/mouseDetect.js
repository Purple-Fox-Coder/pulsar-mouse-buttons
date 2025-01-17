// based off https://github.com/jostrander/mouse-forward-back

'use strict';

var g_mouseForwardBack;
if (process.platform === 'linux') {
  g_mouseForwardBack = require('bindings')('mouseDetectLINUX');
} else if (process.platform === 'win32') {
  g_mouseForwardBack = require('bindings')('mouseDetectWIN');
}

module.exports = {
    register: function(callback, handle) {
        g_mouseForwardBack.register(function(msg) {
          if (msg === 8) {
            callback("XButton4"); // back
          } else if (msg === 9) {
            callback("XButton5"); // forward
          }
        }, handle);
    }
}
